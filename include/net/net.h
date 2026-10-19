#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "core/app.h"

typedef struct {
    char  *data;
    size_t len;
    long   status;
} NetBuf;

bool net_init(void);
void net_exit(void);

bool net_get(const char *url, NetBuf *out);
void net_buf_free(NetBuf *b);

bool net_download(const char *url, const char *dest, ProgressFn cb, void *user);

const char *net_error(void);
