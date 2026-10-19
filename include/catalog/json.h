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
    int size;
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

int json_next(const JsonTok *toks, int idx);
int json_key(const char *s, const JsonTok *toks, int obj, const char *key);
int json_at(const JsonTok *toks, int arr, int i);
int json_len(const JsonTok *toks, int arr);

bool json_is_str(const JsonTok *toks, int idx);
bool json_is_object(const JsonTok *toks, int idx);

void json_get_str(const char *s, const JsonTok *toks, int idx, char *out, size_t cap);
long long json_get_int(const char *s, const JsonTok *toks, int idx, long long def);
