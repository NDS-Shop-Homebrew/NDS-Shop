// Utilitaires carte SD : dossiers, fichiers, configuration.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app.h"

// Crée récursivement les dossiers d'un chemin complet (le dernier élément
// étant considéré comme un fichier). Renvoie true si les dossiers existent.
bool fs_mkdir_tree(const char *dir_path);
bool fs_mkdir_parents(const char *file_path);

bool  fs_exists(const char *path);
bool  fs_write_file(const char *path, const void *data, size_t len);
char *fs_read_file(const char *path, size_t *out_len);  // à libérer with free()

// Normalise une adresse d'API : espaces de fin retirés, '/' final supprimé et
// suffixe "/api" redondant enlevé (l'application ajoute elle-même "/api/v1/...").
// Une valeur devenue vide est remplacée par APP_API_DEFAULT.
void config_normalize_api(char *api, size_t cap);

void config_load(Config *cfg);
bool config_save(const Config *cfg);
