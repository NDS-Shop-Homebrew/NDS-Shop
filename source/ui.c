// Interface de NDS-Shop (citro2d).
//
// Style repris du site : fond clair, accent bleu #0072CE, cartes à coins
// arrondis (12 px) et bordure fine.
#include "ui.h"

#include "assets.h"
#include "atlas.h"
#include "i18n.h"
#include "theme.h"

#include <stdio.h>
#include <string.h>

#include "ui_gfx_t3x.h"

#define TOP_W  400.0f
#define TOP_H  240.0f
#define BOT_W  320.0f
#define BOT_H  240.0f

#define HEADER_H    34.0f
#define ROW_H       48.0f
#define ROW_GAP     6.0f
#define ROW_X       6.0f
#define ROW_W       (TOP_W - 2 * ROW_X)

#define DESC_WINDOW 300
#define HELP_WINDOW 620

Theme g_theme;

static C2D_Font          s_font;
static C2D_TextBuf       s_text;
static C2D_SpriteSheet   s_ui;
static C3D_RenderTarget *s_top;
static C3D_RenderTarget *s_bottom;
static float             s_char_w = 8.0f;

// ---------------------------------------------------------------------------
// Palette
// ---------------------------------------------------------------------------

void theme_apply(bool dark)
{
    if (dark) {
        g_theme.bg         = C2D_Color32(0x0F, 0x17, 0x2A, 0xFF);
        g_theme.fg         = C2D_Color32(0xF1, 0xF5, 0xF9, 0xFF);
        g_theme.card       = C2D_Color32(0x1E, 0x29, 0x3B, 0xFF);
        g_theme.primary    = C2D_Color32(0x60, 0xA5, 0xFA, 0xFF);
        g_theme.primary_fg = C2D_Color32(0x0F, 0x17, 0x2A, 0xFF);
        g_theme.accent     = C2D_Color32(0x1E, 0x3A, 0x5F, 0xFF);
        g_theme.accent_fg  = C2D_Color32(0x60, 0xA5, 0xFA, 0xFF);
        g_theme.muted      = C2D_Color32(0x33, 0x41, 0x55, 0xFF);
        g_theme.muted_fg   = C2D_Color32(0x94, 0xA3, 0xB8, 0xFF);
        g_theme.border     = C2D_Color32(0x33, 0x41, 0x55, 0xFF);
        g_theme.danger     = C2D_Color32(0xEF, 0x44, 0x44, 0xFF);
    } else {
        g_theme.bg         = C2D_Color32(0xF8, 0xFA, 0xFC, 0xFF);
        g_theme.fg         = C2D_Color32(0x0F, 0x17, 0x2A, 0xFF);
        g_theme.card       = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
        g_theme.primary    = C2D_Color32(0x00, 0x72, 0xCE, 0xFF);
        g_theme.primary_fg = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
        g_theme.accent     = C2D_Color32(0xE8, 0xF4, 0xFD, 0xFF);
        g_theme.accent_fg  = C2D_Color32(0x00, 0x72, 0xCE, 0xFF);
        g_theme.muted      = C2D_Color32(0xF1, 0xF5, 0xF9, 0xFF);
        g_theme.muted_fg   = C2D_Color32(0x64, 0x74, 0x8B, 0xFF);
        g_theme.border     = C2D_Color32(0xE2, 0xE8, 0xF0, 0xFF);
        g_theme.danger     = C2D_Color32(0xDC, 0x26, 0x26, 0xFF);
    }
}

// ---------------------------------------------------------------------------
// Cycle de vie
// ---------------------------------------------------------------------------

static bool parse_text(C2D_Text *out, const char *str)
{
    if (str == NULL || str[0] == '\0') return false;
    if (s_font != NULL) return C2D_TextFontParse(out, s_font, s_text, str) != NULL;
    return C2D_TextParse(out, s_text, str) != NULL;
}

bool ui_init(void)
{
    static const char *probe = "n";

    s_font = C2D_FontLoadSystem(CFG_REGION_EUR);
    s_text = C2D_TextBufNew(16384);
    s_ui = C2D_SpriteSheetLoadFromMem(ui_gfx_t3x, ui_gfx_t3x_size);
    s_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    s_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    if (s_text != NULL) {
        C2D_Text t;
        if (parse_text(&t, probe)) {
            float w = 0.0f, h = 0.0f;
            C2D_TextGetDimensions(&t, 1.0f, 1.0f, &w, &h);
            if (w > 0.0f) s_char_w = w;
        }
        C2D_TextBufClear(s_text);
    }

    theme_apply(false);
    return s_text != NULL && s_top != NULL && s_bottom != NULL;
}

void ui_exit(void)
{
    if (s_ui != NULL) C2D_SpriteSheetFree(s_ui);
    if (s_text != NULL) C2D_TextBufDelete(s_text);
    if (s_font != NULL) C2D_FontFree(s_font);
    s_ui = NULL;
    s_text = NULL;
    s_font = NULL;
}

void ui_frame_begin(void)
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    if (s_text != NULL) C2D_TextBufClear(s_text);
    C2D_TargetClear(s_top, g_theme.bg);
    C2D_TargetClear(s_bottom, g_theme.bg);
}

void ui_frame_end(void)
{
    C3D_FrameEnd(0);
}

// ---------------------------------------------------------------------------
// Primitives
// ---------------------------------------------------------------------------

static void text_at(const char *str, float x, float y, float scale, u32 color)
{
    C2D_Text t;
    if (!parse_text(&t, str)) return;
    C2D_TextOptimize(&t);
    C2D_DrawText(&t, C2D_WithColor, x, y, 0.5f, scale, scale, color);
}

static float text_w(const char *str, float scale)
{
    C2D_Text t;
    float w = 0.0f, h = 0.0f;
    if (!parse_text(&t, str)) return 0.0f;
    C2D_TextGetDimensions(&t, scale, scale, &w, &h);
    return w;
}

static void text_right(const char *str, float x_right, float y, float scale, u32 color)
{
    text_at(str, x_right - text_w(str, scale), y, scale, color);
}

static void text_center(const char *str, float x_center, float y, float scale, u32 color)
{
    text_at(str, x_center - text_w(str, scale) / 2.0f, y, scale, color);
}

static void text_render_wrap(const char *str, float x, float y, float box_w, float scale, u32 color)
{
    C2D_Text t;
    if (!parse_text(&t, str)) return;
    C2D_TextOptimize(&t);
    C2D_DrawText(&t, C2D_WithColor | C2D_WordWrap, x, y, 0.5f, scale, scale, color, box_w);
}

static void fill_round(float x, float y, float w, float h, float r, u32 color)
{
    if (w <= 0.0f || h <= 0.0f) return;
    if (r * 2.0f > w) r = w / 2.0f;
    if (r * 2.0f > h) r = h / 2.0f;
    if (r < 0.0f) r = 0.0f;
    C2D_DrawRectSolid(x + r, y, 0.9f, w - 2.0f * r, h, color);
    C2D_DrawRectSolid(x, y + r, 0.9f, w, h - 2.0f * r, color);
    if (r > 0.0f) {
        C2D_DrawCircleSolid(x + r, y + r, 0.9f, r, color);
        C2D_DrawCircleSolid(x + w - r, y + r, 0.9f, r, color);
        C2D_DrawCircleSolid(x + r, y + h - r, 0.9f, r, color);
        C2D_DrawCircleSolid(x + w - r, y + h - r, 0.9f, r, color);
    }
}

// Carte : fond arrondi + bordure d'un pixel (bordure dessinée en dessous).
static void card(float x, float y, float w, float h, u32 fill, u32 border)
{
    fill_round(x, y, w, h, UI_RADIUS, border);
    fill_round(x + 1.0f, y + 1.0f, w - 2.0f, h - 2.0f, UI_RADIUS - 1.0f, fill);
}

static void draw_icon(int index, float x, float y, float size, u32 color)
{
    C2D_Image img;
    C2D_ImageTint tint;
    if (s_ui == NULL) return;
    if (index < 0 || (size_t)index >= C2D_SpriteSheetCount(s_ui)) return;
    img = C2D_SpriteSheetGetImage(s_ui, (size_t)index);
    C2D_PlainImageTint(&tint, color, 1.0f);
    C2D_DrawImageAt(img, x, y, 0.5f, &tint,
                    size / (float)img.subtex->width, size / (float)img.subtex->height);
}

static void draw_game_icon(int icon_index, float x, float y, float size)
{
    C2D_Image img;
    if (icon_index >= 0 && atlas_get(icon_index, &img)) {
        C2D_DrawImageAt(img, x, y, 0.5f, NULL,
                        size / (float)img.subtex->width, size / (float)img.subtex->height);
        return;
    }
    fill_round(x, y, size, size, 6.0f, g_theme.muted);
    draw_icon(ICON_CARTRIDGE, x + size * 0.15f, y + size * 0.15f, size * 0.7f, g_theme.muted_fg);
}

static void draw_progress(float x, float y, float w, float h, float frac)
{
    float fw;
    fill_round(x, y, w, h, h / 2.0f, g_theme.muted);
    if (frac <= 0.0f) return;
    if (frac > 1.0f) frac = 1.0f;
    fw = w * frac;
    if (fw < h) fw = h;
    fill_round(x, y, fw, h, h / 2.0f, g_theme.primary);
}

// Tronque `src` pour tenir dans `max_w` (approx. via la largeur moyenne).
static void ellipsize(const char *src, char *out, size_t cap, float max_w, float scale)
{
    size_t max_chars;
    size_t len;
    float  unit = s_char_w * scale;

    snprintf(out, cap, "%s", src != NULL ? src : "");
    if (unit <= 0.0f) return;
    max_chars = (size_t)(max_w / unit);
    if (max_chars < 4) max_chars = 4;

    len = strlen(out);
    if (len <= max_chars) return;

    len = max_chars;
    while (len > 0 && ((unsigned char)out[len] & 0xC0) == 0x80) len--;

    // Ajout explicite du caractère de suspension (3 octets UTF-8).
    if (len + 3 >= cap) len = (cap > 4) ? cap - 4 : 0;
    memcpy(out + len, "\u2026", 3);
    out[len + 3] = '\0';
}

// Fenêtre de texte (pagination par caractères, sans découpe de caractère).
static void window(const char *src, int scroll, int span, char *out, size_t cap)
{
    size_t start;
    size_t len;
    size_t n;

    out[0] = '\0';
    if (src == NULL) return;
    len = strlen(src);
    start = (size_t)(scroll < 0 ? 0 : scroll);
    if (start >= len) start = len > 0 ? len - 1 : 0;
    while (start > 0 && ((unsigned char)src[start] & 0xC0) == 0x80) start--;

    n = len - start;
    if (n > (size_t)span) {
        n = (size_t)span;
        while (start + n < len && ((unsigned char)src[start + n] & 0xC0) == 0x80) n--;
    }
    if (n >= cap) n = cap - 1;
    memcpy(out, src + start, n);
    out[n] = '\0';
}

// ---------------------------------------------------------------------------
// Blocs communs
// ---------------------------------------------------------------------------

static void draw_header(const char *right_label)
{
    char subtitle[192];

    C2D_DrawRectSolid(0.0f, 0.0f, 0.8f, TOP_W, HEADER_H, g_theme.card);
    C2D_DrawRectSolid(0.0f, HEADER_H - 1.0f, 0.8f, TOP_W, 1.0f, g_theme.border);

    draw_icon(ICON_LOGO, 8.0f, 6.0f, 22.0f, g_theme.primary);
    text_at(APP_TITLE, 36.0f, 4.0f, 0.6f, g_theme.fg);

    if (g_app.catalog.store.title[0] != '\0') {
        snprintf(subtitle, sizeof(subtitle), "%s \u00b7 %d/%d",
                 g_app.catalog.store.title, g_app.filter_count, g_app.catalog.count);
    } else {
        snprintf(subtitle, sizeof(subtitle), "%d/%d", g_app.filter_count, g_app.catalog.count);
    }
    text_at(subtitle, 36.0f, 20.0f, 0.38f, g_theme.muted_fg);

    if (right_label != NULL && right_label[0] != '\0') {
        if (g_app.offline) {
            text_right(tr("loading.offline"), TOP_W - 10.0f, 12.0f, 0.38f, g_theme.danger);
        } else {
            text_right(right_label, TOP_W - 10.0f, 12.0f, 0.38f, g_theme.muted_fg);
        }
    }
}

static void draw_bottom_hint(const char *hint)
{
    C2D_DrawRectSolid(0.0f, BOT_H - 22.0f, 0.8f, BOT_W, 22.0f, g_theme.card);
    C2D_DrawRectSolid(0.0f, BOT_H - 22.0f, 0.8f, BOT_W, 1.0f, g_theme.border);
    text_at(hint, 8.0f, BOT_H - 18.0f, 0.36f, g_theme.muted_fg);
}

static const Game *game_at(int visible)
{
    int index;
    if (visible < 0 || visible >= g_app.filter_count) return NULL;
    index = g_app.filter[visible];
    if (index < 0 || index >= g_app.catalog.count) return NULL;
    return &g_app.catalog.games[index];
}

static bool game_has_rom(const Game *g)
{
    int i;
    for (i = 0; i < g->download_count; i++) {
        size_t n = strlen(g->downloads[i].filename);
        if (n > 4 && (g->downloads[i].filename[n - 1] == 's' || g->downloads[i].filename[n - 1] == 'S') &&
            (g->downloads[i].filename[n - 2] == 'd' || g->downloads[i].filename[n - 2] == 'D') &&
            (g->downloads[i].filename[n - 3] == 'n' || g->downloads[i].filename[n - 3] == 'N')) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Écrans
// ---------------------------------------------------------------------------

void ui_draw_loading(const char *label, float frac)
{
    C2D_SceneBegin(s_top);
    draw_icon(ICON_LOGO, TOP_W / 2.0f - 26.0f, 48.0f, 52.0f, g_theme.primary);
    text_center(APP_TITLE, TOP_W / 2.0f, 110.0f, 0.85f, g_theme.fg);
    text_center(tr("app.tagline"), TOP_W / 2.0f, 132.0f, 0.45f, g_theme.muted_fg);
    text_center(label != NULL ? label : "", TOP_W / 2.0f, 168.0f, 0.45f, g_theme.muted_fg);
    draw_progress(TOP_W / 2.0f - 110.0f, 190.0f, 220.0f, 10.0f, frac);

    C2D_SceneBegin(s_bottom);
    text_center(g_app.config.api_base, BOT_W / 2.0f, 110.0f, 0.38f, g_theme.muted_fg);
    draw_bottom_hint(tr("hint.quit"));
}

void ui_draw_list(void)
{
    char  right[192];
    char  line[256];
    int   i;
    float y;

    // --- Écran supérieur : liste ---
    C2D_SceneBegin(s_top);

    if (g_app.search[0] != '\0') {
        snprintf(right, sizeof(right), "%s %s", tr("search.label"), g_app.search);
    } else {
        snprintf(right, sizeof(right), "%s", tr("search.hint"));
    }
    draw_header(right);

    if (g_app.filter_count == 0) {
        text_center(tr("list.empty"), TOP_W / 2.0f, 110.0f, 0.5f, g_theme.muted_fg);
    }

    for (i = 0; i < 4; i++) {
        int visible = g_app.scroll + i;
        const Game *g = game_at(visible);
        bool selected = (visible == g_app.selected);
        char title[256];
        char meta[320];

        if (g == NULL) break;
        y = HEADER_H + 4.0f + (float)i * (ROW_H + ROW_GAP);

        if (selected) card(ROW_X, y, ROW_W, ROW_H, g_theme.accent, g_theme.primary);
        else          card(ROW_X, y, ROW_W, ROW_H, g_theme.card, g_theme.border);

        draw_game_icon(g->icon_index, ROW_X + 8.0f, y + 6.0f, 36.0f);

        ellipsize(g->title, title, sizeof(title), ROW_W - 90.0f, 0.52f);
        text_at(title, ROW_X + 52.0f, y + 7.0f, 0.52f, selected ? g_theme.accent_fg : g_theme.fg);

        snprintf(meta, sizeof(meta), "%s", g->author[0] ? g->author : g->developer);
        if (g->version[0] != '\0') {
            size_t used = strlen(meta);
            snprintf(meta + used, sizeof(meta) - used, "%s%s", used ? " \u00b7 " : "", g->version);
        }
        text_at(meta, ROW_X + 52.0f, y + 27.0f, 0.36f, g_theme.muted_fg);

        if (game_has_rom(g)) {
            draw_icon(ICON_DOWNLOAD, ROW_X + ROW_W - 26.0f, y + 14.0f, 18.0f, g_theme.primary);
        }
    }

    if (g_app.filter_count > 4) {
        float track_h = TOP_H - HEADER_H - 12.0f;
        float ratio = (float)g_app.filter_count / 4.0f;
        float thumb_h = track_h / ratio;
        float thumb_y = HEADER_H + 6.0f + (track_h - thumb_h) * ((float)g_app.scroll / (float)(g_app.filter_count - 4));
        C2D_DrawRectSolid(TOP_W - 3.0f, HEADER_H + 6.0f, 0.8f, 2.0f, track_h, g_theme.muted);
        fill_round(TOP_W - 3.0f, thumb_y, 2.0f, thumb_h, 1.0f, g_theme.muted_fg);
    }

    // --- Écran inférieur : détail ---
    C2D_SceneBegin(s_bottom);

    {
        const Game *g = game_at(g_app.selected);
        if (g == NULL) {
            text_center(tr("list.none"), BOT_W / 2.0f, 100.0f, 0.5f, g_theme.muted_fg);
        } else {
            char title[256];
            char meta[320];
            char desc[DESC_WINDOW + 8];

            ellipsize(g->title, title, sizeof(title), BOT_W - 16.0f, 0.62f);
            text_render_wrap(title, 8.0f, 8.0f, BOT_W - 16.0f, 0.62f, g_theme.fg);

            snprintf(line, sizeof(line), "%s%s%s",
                     g->author[0] ? g->author : (g->developer[0] ? g->developer : ""),
                     g->version[0] ? " \u00b7 " : "", g->version);
            snprintf(meta, sizeof(meta), "%s  \u00b7  %s", line[0] ? line : "Nintendo DS", "Nintendo DS");
            text_at(meta, 8.0f, 34.0f, 0.4f, g_theme.muted_fg);

            card(6.0f, 54.0f, BOT_W - 12.0f, 90.0f, g_theme.card, g_theme.border);
            window(g->description, g_app.detail_scroll, DESC_WINDOW, desc, sizeof(desc));
            if (desc[0] == '\0') text_at(tr("list.none"), 14.0f, 60.0f, 0.38f, g_theme.muted_fg);
            else text_render_wrap(desc, 14.0f, 60.0f, BOT_W - 28.0f, 0.4f, g_theme.muted_fg);

            if (game_has_rom(g)) {
                card(6.0f, 152.0f, BOT_W - 12.0f, 38.0f, g_theme.primary, g_theme.primary);
                draw_icon(ICON_DOWNLOAD, 18.0f, 162.0f, 18.0f, g_theme.primary_fg);
                text_at(tr("action.download"), 44.0f, 161.0f, 0.5f, g_theme.primary_fg);
            } else {
                card(6.0f, 152.0f, BOT_W - 12.0f, 38.0f, g_theme.muted, g_theme.border);
                draw_icon(ICON_WARNING, 18.0f, 162.0f, 18.0f, g_theme.muted_fg);
                text_at(tr("detail.norom"), 44.0f, 161.0f, 0.5f, g_theme.muted_fg);
            }
        }
    }
    draw_bottom_hint((g_app.status_ttl > 0 && g_app.status[0] != '\0') ? g_app.status : tr("hint.list"));
}

void ui_draw_settings(int cursor)
{
    static const char *keys[4] = { "settings.api", "settings.lang", "settings.theme", "settings.reload" };
    char  value[512];
    int   i;
    float y;

    C2D_SceneBegin(s_top);
    draw_header("");
    text_at(tr("settings.title"), 10.0f, HEADER_H + 6.0f, 0.6f, g_theme.fg);

    for (i = 0; i < 4; i++) {
        bool selected = (i == cursor);
        y = HEADER_H + 30.0f + (float)i * 40.0f;

        if (selected) card(ROW_X, y, ROW_W, 34.0f, g_theme.accent, g_theme.primary);
        else          card(ROW_X, y, ROW_W, 34.0f, g_theme.card, g_theme.border);

        text_at(tr(keys[i]), ROW_X + 12.0f, y + 9.0f, 0.45f, selected ? g_theme.accent_fg : g_theme.fg);

        value[0] = '\0';
        if (i == 0) {
            snprintf(value, sizeof(value), "%s", g_app.config.api_base);
        } else if (i == 1) {
            snprintf(value, sizeof(value), "%s", g_app.config.lang);
        } else if (i == 2) {
            snprintf(value, sizeof(value), "%s", g_app.config.dark ? tr("settings.dark") : tr("settings.light"));
        }
        if (value[0] != '\0') {
            char short_value[128];
            ellipsize(value, short_value, sizeof(short_value), 180.0f, 0.4f);
            text_right(short_value, TOP_W - 26.0f, y + 11.0f, 0.4f, g_theme.muted_fg);
        } else {
            draw_icon(ICON_REFRESH, TOP_W - 28.0f, y + 8.0f, 16.0f, g_theme.primary);
        }
    }

    C2D_SceneBegin(s_bottom);
    text_at(tr("settings.hint"), 8.0f, 12.0f, 0.42f, g_theme.muted_fg);
    text_at(tr("settings.reset"), 8.0f, 34.0f, 0.42f, g_theme.muted_fg);
    text_render_wrap(tr("app.tagline"), 8.0f, 70.0f, BOT_W - 16.0f, 0.42f, g_theme.muted_fg);
    draw_bottom_hint(tr("hint.back"));
}

void ui_draw_task(const char *label, float frac)
{
    char percent[32];

    C2D_SceneBegin(s_top);
    draw_header("");
    text_center(tr("task.title"), TOP_W / 2.0f, 70.0f, 0.7f, g_theme.fg);
    text_center(label != NULL ? label : "", TOP_W / 2.0f, 104.0f, 0.48f, g_theme.muted_fg);
    draw_progress(TOP_W / 2.0f - 120.0f, 132.0f, 240.0f, 12.0f, frac);
    snprintf(percent, sizeof(percent), "%d %%", (int)(frac * 100.0f));
    text_center(frac >= 0.0f ? percent : "\u2026", TOP_W / 2.0f, 152.0f, 0.45f, g_theme.muted_fg);

    C2D_SceneBegin(s_bottom);
    draw_bottom_hint(tr("task.cancel"));
}

void ui_draw_help(int scroll)
{
    char chunk[HELP_WINDOW + 8];

    C2D_SceneBegin(s_top);
    draw_header("");
    draw_icon(ICON_HELP, 10.0f, HEADER_H + 6.0f, 18.0f, g_theme.primary);
    text_at(tr("help.title"), 32.0f, HEADER_H + 6.0f, 0.55f, g_theme.fg);

    window(tr("help.body"), scroll, HELP_WINDOW, chunk, sizeof(chunk));
    text_render_wrap(chunk, 10.0f, HEADER_H + 30.0f, TOP_W - 20.0f, 0.42f, g_theme.fg);

    C2D_SceneBegin(s_bottom);
    text_at(tr("help.scroll"), 8.0f, 12.0f, 0.42f, g_theme.muted_fg);
    draw_bottom_hint(tr("hint.back"));
}

void ui_draw_error(const char *message)
{
    C2D_SceneBegin(s_top);
    draw_header("");
    draw_icon(ICON_WARNING, 10.0f, HEADER_H + 8.0f, 20.0f, g_theme.danger);
    text_at(tr("error.title"), 34.0f, HEADER_H + 8.0f, 0.6f, g_theme.danger);
    text_render_wrap(message != NULL ? message : "", 10.0f, HEADER_H + 40.0f, TOP_W - 20.0f, 0.45f, g_theme.fg);

    C2D_SceneBegin(s_bottom);
    draw_bottom_hint(tr("error.hint"));
}
