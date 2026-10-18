// Atlas d'icônes de la boutique (store.sheet), téléchargé puis mis en cache.
//
// Le serveur publie un atlas .t3x : l'index d'une icône est fourni par le
// catalogue (champ `icon_index`). L'atlas est conservé sur la carte SD pour le
// mode hors-ligne.
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

#include "app.h"

// Nombre d'icônes disponibles (0 si l'atlas n'est pas chargé).
int atlas_count(void);
bool atlas_ready(void);

// Image de l'icône `index` ; false si l'index est invalide.
bool atlas_get(int index, C2D_Image *out);

// Libère l'atlas courant.
void atlas_free(void);

// Charge l'atlas depuis la SD si l'URL en cache correspond à `url`.
// Renvoie le nombre d'icônes chargées (0 si absent ou obsolète).
int atlas_load_cached(const char *url);

// Télécharge l'atlas de `url` puis le charge.
bool atlas_fetch(const char *url, ProgressFn cb, void *user);
