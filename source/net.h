// Accès réseau (libcurl + mbedtls via la puce SOC de la 3DS).
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app.h"

typedef struct {
    char  *data;
    size_t len;
    long   status;
} NetBuf;

bool net_init(void);
void net_exit(void);

// GET `url` en mémoire. Renvoie true si le code HTTP est 2xx.
bool net_get(const char *url, NetBuf *out);
void net_buf_free(NetBuf *b);

// GET `url` vers le fichier `dest` (chemin sdmc:). `cb` reçoit la progression
// (fraction 0..1, ou <0 si la taille est inconnue).
bool net_download(const char *url, const char *dest, ProgressFn cb, void *user);

// Dernier message d'erreur lisible (jamais NULL).
const char *net_error(void);
