#include "net/net.h"
#include "core/fsutil.h"

#include <3ds.h>
#include <curl/curl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"

#define HTTP_USER_AGENT "NDS-Shop/" APP_VERSION_STR " (Nintendo 3DS)"
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

// Bulk downloads go through httpc: the system stack runs on the wireless
// hardware and is far cheaper than libcurl+mbedTLS on the ARM11. Large chunks
// and a large stdio buffer keep the SD card writes few and sequential, which is
// what actually limits throughput on real hardware.
#define DL_RECV_SIZE    (64 * 1024)
#define DL_FILE_BUFSIZE (512 * 1024)
#define DL_TIMEOUT_NS   (15ULL * 1000000000ULL)
#define DL_MAX_REDIRECTS 4

static void  *s_soc_buffer = NULL;
static bool   s_ready = false;
static char   s_error[256];

struct MemSink {
    char  *data;
    size_t len;
    size_t cap;
};

struct DlMeter {
    u64    tick;
    size_t bytes;
    bool   started;
    float  speed;
};

static void set_error(const char *fmt, const char *arg)
{
    if (arg != NULL) snprintf(s_error, sizeof(s_error), fmt, arg);
    else             snprintf(s_error, sizeof(s_error), "%s", fmt);
}

static void set_error_status(const char *prefix, long status)
{
    snprintf(s_error, sizeof(s_error), "%s HTTP %ld", prefix, status);
}

static void set_error_result(Result rc)
{
    snprintf(s_error, sizeof(s_error), "download failed: 0x%08lX", (unsigned long)rc);
}

const char *net_error(void)
{
    return s_error[0] ? s_error : "unknown network error";
}

bool net_init(void)
{
    if (s_ready) return true;

    s_soc_buffer = memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (s_soc_buffer == NULL) {
        set_error("out of memory for SOC", NULL);
        return false;
    }
    if (R_FAILED(socInit((u32 *)s_soc_buffer, SOC_BUFFERSIZE))) {
        free(s_soc_buffer);
        s_soc_buffer = NULL;
        set_error("socInit() failed", NULL);
        return false;
    }
    // GET requests never upload a body, so httpc needs no shared memory.
    if (R_FAILED(httpcInit(0))) {
        socExit();
        free(s_soc_buffer);
        s_soc_buffer = NULL;
        set_error("httpcInit() failed", NULL);
        return false;
    }
    curl_global_init(CURL_GLOBAL_DEFAULT);
    s_ready = true;
    return true;
}

void net_exit(void)
{
    if (!s_ready) return;
    httpcExit();
    curl_global_cleanup();
    socExit();
    free(s_soc_buffer);
    s_soc_buffer = NULL;
    s_ready = false;
}

static CURL *curl_new(const char *url)
{
    CURL *h = curl_easy_init();
    if (h == NULL) {
        set_error("curl_easy_init() failed", NULL);
        return NULL;
    }
    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_USERAGENT, HTTP_USER_AGENT);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(h, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(h, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(h, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(h, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(h, CURLOPT_CONNECTTIMEOUT, 20L);
    curl_easy_setopt(h, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(h, CURLOPT_LOW_SPEED_TIME, 60L);
    return h;
}

static size_t sink_mem(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    struct MemSink *m = (struct MemSink *)userdata;
    size_t n = size * nmemb;

    if (m->len + n + 1 > m->cap) {
        size_t ncap = m->cap ? m->cap : 64 * 1024;
        char *grown;
        while (ncap < m->len + n + 1) ncap *= 2;
        grown = (char *)realloc(m->data, ncap);
        if (grown == NULL) return 0;
        m->data = grown;
        m->cap = ncap;
    }
    memcpy(m->data + m->len, ptr, n);
    m->len += n;
    m->data[m->len] = '\0';
    return n;
}

static void meter_reset(struct DlMeter *m)
{
    m->tick = 0;
    m->bytes = 0;
    m->started = false;
    // g_app.download_speed keeps the previous reading so the indicator does not
    // blank out between files; the first fresh sample replaces it. The EMA seed
    // itself must stay at zero, otherwise the old speed skews the new average.
    m->speed = 0.0f;
}

static void meter_update(struct DlMeter *m, size_t bytes, u32 total)
{
    u64   now = svcGetSystemTick();
    float dt;
    float inst;

    // Sample at a fixed pace and smooth the result: a per-callback value makes
    // the speed/ETA text jitter and visibly blink.
    if (!m->started) {
        // First sample only stores the baseline: there is nothing to measure yet.
        m->started = true;
        m->tick = now;
        m->bytes = bytes;
        return;
    }

    dt = (float)(now - m->tick) / (float)SYSCLOCK_ARM11;
    if (dt < 0.5f) return;

    inst = (float)(bytes - m->bytes) / dt;
    if (inst < 0.0f) inst = 0.0f;
    m->speed = (m->speed > 0.0f) ? (m->speed * 0.7f + inst * 0.3f) : inst;
    g_app.download_speed = m->speed;
    g_app.download_eta = (total > 0 && m->speed > 1.0f)
                             ? (float)((double)total - (double)bytes) / m->speed
                             : 0.0f;
    m->tick = now;
    m->bytes = bytes;
}

void net_buf_free(NetBuf *b)
{
    if (b == NULL) return;
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->status = 0;
}

bool net_get(const char *url, NetBuf *out)
{
    CURL *h;
    struct MemSink sink = { NULL, 0, 0 };
    CURLcode rc;
    long status = 0;

    out->data = NULL;
    out->len = 0;
    out->status = 0;

    if (!s_ready && !net_init()) return false;

    h = curl_new(url);
    if (h == NULL) return false;

    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, sink_mem);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &sink);
    curl_easy_setopt(h, CURLOPT_HTTPGET, 1L);

    rc = curl_easy_perform(h);
    if (rc != CURLE_OK) {
        set_error("request failed: %s", curl_easy_strerror(rc));
        curl_easy_cleanup(h);
        free(sink.data);
        return false;
    }
    curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(h);

    if (sink.data == NULL) {
        sink.data = (char *)malloc(1);
        if (sink.data == NULL) return false;
        sink.data[0] = '\0';
    }

    out->data = sink.data;
    out->len = sink.len;
    out->status = status;

    if (status < 200 || status >= 300) {
        set_error_status("server responded", status);
        return false;
    }
    return true;
}

// httpc does not follow redirects, so resolve the Location header ourselves.
// Absolute targets are used as-is; a path is appended to the current origin.
static bool resolve_location(const char *base, const char *location,
                             char *out, size_t cap)
{
    const char *sep;
    const char *slash;
    size_t      prefix;
    size_t      len;

    if (strncmp(location, "http://", 7) == 0 ||
        strncmp(location, "https://", 8) == 0) {
        if (strlen(location) >= cap) return false;
        snprintf(out, cap, "%s", location);
        return true;
    }

    if (location[0] != '/') return false;

    sep = strstr(base, "://");
    if (sep == NULL) return false;
    slash = strchr(sep + 3, '/');
    prefix = (slash != NULL) ? (size_t)(slash - base) : strlen(base);
    len = strlen(location);
    if (prefix + len >= cap) return false;

    memcpy(out, base, prefix);
    memcpy(out + prefix, location, len + 1);
    return true;
}

bool net_download(const char *url, const char *dest, ProgressFn cb, void *user)
{
    httpcContext    ctx;
    struct DlMeter  meter;
    char            current[1400];
    char            location[1024];
    char            next[1400];
    u8             *buf = NULL;
    FILE           *fp = NULL;
    bool            opened = false;
    bool            ok = false;
    u32             status = 0;
    u32             total = 0;
    size_t          done = 0;
    Result          rc = 0;
    int             hop;

    if (!s_ready && !net_init()) return false;

    if (!fs_mkdir_parents(dest)) {
        set_error("failed to create destination directory", NULL);
        return false;
    }

    if (strlen(url) >= sizeof(current)) {
        set_error("download URL is too long", NULL);
        return false;
    }
    snprintf(current, sizeof(current), "%s", url);

    buf = (u8 *)memalign(SOC_ALIGN, DL_RECV_SIZE);
    if (buf == NULL) {
        set_error("out of memory for download buffer", NULL);
        return false;
    }

    fp = fopen(dest, "wb");
    if (fp == NULL) {
        set_error("failed to open %s for writing", dest);
        free(buf);
        return false;
    }
    setvbuf(fp, NULL, _IOFBF, DL_FILE_BUFSIZE);
    meter_reset(&meter);

    for (hop = 0; hop <= DL_MAX_REDIRECTS; hop++) {
        rc = httpcOpenContext(&ctx, HTTPC_METHOD_GET, current, 0);
        if (R_FAILED(rc)) {
            set_error_result(rc);
            goto out;
        }
        opened = true;

        // Same policy as the previous libcurl path: the console has no usable
        // CA bundle, so certificate verification stays disabled.
        httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify);
        httpcSetKeepAlive(&ctx, HTTPC_KEEPALIVE_ENABLED);
        httpcAddRequestHeaderField(&ctx, "User-Agent", HTTP_USER_AGENT);

        rc = httpcBeginRequest(&ctx);
        if (R_FAILED(rc)) {
            set_error_result(rc);
            goto out;
        }

        rc = httpcGetResponseStatusCodeTimeout(&ctx, &status, DL_TIMEOUT_NS);
        if (R_FAILED(rc)) {
            set_error_result(rc);
            goto out;
        }

        if (status / 100 != 3) break;

        rc = httpcGetResponseHeader(&ctx, "location", location, sizeof(location));
        if (R_FAILED(rc)) {
            set_error_status("server responded", (long)status);
            goto out;
        }
        if (!resolve_location(current, location, next, sizeof(next))) {
            set_error_status("server responded", (long)status);
            goto out;
        }
        snprintf(current, sizeof(current), "%s", next);

        httpcCancelConnection(&ctx);
        httpcCloseContext(&ctx);
        opened = false;
    }

    if (status / 100 != 2) {
        set_error_status("server responded", (long)status);
        goto out;
    }

    httpcGetDownloadSizeState(&ctx, NULL, &total);

    for (;;) {
        u32 got = 0;

        rc = httpcDownloadData(&ctx, buf, DL_RECV_SIZE, &got);
        if (R_FAILED(rc) && rc != (Result)HTTPC_RESULTCODE_DOWNLOADPENDING) {
            set_error_result(rc);
            goto out;
        }

        if (got > 0) {
            if (fwrite(buf, 1, got, fp) != got) {
                set_error("incomplete write to SD card", NULL);
                goto out;
            }
            done += got;
            meter_update(&meter, done, total);
            if (cb != NULL) {
                float frac = (total > 0)
                                 ? (float)((double)done / (double)total)
                                 : -1.0f;
                cb(NULL, frac, user);
            }
        }

        if (g_app.cancel) {
            set_error("download cancelled", NULL);
            goto out;
        }

        // DOWNLOADPENDING means data was written and more is on the way.
        if (rc != (Result)HTTPC_RESULTCODE_DOWNLOADPENDING) break;
        if (got == 0) {
            set_error("download stalled", NULL);
            goto out;
        }
    }

    if (fflush(fp) != 0 || ferror(fp)) {
        set_error("incomplete write to SD card", NULL);
        goto out;
    }

    ok = true;

out:
    if (opened) {
        httpcCancelConnection(&ctx);
        httpcCloseContext(&ctx);
    }
    if (fp != NULL) fclose(fp);
    free(buf);
    if (!ok) remove(dest);
    return ok;
}
