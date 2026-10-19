// Installation d'un titre (.cia) via le service AM.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app.h"

// Installe le fichier .cia situé à `sd_path` (ex. "sdmc:/jeu.cia").
// Renvoie false en cas d'échec ; `err` reçoit alors un message lisible.
bool cia_install(const char *sd_path, ProgressFn cb, void *user, char *err, size_t errcap);
