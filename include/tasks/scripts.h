#pragma once

#include "core/app.h"

bool scripts_run(const Game *g, ProgressFn cb, void *user, char *err, size_t errcap);
