// Interface : rendu de l'application (citro2d).
#pragma once

#include <3ds.h>
#include <citro2d.h>

#include "app.h"
#include "theme.h"

bool ui_init(void);
void ui_exit(void);

void ui_frame_begin(void);
void ui_frame_end(void);

void ui_draw_loading(const char *label, float frac);
void ui_draw_list(void);
void ui_draw_settings(int cursor);
void ui_draw_task(const char *label, float frac);
void ui_draw_help(int scroll);
void ui_draw_error(const char *message);

// Palette courante (définie dans ui.c) et thème.
extern Theme g_theme;
void theme_apply(bool dark);
