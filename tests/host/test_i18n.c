/* Host test: language selection drives the strings returned by tr(). */
#include <stdio.h>
#include <string.h>

#include "core/app.h"
#include "core/i18n.h"

AppState g_app;

static int failures;

static void check(const char *label, const char *got, const char *want)
{
    if (strcmp(got, want) != 0) {
        printf("  FAIL: %s -> \"%s\" (want \"%s\")\n", label, got, want);
        failures++;
    } else {
        printf("  ok  : %s -> \"%s\"\n", label, got);
    }
}

int main(void)
{
    memset(&g_app, 0, sizeof(g_app));

    snprintf(g_app.config.lang, sizeof(g_app.config.lang), "%s", "en");
    printf("== English ==\n");
    check("settings.lang", tr("settings.lang"), "Language");
    check("hint.back", tr("hint.back"), "B: back");

    snprintf(g_app.config.lang, sizeof(g_app.config.lang), "%s", "fr");
    printf("== French ==\n");
    check("settings.lang", tr("settings.lang"), "Langue");
    check("hint.back", tr("hint.back"), "B: retour");
    check("settings.title", tr("settings.title"), "R\u00e9glages");

    printf("== Unknown key ==\n");
    check("missing.key", tr("missing.key"), "missing.key");

    if (failures != 0) {
        printf("FAILED: %d\n", failures);
        return 1;
    }
    printf("OK: translations switch with the config language.\n");
    return 0;
}
