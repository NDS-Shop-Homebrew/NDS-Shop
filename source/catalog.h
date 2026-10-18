// Chargement du catalogue /api/v1/catalog.
#pragma once

#include "app.h"
#include "net.h"

// Analyse pure d'une page de catalogue (aucun accès réseau) : les jeux
// s'ajoutent à `cat`, la boutique et la révision sont remplacées.
// Renvoie false si la structure attendue est absente.
bool catalog_parse_page(const char *json, Catalog *cat);

// Récupère le catalogue complet (pages successives) ; si le réseau échoue,
// tente la copie locale mise en cache. `cb` rapporte la progression.
bool catalog_load(Catalog *cat, ProgressFn cb, void *user);

void catalog_free(Catalog *cat);
