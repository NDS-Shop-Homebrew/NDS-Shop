// Host test for the catalog parser (no network, no SD card).
//
//   make -C tests/host
//
// Checks catalog_parse_page() on a response faithful to /api/v1/catalog.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "catalog/catalog.h"
#include "catalog/json.h"

// --- Doublures --------------------------------------------------------------
// The network/SD layer is not tested here: catalog.c must not call it.
AppState g_app;

const char *tr(const char *key) { return key; }

bool net_get(const char *url, NetBuf *out)
{
    (void)url;
    out->data = NULL;
    out->len = 0;
    out->status = 0;
    return false;
}

void net_buf_free(NetBuf *b) { (void)b; }

bool fs_write_file(const char *path, const void *data, size_t len)
{
    (void)path;
    (void)data;
    (void)len;
    return false;
}

char *fs_read_file(const char *path, size_t *out_len)
{
    (void)path;
    (void)out_len;
    return NULL;
}

// --- Échantillon ------------------------------------------------------------

static const char *PAGE =
    "{"
    "\"entries\":["
    "{"
    "\"title\":\"Game One\","
    "\"fileName\":\"game-one\","
    "\"description\":\"Un jeu de test.\","
    "\"author\":\"Studio A\","
    "\"developer\":\"Developpeur A\","
    "\"publisher\":\"Editeur A\","
    "\"version\":\"1.2\","
    "\"titleId\":\"0004000000000000\","
    "\"systems\":[\"DS\"],"
    "\"genres\":[\"puzzle\"],"
    "\"icon_index\":0,"
    "\"icon\":\"https://api.example/api/v1/assets/boxart/game-one.png\","
    "\"downloads\":{"
    "\"Game One.nds\":{\"url\":\"https://api.example/api/v1/download/Game%20One.nds\",\"size\":1048576,\"size_str\":\"1 Mo\"}"
    "},"
    "\"scripts\":["
    "{\"type\":\"downloadFile\",\"file\":\"https://api.example/api/v1/download/Game%20One.nds\",\"output\":\"/roms/nds/Game One.nds\"},"
    "{\"type\":\"downloadFile\",\"file\":\"https://api.example/api/v1/download/0001.cia\",\"output\":\"/0001.cia\"},"
    "{\"type\":\"installCia\",\"file\":\"/0001.cia\"},"
    "{\"type\":\"deleteFile\",\"file\":\"/0001.cia\"}"
    "]"
    "},"
    "{"
    "\"title\":\"Jeu Deux\","
    "\"fileName\":\"jeu-deux\","
    "\"description\":\"Ligne 1\\nLigne 2 \\u00e9 \\\"cite\\\"\","
    "\"author\":\"Studio B\","
    "\"developer\":\"Developpeur B\","
    "\"version\":\"\","
    "\"icon_index\":1,"
    "\"downloads\":{},"
    "\"scripts\":[]"
    "}"
    "],"
    "\"page\":1,\"limit\":200,\"total\":2,\"revision\":1700000000,"
    "\"generatedAt\":\"2026-01-01T00:00:00.000Z\","
    "\"store\":{"
    "\"title\":\"NDS-Shop\","
    "\"author\":\"NDS-Shop-Homebrew\","
    "\"description\":\"Boutique\","
    "\"sheet\":\"https://api.example/api/v1/assets/unistore/db-nds-shop.t3x\","
    "\"sheet_dsi\":\"https://api.example/api/v1/assets/unistore/db-nds-shop.tdx\","
    "\"icon\":\"https://api.example/api/v1/assets/unistore/icon.png\","
    "\"cover\":\"https://api.example/api/v1/assets/unistore/cover.png\""
    "}"
    "}";

static int failures = 0;

static void check(int ok, const char *what)
{
    if (!ok) {
        printf("  FAIL: %s\n", what);
        failures++;
    }
}

static void check_str(const char *got, const char *want, const char *what)
{
    if (got == NULL || strcmp(got, want) != 0) {
        printf("  FAIL: %s (expected \"%s\", got \"%s\")\n", what, want, got ? got : "(null)");
        failures++;
    }
}

int main(void)
{
    Catalog cat;

    memset(&cat, 0, sizeof(cat));

    printf("catalog_parse_page()\n");
    check(catalog_parse_page(PAGE, &cat), "analyse de la page");

    check(cat.count == 2, "2 jeux");
    check(cat.total == 2, "total = 2");
    check(cat.revision == 1700000000LL, "revision in seconds");
    check_str(cat.store.title, "NDS-Shop", "titre de la boutique");
    check_str(cat.store.author, "NDS-Shop-Homebrew", "auteur de la boutique");
    if (cat.count >= 1) {
        check_str(cat.store.sheet, "https://api.example/api/v1/assets/unistore/db-nds-shop.t3x", "atlas");
    }

    if (cat.count == 2) {
        const Game *g = &cat.games[0];
        check_str(g->title, "Game One", "titre");
        check_str(g->fileName, "game-one", "fileName");
        check_str(g->author, "Studio A", "auteur");
        check_str(g->developer, "Developpeur A", "developer");
        check_str(g->version, "1.2", "version");
        check(g->icon_index == 0, "icon_index = 0");

        check(g->download_count == 1, "1 download");
        if (g->download_count == 1) {
            check_str(g->downloads[0].filename, "Game One.nds", "nom du fichier");
            check_str(g->downloads[0].url,
                      "https://api.example/api/v1/download/Game%20One.nds", "download url");
            check_str(g->downloads[0].size_str, "1 Mo", "taille lisible");
        }

        check(g->script_count == 4, "4 script steps");
        if (g->script_count == 4) {
            check_str(g->scripts[0].type, "downloadFile", "step 0: type");
            check_str(g->scripts[0].output, "/roms/nds/Game One.nds", "step 0: destination");
            check_str(g->scripts[1].file, "https://api.example/api/v1/download/0001.cia", "step 1: source");
            check_str(g->scripts[2].type, "installCia", "step 2: type");
            check_str(g->scripts[2].file, "/0001.cia", "step 2: path");
            check_str(g->scripts[3].type, "deleteFile", "step 3: type");
        }

        g = &cat.games[1];
        check_str(g->title, "Jeu Deux", "accented title");
        check(g->icon_index == 1, "icon_index = 1");
        check(g->download_count == 0, "no downloads");
        check(g->script_count == 0, "aucun script");
        check_str(g->description, "Ligne 1\nLigne 2 \xc3\xa9 \"cite\"", "JSON escapes");
        check_str(g->version, "", "version absente");
        check_str(g->systems, "", "JSON array skipped cleanly");
    }

    // Pagination: a second page appends to the same catalog.
    printf("pagination\n");
    check(catalog_parse_page(PAGE, &cat), "analyse de la seconde page");
    check(cat.count == 4, "4 games after pagination");
    check(cat.total == 2, "total unchanged");
    if (cat.count == 4) {
        check_str(cat.games[3].title, "Jeu Deux", "titre du dernier jeu");
    }

    catalog_free(&cat);
    check(cat.count == 0 && cat.games == NULL, "catalog freed");

    // Invalid / truncated JSON.
    printf("robustesse\n");
    check(!catalog_parse_page("{\"entries\":[}],\"total\":1}", &cat), "invalid JSON rejected");
    check(!catalog_parse_page("{\"total\":1}", &cat), "missing entries rejected");
    check(!catalog_parse_page("", &cat), "empty body rejected");
    catalog_free(&cat);

    if (failures == 0) {
        printf("\nOK: all tests pass.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
