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
*   NDS-Shop Team reserves the right to update the license terms
*	at any time without prior notice.
*   Any changes to the code must be clearly marked as such to avoid confusion.
*/

#ifndef _NDS_SHOP_GFX_HPP
#define _NDS_SHOP_GFX_HPP

#include "common.hpp"
#include "sprites.h"
#include "theme.hpp"
#include <citro2d.h>
#include <string>

extern std::unique_ptr<Theme> UIThemes;

/* Standard Colors. */
#define WHITE C2D_Color32(255, 255, 255, 255)
#define BLACK C2D_Color32(0, 0, 0, 255)
#define TRANSPARENT C2D_Color32(0, 0, 0, 0)
#define DIM_COLOR C2D_Color32(0, 0, 0, 190)

namespace GFX {
	void DrawTop(void);
	void DrawBottom();
	void DrawSprite(int img, int x, int y, float ScaleX = 1, float ScaleY = 1);
	void DrawBox(float xPos, float yPos, float width = 50, float height = 50, bool selected = false, uint32_t clr = UIThemes->BoxInside());
	void DrawCheckbox(float xPos, float yPos, bool selected);
	void DrawToggle(float xPos, float yPos, bool toggled);
	void DrawTime();
	void DrawBattery();
	void HandleBattery();
	void DrawWifi();
	void DrawIcon(const int Idx, int X, int Y, uint32_t Color = UIThemes->SideBarIconColor(), float BlendPower = 1.0f, float ScaleX = 1, float ScaleY = 1);
};

#endif