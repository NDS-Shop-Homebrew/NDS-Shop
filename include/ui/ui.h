#pragma once

#include <3ds.h>
#include <citro2d.h>

#include "core/app.h"
#include "ui/theme.h"

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
void ui_draw_confirm(const char *question, int choice);
