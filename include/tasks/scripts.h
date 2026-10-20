#pragma once

#include "core/app.h"

typedef bool (*AskForwarderFn)(void *user);

bool scripts_run(const Game *g, ProgressFn cb, void *user, char *err, size_t errcap);
bool scripts_run_with_prompt(const Game *g, ProgressFn cb, AskForwarderFn ask_forwarder,
                             void *user, char *err, size_t errcap);
