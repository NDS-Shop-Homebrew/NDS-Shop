/*
*   This file is part of NDS-Shop Project
*   Copyright (C) 2024-2025 NDS-Shop Team
*   
*   This program is a free open-source software that allows users
*	to browse and download digital products.  It is based on the
*	code of the Universal-Updater project from Universal-Team.
*   
*   It's distributed under the terms of the GNU General Public
*	License and it's completely free to use and modify.
*   
*   This program comes with no warranty, but we are constantly
*	working on improving its functionality and user experience.
*   
*   If you have any suggestions or find any bugs, please let us know!
*   
*   Any changes to the code must be clearly marked as such to avoid confusion.
*/

#ifndef _NDS_SHOP_COMMON_HPP
#define _NDS_SHOP_COMMON_HPP

#include "config.hpp"
#include "gfx.hpp"
#include "lang.hpp"
#include "msg.hpp"
#include "screenCommon.hpp"
#include <3ds.h>
#include <vector>

#define _STORE_PATH "sdmc:/3ds/NDS-Shop/stores/"
#define _META_PATH "sdmc:/3ds/NDS-Shop/MetaData.json"
#define _THEME_PATH "sdmc:/3ds/NDS-Shop/Themes.json"
#define _CONFIG_PATH "sdmc:/3ds/NDS-Shop/Config.json"
#define _THEME_AMOUNT 2
#define _UNISTORE_VERSION 4

inline std::unique_ptr<Config> config;
inline uint32_t hRepeat, hDown, hHeld;
inline touchPosition touch;
inline C2D_Font font;

#endif