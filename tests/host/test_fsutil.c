// Host tests: API address normalization (config layer, no 3DS).
//
//   make -C tests/host
//
// The address typed in settings is stored as-is and then used
// immediately: it must be normalized at input time, not
// seulement au chargement du fichier de configuration.
#include <stdio.h>
#include <string.h>

#include "core/app.h"
#include "core/fsutil.h"

AppState g_app;

static int failures = 0;

static void check(const char *input, const char *want)
{
    char buf[512];

    snprintf(buf, sizeof(buf), "%s", input);
    config_normalize_api(buf, sizeof(buf));
    if (strcmp(buf, want) != 0) {
        printf("  FAIL: \"%s\" -> \"%s\" (attendu \"%s\")\n", input, buf, want);
        failures++;
    } else {
        printf("  ok    : \"%s\" -> \"%s\"\n", input, buf);
    }
}

int main(void)
{
    printf("API address normalization:\n");

    check("https://db-nds-shop.fr", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/api", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/api/", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr///", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr//api//", "https://db-nds-shop.fr");
    check("  https://db-nds-shop.fr  ", "https://db-nds-shop.fr");
    check("http://127.0.0.1:3000", "http://127.0.0.1:3000");
    check("http://127.0.0.1:3000/api", "http://127.0.0.1:3000");

    // A path ending differently must not be truncated.
    check("https://x.example/api/v2", "https://x.example/api/v2");
    check("https://x.example/apis", "https://x.example/apis");

    // Empty value after cleanup: fall back to the compiled default.
    check("", APP_API_DEFAULT);
    check("///", APP_API_DEFAULT);

    if (failures == 0) {
        printf("\nOK: API address normalized.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
