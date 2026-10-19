#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "core/app.h"

bool fs_mkdir_tree(const char *dir_path);
bool fs_mkdir_parents(const char *file_path);

bool  fs_exists(const char *path);
bool  fs_write_file(const char *path, const void *data, size_t len);
char *fs_read_file(const char *path, size_t *out_len);

void config_normalize_api(char *api, size_t cap);

void config_load(Config *cfg);
bool config_save(const Config *cfg);