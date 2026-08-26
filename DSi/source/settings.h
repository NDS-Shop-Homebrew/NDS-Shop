#pragma once
#include "colors.h"
#include "gettext.h"
#include <sys/syslimits.h>

typedef enum {
    SETTING_DLPATH,
    SETTING_DLUSEDIRS,
    SETTING_COLORSCHEME,
    SETTING_CHECKUPDATEONSTART,
    SETTING_LANG,
    SETTINGS_COUNT
} SettingEnum;

struct Settings {
    char dlPath[PATH_MAX];
    bool dlUseDirs;
    ColorSchemeEnum colorScheme;
    LanguageEnum lang;
    bool checkUpdateOnStart;
};

extern struct Settings settings;

#define dlUseDirsStr(x) \
    ((x) == false ? "No" \
    : (x) == true ? "Yes" \
    : "")

#define langStr(x) \
    ((x) == LANG_EN ? "English" \
	: (x) == LANG_IT ? "Italian" \
    : (x) == LANG_NL ? "Dutch" \
    : (x) == LANG_FR ? "French" \
	: "")

#define checkUpdateOnStartStr(x) \
    ((x) == false ? "No" \
    : (x) == true ? "Yes" \
    : "")

#define colorSchemeStr(x) \
    ((x) == COLOR_SCHEME_1 ? "NDS-Shop" \
    : (x) == COLOR_SCHEME_2 ? "Dark" \
    : (x) == COLOR_SCHEME_3 ? "Blue" \
    : (x) == COLOR_SCHEME_4 ? "Amber" \
    : (x) == COLOR_SCHEME_5 ? "Red" \
    : (x) == COLOR_SCHEME_6 ? "Green" \
    : (x) == COLOR_SCHEME_7 ? "Purple" \
    : "")

bool defaultSettings(void);
bool loadSettings(void);
bool saveSettings(void);
