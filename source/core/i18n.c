#include "core/i18n.h"

#include <string.h>

#include "core/app.h"

typedef struct {
    const char *key;
    const char *fr;
    const char *en;
} Entry;

static const Entry kEntries[] = {
    { "app.tagline",   "Nintendo DS game store", "Nintendo DS game store" },

    { "loading.catalog", "Loading catalog...", "Loading catalog..." },
    { "loading.icons",   "Loading icon atlas...",      "Loading icon atlas..." },
    { "loading.offline", "Offline mode (local copy)", "Offline mode (local copy)" },

    { "list.count",    "%d games",      "%d games" },
    { "list.empty",    "No game matches", "No game matches" },
    { "list.selected", "Selection",  "Selection" },
    { "list.none",     "No game selected", "No game selected" },

    { "hint.list",     "A: install   X: search   Y: settings   SELECT: help   START: quit",
                       "A: install   X: search   Y: settings   SELECT: help   START: quit" },
    { "hint.back",     "B: back", "B: back" },
    { "hint.quit",     "START: quit", "START: quit" },

    { "detail.version",  "Version",  "Version" },
    { "detail.author",   "Author",   "Author" },
    { "detail.size",     "Size",   "Size" },
    { "detail.system",   "Media",  "Media" },
    { "detail.source",   "Source",   "Source" },
    { "detail.roms",     "ROM available", "ROM available" },
    { "detail.norom",    "ROM not published", "ROM not published" },

    { "action.download", "Install (.cia forwarder)", "Install (.cia forwarder)" },
    { "action.help",     "How does it work?",    "How does it work?" },

    { "task.title",   "Installation", "Installation" },
    { "task.download", "Downloading...", "Downloading..." },
    { "task.install",  "Installing shortcut...", "Installing shortcut..." },
    { "task.cleanup",  "Cleaning up...", "Cleaning up..." },
    { "task.cancel",  "B: cancel",   "B: cancel" },
    { "task.done",    "Done",  "Done" },
    { "task.cancelled", "Installation cancelled", "Installation cancelled" },
    { "task.failed",  "Installation failed", "Installation failed" },

    { "settings.title",  "Settings", "Settings" },
    { "settings.api",    "API address", "API address" },
    { "settings.lang",   "Language", "Language" },
    { "settings.theme",  "Theme", "Theme" },
    { "settings.dark",   "Dark", "Dark" },
    { "settings.light",  "Light", "Light" },
    { "settings.save",   "A: save", "A: save" },
    { "settings.reset",  "X: reset", "X: reset" },
    { "settings.reload", "Y: reload catalog", "Y: reload catalog" },
    { "settings.hint",   "A: edit   B: back", "A: edit   B: back" },

    { "search.title",  "Search a game", "Search a game" },
    { "search.label",  "Search:", "Search:" },
    { "search.hint",   "X: search", "X: search" },

    { "help.title", "Help", "Help" },
    { "help.scroll", "L/R: scroll", "L/R: scroll" },
    { "help.body",
      "NDS-Shop installs your Nintendo DS games straight onto the console.\n\n"
      "1. Pick a game (A), then start the installation.\n"
      "2. The app downloads the ROM to the SD card (/roms/nds).\n"
      "3. It then installs a shortcut (.cia forwarder) that launches the game "
      "from the home menu.\n\n"
      "A shortcut appears on the home menu, just like an installed game. "
      "To uninstall a game, remove its shortcut from the console Settings.\n\n"
      "This application distributes Nintendo DS games only. For takedown "
      "requests, see the DMCA page on the website.",
      "NDS-Shop installs your Nintendo DS games straight onto the console.\n\n"
      "1. Pick a game (A), then start the installation.\n"
      "2. The app downloads the ROM to the SD card (/roms/nds).\n"
      "3. It then installs a shortcut (.cia forwarder) that launches the game "
      "from the home menu.\n\n"
      "A shortcut appears on the home menu, just like an installed game. "
      "To uninstall a game, remove its shortcut from the console Settings.\n\n"
      "This application distributes Nintendo DS games only. For takedown "
      "requests, see the DMCA page on the website." },

    { "error.title", "Error", "Error" },
    { "error.hint",  "A: retry   B: back", "A: retry   B: back" },
    { "common.yes",  "Yes", "Yes" },
    { "common.no",   "No", "No" },
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
