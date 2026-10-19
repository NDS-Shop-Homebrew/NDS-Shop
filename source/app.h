// NDS-Shop — types et état global de l'application.
//
// Ce fichier reste indépendant de libctru/citro2d afin de pouvoir compiler la
// couche de données sur une machine hôte (voir tests/host).
#pragma once

#include <stdbool.h>
#include <stddef.h>

// ---------------------------------------------------------------------------
// Chemins sur la carte SD
// ---------------------------------------------------------------------------
#define APP_DIR         "sdmc:/3ds/NDS-Shop"
#define APP_CONFIG_PATH APP_DIR "/config.txt"
#define APP_ATLAS_PATH  APP_DIR "/atlas.t3x"
#define APP_ATLAS_URL   APP_DIR "/atlas.url"
#define APP_CACHE_PATH  APP_DIR "/catalog.json"
#define APP_ROM_DIR     "sdmc:/roms/nds"

// ---------------------------------------------------------------------------
// Valeurs par défaut
// ---------------------------------------------------------------------------
#define APP_TITLE        "NDS-Shop"
#define APP_API_DEFAULT  "https://api.db-nds-shop.fr"
#define APP_LANG_DEFAULT "fr"

#define CFG_API   "api"
#define CFG_LANG  "lang"
#define CFG_THEME "theme"

#define CATALOG_PAGE_LIMIT 200

// ---------------------------------------------------------------------------
// Modèle de données (miroir du contrat /api/v1/catalog)
// ---------------------------------------------------------------------------
typedef struct {
    char type[32];     // downloadFile | installCia | deleteFile
    char file[1024];   // URL (downloadFile) ou chemin SD (installCia/deleteFile)
    char output[512];  // destination SD (downloadFile)
} ScriptStep;

typedef struct {
    char filename[256];
    char url[1024];
    char size_str[32];
} Download;

typedef struct {
    char title[256];
    char fileName[160];
    char description[4096];
    char author[160];
    char developer[160];
    char version[32];
    char systems[128];
    int  icon_index;   // position dans l'atlas (-1 = pas d'icône)
    char icon[1024];
    Download *downloads;
    int       download_count;
    ScriptStep *scripts;
    int         script_count;
} Game;

typedef struct {
    char title[128];
    char author[128];
    char sheet[1024];
} Store;

typedef struct {
    Game *games;
    int   count;
    int   total;
    long long revision;
    Store store;
} Catalog;

typedef struct {
    char api_base[512];
    char lang[8];
    bool dark;
} Config;

typedef enum {
    SCR_LOADING = 0,
    SCR_LIST,
    SCR_SETTINGS,
    SCR_TASK,
    SCR_HELP,
    SCR_ERROR,
} ScreenId;

// Progression (label affiché, 0..1 ou <0 si indéterminée).
typedef void (*ProgressFn)(const char *label, float frac, void *user);

typedef struct {
    Config   config;
    Catalog  catalog;

    ScreenId screen;
    ScreenId previous;

    int  selected;       // index dans la liste filtrée
    int  scroll;         // premier élément visible
    int  detail_scroll;
    int  cursor;         // index de ligne (écran réglages)

    char  search[128];   // filtre courant ("" = tout)
    int  *filter;        // indices des jeux visibles
    int   filter_count;

    char status[256];    // ligne d'état (barre du bas)
    int  status_ttl;     // durée d'affichage du statut (images)
    char error[512];
    float progress;
    bool  offline;       // catalogue servi depuis le cache local
    bool  running;
    volatile bool cancel; // demande d'annulation (tâche en cours)
} AppState;

extern AppState g_app;

// i18n
const char *tr(const char *key);
