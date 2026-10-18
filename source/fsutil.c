// Utilitaires carte SD et configuration.
#include "fsutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Longueur de "sdmc:" : racine non créable.
#define ROOT_LEN 5

bool fs_mkdir_tree(const char *dir_path)
{
    char  buf[1024];
    size_t n;
    char *p;

    if (dir_path == NULL) return false;
    n = strlen(dir_path);
    if (n == 0 || n >= sizeof(buf)) return false;
    memcpy(buf, dir_path, n + 1);

    for (p = buf + 1; *p != '\0'; p++) {
        if (*p != '/') continue;
        *p = '\0';
        if (strlen(buf) > ROOT_LEN) mkdir(buf, 0777);
        *p = '/';
    }
    if (strlen(buf) > ROOT_LEN) mkdir(buf, 0777);
    return true;
}

bool fs_mkdir_parents(const char *file_path)
{
    char  buf[1024];
    size_t n;
    char *slash;

    if (file_path == NULL) return false;
    n = strlen(file_path);
    if (n >= sizeof(buf)) return false;
    memcpy(buf, file_path, n + 1);

    slash = strrchr(buf, '/');
    if (slash == NULL) return true;
    *slash = '\0';
    if (strlen(buf) <= ROOT_LEN) return true;
    return fs_mkdir_tree(buf);
}

bool fs_exists(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) return false;
    fclose(fp);
    return true;
}

bool fs_write_file(const char *path, const void *data, size_t len)
{
    FILE *fp;

    if (!fs_mkdir_parents(path)) return false;
    fp = fopen(path, "wb");
    if (fp == NULL) return false;
    if (len > 0 && fwrite(data, 1, len, fp) != len) {
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
}

char *fs_read_file(const char *path, size_t *out_len)
{
    FILE  *fp;
    char  *buf;
    long   size;

    fp = fopen(path, "rb");
    if (fp == NULL) return NULL;

    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    size = ftell(fp);
    if (size < 0) { fclose(fp); return NULL; }
    rewind(fp);

    buf = (char *)malloc((size_t)size + 1);
    if (buf == NULL) { fclose(fp); return NULL; }

    if (size > 0 && fread(buf, 1, (size_t)size, fp) != (size_t)size) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    buf[size] = '\0';
    if (out_len != NULL) *out_len = (size_t)size;
    return buf;
}

// ---------------------------------------------------------------------------
// Configuration (clé=valeur)
// ---------------------------------------------------------------------------

static void trim_eol(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t')) {
        s[--n] = '\0';
    }
}

static void copy_str(char *dst, size_t cap, const char *src)
{
    if (cap == 0) return;
    snprintf(dst, cap, "%s", src != NULL ? src : "");
}

void config_load(Config *cfg)
{
    char *text;
    char *line;
    char *save = NULL;

    copy_str(cfg->api_base, sizeof(cfg->api_base), APP_API_DEFAULT);
    copy_str(cfg->lang, sizeof(cfg->lang), APP_LANG_DEFAULT);
    cfg->dark = false;

    text = fs_read_file(APP_CONFIG_PATH, NULL);
    if (text == NULL) return;

    for (line = strtok_r(text, "\n", &save); line != NULL; line = strtok_r(NULL, "\n", &save)) {
        char *eq;
        trim_eol(line);
        if (line[0] == '#' || line[0] == '\0') continue;
        eq = strchr(line, '=');
        if (eq == NULL) continue;
        *eq = '\0';
        if (strcmp(line, CFG_API) == 0) {
            copy_str(cfg->api_base, sizeof(cfg->api_base), eq + 1);
        } else if (strcmp(line, CFG_LANG) == 0) {
            copy_str(cfg->lang, sizeof(cfg->lang), eq + 1);
        } else if (strcmp(line, CFG_THEME) == 0) {
            cfg->dark = (strcmp(eq + 1, "dark") == 0);
        }
    }
    free(text);

    // Retire un éventuel '/' final et un "/api" redondant.
    {
        size_t n = strlen(cfg->api_base);
        while (n > 0 && cfg->api_base[n - 1] == '/') cfg->api_base[--n] = '\0';
        if (n == 0) copy_str(cfg->api_base, sizeof(cfg->api_base), APP_API_DEFAULT);
    }
}

bool config_save(const Config *cfg)
{
    char buf[1024];
    int  n;

    n = snprintf(buf, sizeof(buf),
                 "# Configuration NDS-Shop\n"
                 "%s=%s\n"
                 "%s=%s\n"
                 "%s=%s\n",
                 CFG_API, cfg->api_base,
                 CFG_LANG, cfg->lang,
                 CFG_THEME, cfg->dark ? "dark" : "light");
    if (n <= 0) return false;
    return fs_write_file(APP_CONFIG_PATH, buf, (size_t)n);
}
