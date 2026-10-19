#pragma once

#include "core/app.h"
#include "net/net.h"

bool catalog_parse_page(const char *json, Catalog *cat);
bool catalog_load(Catalog *cat, ProgressFn cb, void *user);
void catalog_free(Catalog *cat);
