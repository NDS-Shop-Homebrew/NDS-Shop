#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "core/app.h"

bool cia_install(const char *sd_path, ProgressFn cb, void *user, char *err, size_t errcap);
