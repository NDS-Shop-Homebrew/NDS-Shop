#include <3ds.h>
#include <citro2d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"
#include "catalog/atlas.h"
#include "catalog/catalog.h"
#include "core/fsutil.h"
#include "core/i18n.h"
#include "core/status.h"
#include "net/net.h"
#include "tasks/scripts.h"
#include "ui/ui.h"

#define DESC_STEP 300
#define STATUS_FRAMES 180

AppState g_app;

static bool contains_ci(const char *hay, const char *needle)
{
    size_t n;
    const char *p;

    if (needle == NULL || needle[0] == '\0') return true;
    if (hay == NULL) return false;
    n = strlen(needle);

    for (p = hay; *p != '\0'; p++) {
        size_t i;
        for (i = 0; i < n; i++) {
            char a = p[i];
            char b = needle[i];
            if (a == '\0') return false;
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
            if (a != b) break;
        }
        if (i == n) return true;
    }
    return false;
}

static void filter_build(void)
{
    int i;

    free(g_app.filter);
    g_app.filter = NULL;
    g_app.filter_count = 0;
    if (g_app.catalog.count <= 0) {
        g_app.selected = 0;
        g_app.scroll = 0;
        return;
    }

    g_app.filter = (int *)malloc(sizeof(int) * (size_t)g_app.catalog.count);
    if (g_app.filter == NULL) return;

    for (i = 0; i < g_app.catalog.count; i++) {
        const Game *g = &g_app.catalog.games[i];
        if (contains_ci(g->title, g_app.search) ||
            contains_ci(g->author, g_app.search) ||
            contains_ci(g->developer, g_app.search)) {
            g_app.filter[g_app.filter_count++] = i;
        }
    }

    if (g_app.selected >= g_app.filter_count) {
        g_app.selected = (g_app.filter_count > 0) ? g_app.filter_count - 1 : 0;
    }
    if (g_app.selected < 0) g_app.selected = 0;
    g_app.scroll = 0;
}

static const Game *selected_game(void)
{
    int index;
    if (g_app.selected < 0 || g_app.selected >= g_app.filter_count) return NULL;
    index = g_app.filter[g_app.selected];
    if (index < 0 || index >= g_app.catalog.count) return NULL;
    return &g_app.catalog.games[index];
}

static void set_status(const char *text)
{
    snprintf(g_app.status, sizeof(g_app.status), "%s", text != NULL ? text : "");
    g_app.status_ttl = STATUS_FRAMES;
}

// Redrawing a whole frame costs far more than receiving a download chunk: at
// 60 fps the UI would eat a large share of the bandwidth. 10 fps stays smooth
// enough for the progress bar while leaving the transfer room to breathe.
static bool ui_draw_due(void)
{
    static u64 last = 0;
    u64        now = svcGetSystemTick();

    if (last != 0 && (u64)(now - last) < (u64)(SYSCLOCK_ARM11 / 10)) return false;
    last = now;
    return true;
}

static void loading_progress(const char *label, float frac, void *user)
{
    (void)user;
    if (!ui_draw_due()) return;
    ui_frame_begin();
    ui_draw_loading(label != NULL ? label : tr("loading.catalog"), frac);
    ui_frame_end();
}

static void task_progress(const char *label, float frac, void *user)
{
    u32 down;

    (void)user;
    if (label != NULL) snprintf(g_app.status, sizeof(g_app.status), "%s", label);
    if (frac >= 0.0f) g_app.progress = frac;

    // Cancel has to be polled on every chunk, even when the frame is skipped.
    hidScanInput();
    down = hidKeysDown();
    if (down & KEY_B) g_app.cancel = true;

    if (!ui_draw_due()) return;

    ui_frame_begin();
    ui_draw_task(g_app.status, g_app.progress);
    ui_frame_end();
}

static void load_atlas(void)
{
    const char *sheet = g_app.catalog.store.sheet;
    if (sheet[0] == '\0') return;
    if (atlas_load_cached(sheet) > 0) return;
    atlas_fetch(sheet, loading_progress, NULL);
}

static void load_catalog(void)
{
    g_app.screen = SCR_LOADING;
    ui_frame_begin();
    ui_draw_loading(tr("loading.catalog"), 0.0f);
    ui_frame_end();

    if (!catalog_load(&g_app.catalog, loading_progress, NULL)) {
        snprintf(g_app.error, sizeof(g_app.error), "%s", net_error());
        g_app.previous = SCR_LIST;
        g_app.screen = SCR_ERROR;
        return;
    }

    filter_build();
    g_app.detail_scroll = 0;
    g_app.screen = SCR_LIST;

    ui_frame_begin();
    ui_draw_loading(tr("loading.icons"), 0.0f);
    ui_frame_end();
    load_atlas();
}

static bool ask_forwarder_cb(void *user)
{
    char *question = (char *)user;
    int   choice = 0;

    for (;;) {
        u32 down;

        hidScanInput();
        down = hidKeysDown();

        if (down & KEY_LEFT)  choice = 0;
        if (down & KEY_RIGHT) choice = 1;
        if (down & KEY_A)     return choice == 0;
        if (down & KEY_B)     return false;
        if (down & KEY_START) return false;

        ui_frame_begin();
        ui_draw_confirm(question, choice);
        ui_frame_end();
    }
}

static void action_install(void)
{
    const Game *g = selected_game();
    char err[512];
    char question[192];

    if (g == NULL) return;

    g_app.cancel = false;
    g_app.progress = 0.0f;
    g_app.ask_forwarder = true;
    snprintf(question, sizeof(question), "%s", tr("task.ask_forwarder"));
    snprintf(g_app.status, sizeof(g_app.status), "%s", tr("task.download"));
    g_app.previous = SCR_LIST;
    g_app.screen = SCR_TASK;

    ui_frame_begin();
    ui_draw_task(g_app.status, g_app.progress);
    ui_frame_end();

    if (scripts_run_with_prompt(g, task_progress, ask_forwarder_cb, question, err, sizeof(err))) {
        set_status(tr("task.done"));
    } else {
        snprintf(g_app.error, sizeof(g_app.error), "%s",
                 (g_app.cancel || err[0] == '\0') ? tr("task.failed") : err);
        g_app.previous = SCR_LIST;
        g_app.screen = SCR_ERROR;
        return;
    }
    g_app.screen = SCR_LIST;
}

static void action_search(void)
{
    SwkbdState   swkbd;
    char         buf[128];
    SwkbdButton  button;

    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, sizeof(buf) - 1);
    swkbdSetHintText(&swkbd, tr("search.title"));
    swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
    snprintf(buf, sizeof(buf), "%s", g_app.search);
    button = swkbdInputText(&swkbd, buf, sizeof(buf));

    if (button == SWKBD_BUTTON_CONFIRM) {
        snprintf(g_app.search, sizeof(g_app.search), "%s", buf);
        g_app.selected = 0;
        filter_build();
    }
}

static void action_edit_api(void)
{
    SwkbdState  swkbd;
    char        buf[512];
    SwkbdButton button;

    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, sizeof(buf) - 1);
    swkbdSetHintText(&swkbd, tr("settings.api"));
    swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
    snprintf(buf, sizeof(buf), "%s", g_app.config.api_base);
    button = swkbdInputText(&swkbd, buf, sizeof(buf));

    if (button == SWKBD_BUTTON_CONFIRM && buf[0] != '\0') {
        config_normalize_api(buf, sizeof(buf));
        snprintf(g_app.config.api_base, sizeof(g_app.config.api_base), "%s", buf);
        config_save(&g_app.config);
        load_catalog();
    }
}

static void settings_activate(int cursor)
{
    switch (cursor) {
        case 0:
            action_edit_api();
            break;
        case 1:
            snprintf(g_app.config.lang, sizeof(g_app.config.lang), "%s",
                     (strncmp(g_app.config.lang, "en", 2) == 0) ? "fr" : "en");
            config_save(&g_app.config);
            break;
        default:
            load_catalog();
            break;
    }
}

static void settings_reset(void)
{
    snprintf(g_app.config.api_base, sizeof(g_app.config.api_base), "%s", APP_API_DEFAULT);
    snprintf(g_app.config.lang, sizeof(g_app.config.lang), "%s", APP_LANG_DEFAULT);
    config_save(&g_app.config);
    set_status(tr("settings.reset"));
}

static void clamp_scroll(void)
{
    if (g_app.selected < g_app.scroll) g_app.scroll = g_app.selected;
    if (g_app.selected > g_app.scroll + 3) g_app.scroll = g_app.selected - 3;
    if (g_app.scroll < 0) g_app.scroll = 0;
}

static void handle_list(u32 down)
{
    if (down & KEY_DOWN)  if (g_app.selected + 1 < g_app.filter_count) g_app.selected++;
    if (down & KEY_UP)    if (g_app.selected > 0) g_app.selected--;
    if (down & KEY_RIGHT) if (g_app.selected + 4 < g_app.filter_count) g_app.selected += 4;
    if (down & KEY_LEFT)  g_app.selected = (g_app.selected >= 4) ? g_app.selected - 4 : 0;
    clamp_scroll();

    if (down & KEY_L) g_app.detail_scroll = (g_app.detail_scroll >= DESC_STEP) ? g_app.detail_scroll - DESC_STEP : 0;
    if (down & KEY_R) g_app.detail_scroll += DESC_STEP;

    if (down & KEY_A) action_install();
    if (down & KEY_X) action_search();
    if (down & KEY_Y) { g_app.cursor = 0; g_app.screen = SCR_SETTINGS; }
    if (down & KEY_SELECT) { g_app.detail_scroll = 0; g_app.screen = SCR_HELP; }
}

static void handle_settings(u32 down)
{
    if (down & KEY_DOWN) g_app.cursor = (g_app.cursor + 1) % 3;
    if (down & KEY_UP)   g_app.cursor = (g_app.cursor + 2) % 3;
    if (down & KEY_A)    settings_activate(g_app.cursor);
    if (down & KEY_X)    settings_reset();
    if (down & KEY_B)    g_app.screen = SCR_LIST;
}

static void handle_help(u32 down)
{
    if (down & KEY_L) g_app.detail_scroll = (g_app.detail_scroll >= 620) ? g_app.detail_scroll - 620 : 0;
    if (down & KEY_R) g_app.detail_scroll += 620;
    if (down & KEY_B) g_app.screen = SCR_LIST;
}

static void handle_error(u32 down)
{
    if (down & KEY_A) load_catalog();
    if (down & KEY_B) g_app.screen = SCR_LIST;
}

int main(void)
{
    memset(&g_app, 0, sizeof(g_app));
    g_app.running = true;

    gfxInitDefault();
    cfguInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    config_load(&g_app.config);

    if (!ui_init()) {
        C2D_Fini();
        C3D_Fini();
        cfguExit();
        gfxExit();
        return 1;
    }

    fs_mkdir_tree(APP_DIR);
    net_init();
    status_init();
    load_catalog();

    while (aptMainLoop() && g_app.running) {
        u32 down;

        hidScanInput();
        down = hidKeysDown();
        status_refresh();

        if (down & KEY_START) break;

        switch (g_app.screen) {
            case SCR_LIST:     handle_list(down); break;
            case SCR_SETTINGS: handle_settings(down); break;
            case SCR_HELP:     handle_help(down); break;
            case SCR_ERROR:    handle_error(down); break;
            default: break;
        }

        ui_frame_begin();
        switch (g_app.screen) {
            case SCR_LIST:     ui_draw_list(); break;
            case SCR_SETTINGS: ui_draw_settings(g_app.cursor); break;
            case SCR_TASK:     ui_draw_task(g_app.status, g_app.progress); break;
            case SCR_HELP:     ui_draw_help(g_app.detail_scroll); break;
            case SCR_ERROR:    ui_draw_error(g_app.error); break;
            default:           ui_draw_loading(g_app.status, g_app.progress); break;
        }
        ui_frame_end();

        if (g_app.status_ttl > 0) g_app.status_ttl--;
    }

    atlas_free();
    catalog_free(&g_app.catalog);
    free(g_app.filter);
    net_exit();
    status_exit();
    ui_exit();
    C2D_Fini();
    C3D_Fini();
    cfguExit();
    gfxExit();
    return 0;
}
