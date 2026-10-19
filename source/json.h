// Analyseur JSON minimal (dérivé de jsmn, domaine public) + accès par jeton.
#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    JSON_UNDEFINED = 0,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_PRIMITIVE,
} JsonType;

typedef struct {
    JsonType type;
    int start;
    int end;
    int size;    // enfants (clés + valeurs pour un objet, éléments pour un tableau)
    int parent;
} JsonTok;

typedef struct {
    unsigned pos;
    unsigned next;
    int super;
} JsonParser;

enum {
    JSON_ERR_NOMEM = -1,
    JSON_ERR_INVAL = -2,
    JSON_ERR_PART  = -3,
};

void json_init(JsonParser *p);
int  json_run(JsonParser *p, const char *s, size_t len, JsonTok *toks, unsigned cap);

// Index du premier jeton suivant le sous-arbre `idx`.
int json_next(const JsonTok *toks, int idx);
// Valeur associée à `key` dans l'objet `obj`, ou -1.
int json_key(const char *s, const JsonTok *toks, int obj, const char *key);
// i-ème élément du tableau `arr`, ou -1.
int json_at(const JsonTok *toks, int arr, int i);
// Nombre d'éléments d'un tableau (0 sinon).
int json_len(const JsonTok *toks, int arr);

bool json_is_str(const JsonTok *toks, int idx);
bool json_is_object(const JsonTok *toks, int idx);

// Copie la chaîne (déséchappée). `out` est toujours terminé par NUL.
void json_get_str(const char *s, const JsonTok *toks, int idx, char *out, size_t cap);
long long json_get_int(const char *s, const JsonTok *toks, int idx, long long def);
