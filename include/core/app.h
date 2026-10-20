#pragma once

#include <stdbool.h>
#include <stddef.h>

#define APP_DIR         "sdmc:/3ds/NDS-Shop"
#define APP_CONFIG_PATH APP_DIR "/config.txt"
#define APP_ATLAS_PATH  APP_DIR "/atlas.t3x"
#define APP_ATLAS_URL   APP_DIR "/atlas.url"
#define APP_CACHE_PATH  APP_DIR "/catalog.json"
#define APP_ROM_DIR     "sdmc:/roms/nds"

#define APP_TITLE        "NDS-Shop"
#define APP_API_DEFAULT  "https://api.db-nds-shop.fr"
#define APP_LANG_DEFAULT "en"

#define CFG_API   "api"
#define CFG_LANG  "lang"

#define CATALOG_PAGE_LIMIT 200

typedef struct {
    char type[32];
    char file[1024];
    char output[512];
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
    int  icon_index;
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
} Config;

typedef enum {
    SCR_LOADING = 0,
    SCR_LIST,
    SCR_SETTINGS,
    SCR_TASK,
    SCR_HELP,
    SCR_ERROR,
} ScreenId;

typedef void (*ProgressFn)(const char *label, float frac, void *user);

typedef struct {
    Config   config;
    Catalog  catalog;

    ScreenId screen;
    ScreenId previous;

    int  selected;
    int  scroll;
    int  detail_scroll;
    int  cursor;

    char  search[128];
    int  *filter;
    int   filter_count;

    char status[256];
    int  status_ttl;
    char error[512];
    float progress;
    bool  offline;
    bool  running;
    volatile bool cancel;

    int battery_percent;
    bool battery_charging;
    int wifi_bars;      // -1 = disconnected, 0..3 = signal strength
    bool ask_forwarder; // true = install the HOME menu shortcut
    float download_speed;
    float download_eta;
} AppState;

extern AppState g_app;

const char *tr(const char *key);