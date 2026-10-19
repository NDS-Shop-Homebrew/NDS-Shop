#include "catalog/atlas.h"
#include "core/fsutil.h"
#include "net/net.h"

#include <stdlib.h>
#include <string.h>

#include "core/app.h"

static C2D_SpriteSheet s_sheet = NULL;
static char            s_url[1024];

static void remember_url(const char *url)
{
    snprintf(s_url, sizeof(s_url), "%s", url != NULL ? url : "");
}

int atlas_count(void)
{
    if (s_sheet == NULL) return 0;
    return (int)C2D_SpriteSheetCount(s_sheet);
}

bool atlas_ready(void)
{
    return s_sheet != NULL;
}

bool atlas_get(int index, C2D_Image *out)
{
    if (s_sheet == NULL || out == NULL) return false;
    if (index < 0 || index >= atlas_count()) return false;
    *out = C2D_SpriteSheetGetImage(s_sheet, (size_t)index);
    return true;
}

void atlas_free(void)
{
    if (s_sheet != NULL) {
        C2D_SpriteSheetFree(s_sheet);
        s_sheet = NULL;
    }
    s_url[0] = '\0';
}

static void trim(char *s)
{
    size_t n;
    size_t start = 0;
    if (s == NULL) return;
    while (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n') start++;
    if (start > 0) memmove(s, s + start, strlen(s + start) + 1);
    n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n')) {
        s[--n] = '\0';
    }
}

int atlas_load_cached(const char *url)
{
    char *saved;

    atlas_free();
    if (url == NULL || url[0] == '\0') return 0;
    if (!fs_exists(APP_ATLAS_PATH)) return 0;

    saved = fs_read_file(APP_ATLAS_URL, NULL);
    if (saved == NULL) return 0;
    trim(saved);
    if (strcmp(saved, url) != 0) {
        free(saved);
        return 0;
    }
    free(saved);

    s_sheet = C2D_SpriteSheetLoad(APP_ATLAS_PATH);
    if (s_sheet == NULL) return 0;
    remember_url(url);
    return atlas_count();
}

bool atlas_fetch(const char *url, ProgressFn cb, void *user)
{
    atlas_free();
    if (url == NULL || url[0] == '\0') return false;

    if (!net_download(url, APP_ATLAS_PATH, cb, user)) return false;

    s_sheet = C2D_SpriteSheetLoad(APP_ATLAS_PATH);
    if (s_sheet == NULL) return false;

    remember_url(url);
    fs_write_file(APP_ATLAS_URL, url, strlen(url));
    return true;
}
