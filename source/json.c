// Analyseur JSON minimal — adaptation de jsmn (Serge Zaitsev, domaine public).
#include "json.h"

#include <stdlib.h>
#include <string.h>

static JsonTok *alloc_tok(JsonParser *p, JsonTok *toks, unsigned cap)
{
    JsonTok *t;
    if (p->next >= cap) return NULL;
    t = &toks[p->next++];
    t->start = t->end = -1;
    t->size = 0;
    t->parent = -1;
    return t;
}

static int parse_string(JsonParser *p, const char *s, size_t len, JsonTok *toks, unsigned cap)
{
    int start = (int)p->pos;
    p->pos++;
    for (; p->pos < len && s[p->pos] != '\0'; p->pos++) {
        char c = s[p->pos];
        if (c == '"') {
            JsonTok *t;
            if (toks == NULL) return 0;
            t = alloc_tok(p, toks, cap);
            if (t == NULL) { p->pos = (unsigned)start; return JSON_ERR_NOMEM; }
            t->type = JSON_STRING;
            t->start = start + 1;
            t->end = (int)p->pos;
            t->parent = p->super;
            // Le guillemet fermant est le dernier caractère consommé : la
            // boucle principale avancera juste après.
            return 0;
        }
        if (c == '\\' && p->pos + 1 < len) {
            p->pos++;
            switch (s[p->pos]) {
                case '"': case '/': case '\\': case 'b':
                case 'f': case 'r': case 'n': case 't':
                    break;
                case 'u': {
                    int i;
                    p->pos++;
                    for (i = 0; i < 4 && p->pos < len && s[p->pos] != '\0'; i++) {
                        char h = s[p->pos];
                        if (!((h >= '0' && h <= '9') || (h >= 'a' && h <= 'f') || (h >= 'A' && h <= 'F'))) {
                            p->pos = (unsigned)start;
                            return JSON_ERR_INVAL;
                        }
                        p->pos++;
                    }
                    p->pos--;
                    break;
                }
                default:
                    p->pos = (unsigned)start;
                    return JSON_ERR_INVAL;
            }
        }
    }
    p->pos = (unsigned)start;
    return JSON_ERR_PART;
}

static int parse_primitive(JsonParser *p, const char *s, size_t len, JsonTok *toks, unsigned cap)
{
    int start = (int)p->pos;
    for (; p->pos < len && s[p->pos] != '\0'; p->pos++) {
        char c = s[p->pos];
        if (c == ':' || c == '\t' || c == '\r' || c == '\n' || c == ' ' ||
            c == ',' || c == ']' || c == '}') {
            break;
        }
        if (c < 32 || c >= 127) {
            p->pos = (unsigned)start;
            return JSON_ERR_INVAL;
        }
    }
    if (toks != NULL) {
        JsonTok *t = alloc_tok(p, toks, cap);
        if (t == NULL) { p->pos = (unsigned)start; return JSON_ERR_NOMEM; }
        t->type = JSON_PRIMITIVE;
        t->start = start;
        t->end = (int)p->pos;
        t->parent = p->super;
    }
    p->pos--;
    return 0;
}

void json_init(JsonParser *p)
{
    p->pos = 0;
    p->next = 0;
    p->super = -1;
}

int json_run(JsonParser *p, const char *s, size_t len, JsonTok *toks, unsigned cap)
{
    int i;
    int count = (int)p->next;
    int r;

    for (; p->pos < len && s[p->pos] != '\0'; p->pos++) {
        char c = s[p->pos];
        switch (c) {
            case '{':
            case '[': {
                JsonTok *t;
                count++;
                if (toks == NULL) break;
                t = alloc_tok(p, toks, cap);
                if (t == NULL) return JSON_ERR_NOMEM;
                if (p->super != -1) toks[p->super].size++;
                t->type = (c == '{') ? JSON_OBJECT : JSON_ARRAY;
                t->start = (int)p->pos;
                t->parent = p->super;
                p->super = (int)p->next - 1;
                break;
            }
            case '}':
            case ']': {
                JsonType type = (c == '}') ? JSON_OBJECT : JSON_ARRAY;
                if (toks == NULL) break;
                for (i = (int)p->next - 1; i >= 0; i--) {
                    JsonTok *t = &toks[i];
                    if (t->start != -1 && t->end == -1) {
                        if (t->type != type) return JSON_ERR_INVAL;
                        p->super = -1;
                        t->end = (int)p->pos + 1;
                        break;
                    }
                }
                if (i == -1) return JSON_ERR_INVAL;
                for (; i >= 0; i--) {
                    JsonTok *t = &toks[i];
                    if (t->start != -1 && t->end == -1) {
                        p->super = i;
                        break;
                    }
                }
                break;
            }
            case '"':
                r = parse_string(p, s, len, toks, cap);
                if (r < 0) return r;
                count++;
                if (p->super != -1 && toks != NULL) toks[p->super].size++;
                break;
            case '\t': case '\r': case '\n': case ' ':
                break;
            case ':':
                p->super = (int)p->next - 1;
                break;
            case ',': {
                if (toks != NULL && p->super != -1 &&
                    toks[p->super].type != JSON_ARRAY && toks[p->super].type != JSON_OBJECT) {
                    for (i = (int)p->next - 1; i >= 0; i--) {
                        if ((toks[i].type == JSON_ARRAY || toks[i].type == JSON_OBJECT) &&
                            toks[i].start != -1 && toks[i].end == -1) {
                            p->super = i;
                            break;
                        }
                    }
                }
                break;
            }
            case '-': case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case 't': case 'f': case 'n':
                r = parse_primitive(p, s, len, toks, cap);
                if (r < 0) return r;
                count++;
                if (p->super != -1 && toks != NULL) toks[p->super].size++;
                break;
            default:
                return JSON_ERR_INVAL;
        }
    }

    for (i = (int)p->next - 1; i >= 0; i--) {
        if (toks[i].start != -1 && toks[i].end == -1) return JSON_ERR_PART;
    }
    return count;
}

// Dernier jeton du sous-arbre commençant à `idx`.
//
// Convention jsmn : `size` vaut le nombre de paires pour un objet et le nombre
// d'éléments pour un tableau. Un objet compte donc 2 x size jetons enfants
// (clés et valeurs). Le champ `size` d'une clé n'est pas fiable, d'où le test
// sur le type avant toute descente.
static int tok_last(const JsonTok *toks, int idx)
{
    int      i = idx;
    int      c;
    int      children;
    JsonType type = toks[idx].type;

    if (type == JSON_ARRAY)       children = toks[idx].size;
    else if (type == JSON_OBJECT) children = toks[idx].size * 2;
    else                          return idx;

    for (c = children; c > 0; c--) i = tok_last(toks, i + 1);
    return i;
}

int json_next(const JsonTok *toks, int idx)
{
    if (idx < 0) return -1;
    return tok_last(toks, idx) + 1;
}

int json_key(const char *s, const JsonTok *toks, int obj, const char *key)
{
    int    i, pair;
    size_t klen;
    if (obj < 0 || toks[obj].type != JSON_OBJECT) return -1;
    klen = strlen(key);
    i = obj + 1;
    for (pair = 0; pair < toks[obj].size; pair++) {
        // Dans un objet, la valeur suit immédiatement sa clé.
        if (toks[i].type == JSON_STRING &&
            (size_t)(toks[i].end - toks[i].start) == klen &&
            strncmp(s + toks[i].start, key, klen) == 0) {
            return i + 1;
        }
        i = json_next(toks, i + 1);
        if (i < 0) return -1;
    }
    return -1;
}

int json_at(const JsonTok *toks, int arr, int i)
{
    int idx;
    if (arr < 0 || toks[arr].type != JSON_ARRAY) return -1;
    if (i < 0 || i >= toks[arr].size) return -1;
    idx = arr + 1;
    while (i > 0) {
        idx = json_next(toks, idx);
        if (idx < 0) return -1;
        i--;
    }
    return idx;
}

int json_len(const JsonTok *toks, int arr)
{
    if (arr < 0 || toks[arr].type != JSON_ARRAY) return 0;
    return toks[arr].size;
}

bool json_is_str(const JsonTok *toks, int idx)
{
    return idx >= 0 && toks[idx].type == JSON_STRING;
}

bool json_is_object(const JsonTok *toks, int idx)
{
    return idx >= 0 && toks[idx].type == JSON_OBJECT;
}

void json_get_str(const char *s, const JsonTok *toks, int idx, char *out, size_t cap)
{
    const JsonTok *t;
    size_t o = 0;
    int i;

    if (cap == 0) return;
    out[0] = '\0';
    if (idx < 0) return;

    t = &toks[idx];
    if (t->type != JSON_STRING && t->type != JSON_PRIMITIVE) return;

    for (i = t->start; i < t->end && o + 1 < cap; i++) {
        char c = s[i];
        if (c != '\\') { out[o++] = c; continue; }
        i++;
        if (i >= t->end) break;
        switch (s[i]) {
            case 'n': out[o++] = '\n'; break;
            case 't': out[o++] = '\t'; break;
            case 'r': out[o++] = '\r'; break;
            case 'b': out[o++] = '\b'; break;
            case 'f': out[o++] = '\f'; break;
            case '/': out[o++] = '/'; break;
            case '\\': out[o++] = '\\'; break;
            case '"': out[o++] = '"'; break;
            case 'u': {
                unsigned cp = 0;
                int k;
                for (k = 0; k < 4 && i + 1 < t->end; k++) {
                    char h = s[++i];
                    cp <<= 4;
                    if (h >= '0' && h <= '9') cp |= (unsigned)(h - '0');
                    else if (h >= 'a' && h <= 'f') cp |= (unsigned)(h - 'a' + 10);
                    else if (h >= 'A' && h <= 'F') cp |= (unsigned)(h - 'A' + 10);
                }
                if (cp < 0x80) {
                    out[o++] = (char)cp;
                } else if (cp < 0x800) {
                    if (o + 2 < cap) {
                        out[o++] = (char)(0xC0 | (cp >> 6));
                        out[o++] = (char)(0x80 | (cp & 0x3F));
                    }
                } else {
                    if (o + 3 < cap) {
                        out[o++] = (char)(0xE0 | (cp >> 12));
                        out[o++] = (char)(0x80 | ((cp >> 6) & 0x3F));
                        out[o++] = (char)(0x80 | (cp & 0x3F));
                    }
                }
                break;
            }
            default: out[o++] = s[i]; break;
        }
    }
    out[o] = '\0';
}

long long json_get_int(const char *s, const JsonTok *toks, int idx, long long def)
{
    char buf[32];
    size_t n;
    if (idx < 0 || toks[idx].type != JSON_PRIMITIVE) return def;
    n = (size_t)(toks[idx].end - toks[idx].start);
    if (n == 0 || n >= sizeof(buf)) return def;
    memcpy(buf, s + toks[idx].start, n);
    buf[n] = '\0';
    return strtoll(buf, NULL, 10);
}
