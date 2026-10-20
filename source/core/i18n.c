#include "core/i18n.h"

#include <string.h>

#include "core/app.h"

typedef struct {
    const char *key;
    const char *fr;
    const char *en;
} Entry;

static const Entry kEntries[] = {
    { "app.tagline",   "Boutique de jeux Nintendo DS", "Nintendo DS game store" },

    { "loading.catalog", "Chargement du catalogue\u2026", "Loading catalog\u2026" },
    { "loading.icons",   "Atlas d'ic\u00f4nes\u2026",      "Loading icon atlas\u2026" },
    { "loading.offline", "Mode hors-ligne (copie locale)", "Offline mode (local copy)" },

    { "list.count",    "%d jeux",      "%d games" },
    { "list.empty",    "Aucun jeu ne correspond \u00e0 la recherche", "No game matches the search" },
    { "list.selected", "S\u00e9lection",  "Selection" },
    { "list.none",     "Aucun jeu s\u00e9lectionn\u00e9", "No game selected" },

    { "hint.list",     "A: installer   X: rechercher   Y: r\u00e9glages   SELECT: aide   START: quitter",
                       "A: install   X: search   Y: settings   SELECT: help   START: quit" },
    { "hint.back",     "B: retour", "B: back" },
    { "hint.quit",     "START: quitter", "START: quit" },

    { "detail.version",  "Version",  "Version" },
    { "detail.author",   "Auteur",   "Author" },
    { "detail.size",     "Taille",   "Size" },
    { "detail.system",   "Support",  "Media" },
    { "detail.source",   "Source",   "Source" },
    { "detail.roms",     "ROM disponible", "ROM available" },
    { "detail.norom",    "ROM non publi\u00e9e", "ROM not published" },
    { "detail.nodesc",   "Pas de description", "No description" },

    { "action.download", "T\u00e9l\u00e9charger le jeu", "Download game" },
    { "action.help",     "Comment \u00e7a marche ?",    "How does it work?" },

    { "task.title",   "Installation", "Installation" },
    { "task.download", "T\u00e9l\u00e9chargement\u2026", "Downloading\u2026" },
    { "task.install",  "Installation du raccourci\u2026", "Installing shortcut\u2026" },
    { "task.cleanup",  "Nettoyage\u2026", "Cleaning up\u2026" },
    { "task.cancel",  "B: annuler",   "B: cancel" },
    { "task.done",    "Termin\u00e9",  "Done" },
    { "task.cancelled", "Installation annul\u00e9e", "Installation cancelled" },
    { "task.failed",  "\u00c9chec de l'installation", "Installation failed" },
    { "task.ask_forwarder", "Raccourci dans le menu HOME ?", "Add a HOME menu shortcut?" },
    { "task.speed", "%s  \u00b7  reste %s", "%s  \u00b7  ETA %s" },
    { "confirm.hint", "\u2190/\u2192   A: OK   B: non", "\u2190/\u2192   A: OK   B: no" },

    { "settings.title",  "R\u00e9glages", "Settings" },
    { "settings.api",    "Adresse d'API", "API address" },
    { "settings.lang",   "Langue", "Language" },
    { "lang.fr",         "Fran\u00e7ais", "French" },
    { "lang.en",         "English", "Anglais" },
    { "settings.save",   "A: enregistrer", "A: save" },
    { "settings.reset",  "X: r\u00e9initialiser", "X: reset" },
    { "settings.reload", "Y: recharger le catalogue", "Y: reload catalog" },
    { "settings.hint",   "A: modifier   B: retour", "A: edit   B: back" },

    { "search.title",  "Rechercher un jeu", "Search a game" },
    { "search.label",  "Recherche :", "Search:" },
    { "search.hint",   "X: rechercher", "X: search" },

    { "help.title", "Aide", "Help" },
    { "help.scroll", "L/R: d\u00e9filer", "L/R: scroll" },
    { "help.body",
      "NDS-Shop installe vos jeux Nintendo DS directement sur la console.\n\n"
      "1. Choisissez un jeu (A), puis lancez le t\u00e9l\u00e9chargement.\n"
      "2. L'application t\u00e9l\u00e9charge la ROM sur la carte SD (/roms/nds).\n"
      "3. Elle vous propose ensuite d'installer un raccourci (forwarder .cia) "
      "qui lance le jeu depuis le menu HOME.\n\n"
      "Un raccourci appara\u00eet dans le menu HOME, comme un jeu install\u00e9. "
      "Pour d\u00e9sinstaller un jeu, retirez son raccourci depuis les Param\u00e8tres "
      "de la console.\n\n"
      "Cette application ne distribue que des jeux Nintendo DS. Pour les "
      "demandes de retrait, voir la page DMCA du site.",
      "NDS-Shop installs your Nintendo DS games straight onto the console.\n\n"
      "1. Pick a game (A), then start the download.\n"
      "2. The app downloads the ROM to the SD card (/roms/nds).\n"
      "3. It then offers to install a shortcut (.cia forwarder) that launches "
      "the game from the home menu.\n\n"
      "A shortcut appears on the home menu, just like an installed game. "
      "To uninstall a game, remove its shortcut from the console Settings.\n\n"
      "This application distributes Nintendo DS games only. For takedown "
      "requests, see the DMCA page on the website." },

    { "error.title", "Erreur", "Error" },
    { "error.hint",  "A: r\u00e9essayer   B: retour", "A: retry   B: back" },
    { "common.yes",  "Oui", "Yes" },
    { "common.no",   "Non", "No" },
};

const char *tr(const char *key)
{
    size_t i;
    bool   english = (strncmp(g_app.config.lang, "en", 2) == 0);

    for (i = 0; i < sizeof(kEntries) / sizeof(kEntries[0]); i++) {
        if (strcmp(kEntries[i].key, key) == 0) {
            return english ? kEntries[i].en : kEntries[i].fr;
        }
    }
    return key;
}
