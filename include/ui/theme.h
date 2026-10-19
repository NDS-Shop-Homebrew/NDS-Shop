#pragma once

#include <3ds.h>

typedef struct {
    u32 bg;
    u32 fg;
    u32 card;
    u32 primary;
    u32 primary_fg;
    u32 accent;
    u32 accent_fg;
    u32 muted;
    u32 muted_fg;
    u32 border;
    u32 danger;
    u32 ok;
    u32 charging;
} Theme;

extern Theme g_theme;

void theme_apply(bool dark);

#define SCR_W_TOP 400.0f
#define SCR_H_TOP 240.0f
#define SCR_W_BOT 320.0f
#define SCR_H_BOT 240.0f