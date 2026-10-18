// Exécution des actions d'installation fournies par le catalogue.
#pragma once

#include "app.h"

// Exécute les scripts du jeu (téléchargement de la ROM, installation du
// forwarder .cia, nettoyage). `cb` rapporte la progression globale (0..1).
// En cas d'échec, `err` reçoit un message lisible.
bool scripts_run(const Game *g, ProgressFn cb, void *user, char *err, size_t errcap);
