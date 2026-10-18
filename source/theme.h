// Thème visuel : palette reprise du site (db-nds-shop).
#pragma once

#include <3ds.h>

typedef struct {
    u32 bg;          // fond de page
    u32 fg;          // texte principal
    u32 card;        // fond de carte
    u32 primary;     // bleu NDS-Shop
    u32 primary_fg;  // texte sur bleu
    u32 accent;      // fond d'accent discret
    u32 accent_fg;   // texte d'accent
    u32 muted;       // fond atténué
    u32 muted_fg;    // texte secondaire
    u32 border;      // bordure fine
    u32 danger;      // erreur
} Theme;

extern Theme g_theme;

void theme_apply(bool dark);

// Rayon des cartes (aligné sur la classe .dsi-card du site).
#define UI_RADIUS 12.0f

#define SCR_W_TOP 400.0f
#define SCR_H_TOP 240.0f
#define SCR_W_BOT 320.0f
#define SCR_H_BOT 240.0f
