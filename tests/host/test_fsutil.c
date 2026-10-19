// Tests hôtes : normalisation de l'adresse d'API (couche config, sans 3DS).
//
//   make -C tests/host
//
// L'adresse saisie dans les réglages est enregistrée telle quelle puis utilisée
// immédiatement : elle doit donc être normalisée au moment de la saisie, et pas
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
        printf("  ECHEC : \"%s\" -> \"%s\" (attendu \"%s\")\n", input, buf, want);
        failures++;
    } else {
        printf("  ok    : \"%s\" -> \"%s\"\n", input, buf);
    }
}

int main(void)
{
    printf("normalisation de l'adresse d'API :\n");

    check("https://db-nds-shop.fr", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/api", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr/api/", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr///", "https://db-nds-shop.fr");
    check("https://db-nds-shop.fr//api//", "https://db-nds-shop.fr");
    check("  https://db-nds-shop.fr  ", "https://db-nds-shop.fr");
    check("http://127.0.0.1:3000", "http://127.0.0.1:3000");
    check("http://127.0.0.1:3000/api", "http://127.0.0.1:3000");

    // Un chemin qui se termine autrement ne doit pas être tronqué.
    check("https://x.example/api/v2", "https://x.example/api/v2");
    check("https://x.example/apis", "https://x.example/apis");

    // Valeur vide après nettoyage : retour au défaut compilé.
    check("", APP_API_DEFAULT);
    check("///", APP_API_DEFAULT);

    if (failures == 0) {
        printf("\nOK : adresse d'API normalisée.\n");
        return 0;
    }
    printf("\n%d test(s) en échec.\n", failures);
    return 1;
}
