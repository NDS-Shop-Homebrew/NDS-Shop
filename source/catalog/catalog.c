#include "catalog/catalog.h"
#include "core/fsutil.h"
#include "core/i18n.h"
#include "catalog/json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"
#include "net/net.h"

#define TOKENS_START 32768u
#define TOKENS_MAX   (1u << 22)

static void get_s(const char *json, const JsonTok *toks, int obj, const char *key,
                  char *out, size_t cap)
{
    json_get_str(json, toks, json_key(json, toks, obj, key), out, cap);
}

static Game *push_game(Catalog *cat)
{
    Game *arr = (Game *)realloc(cat->games, sizeof(Game) * (size_t)(cat->count + 1));
    Game *g;
    if (arr == NULL) return NULL;
    cat->games = arr;
    g = &cat->games[cat->count];
    memset(g, 0, sizeof(*g));
    g->icon_index = -1;
    cat->count++;
    return g;
}

static Download *push_download(Game *g)
{
    Download *arr = (Download *)realloc(g->downloads, sizeof(Download) * (size_t)(g->download_count + 1));
    Download *d;
    if (arr == NULL) return NULL;
    g->downloads = arr;
    d = &g->downloads[g->download_count];
    memset(d, 0, sizeof(*d));
    g->download_count++;
    return d;
}

static ScriptStep *push_script(Game *g)
{
    ScriptStep *arr = (ScriptStep *)realloc(g->scripts, sizeof(ScriptStep) * (size_t)(g->script_count + 1));
    ScriptStep *s;
    if (arr == NULL) return NULL;
    g->scripts = arr;
    s = &g->scripts[g->script_count];
    memset(s, 0, sizeof(*s));
    g->script_count++;
    return s;
}

static void parse_downloads(const char *json, const JsonTok *toks, int entry, Game *g)
{
    int dl = json_key(json, toks, entry, "downloads");
    int pair;
    int idx;

    if (dl < 0 || toks[dl].type != JSON_OBJECT) return;

    idx = dl + 1;
    for (pair = 0; pair < toks[dl].size; pair++) {
        int value = idx + 1;
        Download *d;
        if (value < 0) break;
        d = push_download(g);
        if (d == NULL) break;
        json_get_str(json, toks, idx, d->filename, sizeof(d->filename));
        get_s(json, toks, value, "url", d->url, sizeof(d->url));
        get_s(json, toks, value, "size_str", d->size_str, sizeof(d->size_str));
        idx = json_next(toks, value);
        if (idx < 0) break;
    }
}

static void parse_scripts(const char *json, const JsonTok *toks, int entry, Game *g)
{
    int sc = json_key(json, toks, entry, "scripts");
    int n;
    int i;

    if (sc < 0 || toks[sc].type != JSON_ARRAY) return;

    n = json_len(toks, sc);
    for (i = 0; i < n; i++) {
        int s = json_at(toks, sc, i);
        ScriptStep *step;
        if (s < 0) continue;
        step = push_script(g);
        if (step == NULL) break;
        get_s(json, toks, s, "type", step->type, sizeof(step->type));
        get_s(json, toks, s, "file", step->file, sizeof(step->file));
        get_s(json, toks, s, "output", step->output, sizeof(step->output));
    }
}

bool catalog_parse_page(const char *json, Catalog *cat)
{
    JsonParser parser;
    JsonTok   *toks = NULL;
    unsigned   cap = TOKENS_START;
    int        r = JSON_ERR_INVAL;
    int        root;
    int        entries;
    int        store;
    int        e;
    int        i;
    int        n;

    for (;;) {
        toks = (JsonTok *)malloc(sizeof(JsonTok) * cap);
        if (toks == NULL) return false;
        json_init(&parser);
        r = json_run(&parser, json, strlen(json), toks, cap);
        if (r != JSON_ERR_NOMEM) break;
        free(toks);
        toks = NULL;
        if (cap >= TOKENS_MAX) return false;
        cap *= 2;
    }
    if (r < 0 || toks == NULL) {
        free(toks);
        return false;
    }

    root = 0;
    if (toks[root].type != JSON_OBJECT) {
        free(toks);
        return false;
    }

    entries = json_key(json, toks, root, "entries");
    if (entries < 0 || toks[entries].type != JSON_ARRAY) {
        free(toks);
        return false;
    }

    cat->total = (int)json_get_int(json, toks, json_key(json, toks, root, "total"), cat->total);
    cat->revision = json_get_int(json, toks, json_key(json, toks, root, "revision"), cat->revision);

    store = json_key(json, toks, root, "store");
    if (store >= 0) {
        get_s(json, toks, store, "title", cat->store.title, sizeof(cat->store.title));
        get_s(json, toks, store, "author", cat->store.author, sizeof(cat->store.author));
        get_s(json, toks, store, "sheet", cat->store.sheet, sizeof(cat->store.sheet));
    }

    n = json_len(toks, entries);
    e = json_at(toks, entries, 0);
    for (i = 0; i < n && e >= 0; i++) {
        Game *g = push_game(cat);
        if (g == NULL) {
            free(toks);
            return false;
        }
        get_s(json, toks, e, "title", g->title, sizeof(g->title));
        get_s(json, toks, e, "fileName", g->fileName, sizeof(g->fileName));
        get_s(json, toks, e, "description", g->description, sizeof(g->description));
        get_s(json, toks, e, "author", g->author, sizeof(g->author));
        get_s(json, toks, e, "developer", g->developer, sizeof(g->developer));
        get_s(json, toks, e, "version", g->version, sizeof(g->version));
        get_s(json, toks, e, "systems", g->systems, sizeof(g->systems));
        get_s(json, toks, e, "icon", g->icon, sizeof(g->icon));
        g->icon_index = (int)json_get_int(json, toks, json_key(json, toks, e, "icon_index"), -1);
        parse_downloads(json, toks, e, g);
        parse_scripts(json, toks, e, g);
        e = json_next(toks, e);
    }

    free(toks);
    return true;
}

void catalog_free(Catalog *cat)
{
    int i;
    if (cat == NULL) return;
    for (i = 0; i < cat->count; i++) {
        free(cat->games[i].downloads);
        free(cat->games[i].scripts);
    }
    free(cat->games);
    memset(cat, 0, sizeof(*cat));
}

bool catalog_load(Catalog *cat, ProgressFn cb, void *user)
{
    char  url[1400];
    int   page = 1;
    bool  fetched = false;

    catalog_free(cat);

    while (page <= 20) {
        NetBuf buf;
        snprintf(url, sizeof(url), "%s/api/v1/catalog?page=%d&limit=%d",
                 g_app.config.api_base, page, CATALOG_PAGE_LIMIT);
        if (cb != NULL) cb(tr("loading.catalog"), 0.1f, user);
        if (!net_get(url, &buf)) break;
        if (!catalog_parse_page(buf.data, cat)) {
            net_buf_free(&buf);
            break;
        }
        if (page == 1) fs_write_file(APP_CACHE_PATH, buf.data, buf.len);
        net_buf_free(&buf);
        fetched = true;
        if (cat->count == 0 || cat->count >= cat->total) break;
        page++;
    }

    if (fetched) {
        g_app.offline = false;
        return true;
    }

    {
        char *cached = fs_read_file(APP_CACHE_PATH, NULL);
        bool  ok;
        if (cached == NULL) return false;
        ok = catalog_parse_page(cached, cat);
        free(cached);
        if (ok && cat->count > 0) {
            g_app.offline = true;
            return true;
        }
    }
    catalog_free(cat);
    return false;
}
