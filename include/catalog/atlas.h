#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

#include "core/app.h"

int atlas_count(void);
bool atlas_ready(void);
bool atlas_get(int index, C2D_Image *out);
void atlas_free(void);

int atlas_load_cached(const char *url);
bool atlas_fetch(const char *url, ProgressFn cb, void *user);
