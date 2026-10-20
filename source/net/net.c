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

static void  *s_soc_buffer = NULL;
static bool   s_ready = false;
static char   s_error[256];

struct MemSink {
    char  *data;
    size_t len;
    size_t cap;
};

struct FileSink {
    FILE      *fp;
    ProgressFn cb;
    void      *user;
    bool       write_failed;
    u64        sample_tick;
    size_t     sample_bytes;
    bool       sampled;
    float      speed;
    float      eta;
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
    curl_global_init(CURL_GLOBAL_DEFAULT);
    s_ready = true;
    return true;
}

void net_exit(void)
{
    if (!s_ready) return;
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

static size_t sink_file(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    struct FileSink *f = (struct FileSink *)userdata;
    size_t items = fwrite(ptr, size, nmemb, f->fp);
    if (items != nmemb) f->write_failed = true;
    return items * size;
}

static int sink_progress(void *userdata, curl_off_t dltotal, curl_off_t dlnow,
                         curl_off_t ultotal, curl_off_t ulnow)
{
    struct FileSink *f = (struct FileSink *)userdata;
    (void)ultotal;
    (void)ulnow;

    u64 now = svcGetSystemTick();

    // Sample at a fixed pace and smooth the result: a per-callback value makes
    // the speed/ETA text jitter and visibly blink.
    if (!f->sampled) {
        // First callback only stores the baseline: there is nothing to measure yet.
        f->sampled = true;
        f->sample_tick = now;
        f->sample_bytes = (size_t)dlnow;
    } else {
        float dt = (float)(now - f->sample_tick) / (float)SYSCLOCK_ARM11;
        if (dt >= 0.5f) {
            float inst = (float)((size_t)dlnow - f->sample_bytes) / dt;
            if (inst < 0.0f) inst = 0.0f;
            f->speed = (f->speed > 0.0f) ? (f->speed * 0.7f + inst * 0.3f) : inst;
            f->eta = (dltotal > 0 && f->speed > 1.0f)
                         ? (float)(dltotal - dlnow) / f->speed
                         : 0.0f;
            f->sample_tick = now;
            f->sample_bytes = (size_t)dlnow;
            g_app.download_speed = f->speed;
            g_app.download_eta = f->eta;
        }
    }

    if (f->cb != NULL) {
        float frac = -1.0f;
        if (dltotal > 0) frac = (float)((double)dlnow / (double)dltotal);
        f->cb(NULL, frac, f->user);
    }
    return g_app.cancel ? 1 : 0;
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

bool net_download(const char *url, const char *dest, ProgressFn cb, void *user)
{
    CURL *h;
    struct FileSink sink = { 0 };
    CURLcode rc;
    long status = 0;
    bool ok = false;

    if (!s_ready && !net_init()) return false;

    if (!fs_mkdir_parents(dest)) {
        set_error("failed to create destination directory", NULL);
        return false;
    }

    sink.fp = fopen(dest, "wb");
    if (sink.fp == NULL) {
        set_error("failed to open %s for writing", dest);
        return false;
    }
    sink.cb = cb;
    sink.user = user;
    sink.write_failed = false;
    sink.sample_tick = 0;
    sink.sample_bytes = 0;
    sink.sampled = false;
    // g_app.download_speed keeps the previous reading so the indicator does not
    // blank out between files; the first fresh sample replaces it. The EMA seed
    // itself must stay at zero, otherwise the old speed skews the new average.
    sink.speed = 0.0f;
    sink.eta = 0.0f;

    h = curl_new(url);
    if (h == NULL) {
        fclose(sink.fp);
        return false;
    }

    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, sink_file);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &sink);
    curl_easy_setopt(h, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(h, CURLOPT_XFERINFOFUNCTION, sink_progress);
    curl_easy_setopt(h, CURLOPT_XFERINFODATA, &sink);

    rc = curl_easy_perform(h);
    if (rc != CURLE_OK) {
        set_error("download failed: %s", curl_easy_strerror(rc));
    } else {
        curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &status);
        if (status < 200 || status >= 300) {
            set_error_status("server responded", status);
        } else if (sink.write_failed) {
            set_error("incomplete write to SD card", NULL);
        } else {
            ok = true;
        }
    }
    curl_easy_cleanup(h);
    fclose(sink.fp);

    if (!ok) remove(dest);
    return ok;
}
