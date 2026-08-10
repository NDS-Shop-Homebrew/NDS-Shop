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

#include "animation.hpp"
#include "common.hpp"
#include "overlay.hpp"
#include "version.hpp"

/* Show the Credits. */
void Overlays::ShowCredits() {
	bool doOut = false;

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		GFX::DrawTop();
		GFX::DrawSprite(sprites_nds_shop_idx, 220, 26);
		Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), "NDS-Shop - " + Lang::get("CREDITS"), 395, 0, font);

		Gui::DrawString(10, 30, 0.5f, UIThemes->TextColor(), "- NDS-Shop Team :", 0, 0, font);
		Gui::DrawString(10, 50, 0.5f, UIThemes->TextColor(), "  - Rinzler", 0, 0, font);
		Gui::DrawString(10, 70, 0.5f, UIThemes->TextColor(), "  - CreeperTV", 0, 0, font);
		Gui::DrawString(10, 90, 0.5f, UIThemes->TextColor(), "- BenjaminCM", 0, 0, font);
		Gui::DrawString(10, 110, 0.5f, UIThemes->TextColor(), "- Ghost", 0, 0, font);
		Gui::DrawString(10, 197, 0.5f, UIThemes->TextColor(), Lang::get("GITHUB"), 390, 0, font);

		Gui::Draw_Rect(0, 215, 400, 25, UIThemes->BarColor());
		Gui::Draw_Rect(0, 214, 400, 1, UIThemes->BarOutline());
		Gui::DrawStringCentered(0, 218, 0.6f, UIThemes->TextColor(), Lang::get("CURRENT_VERSION") + std::string(VER_NUMBER), 390, 0, font);

		Animation::QueueEntryDone();
		GFX::DrawBottom();
		GFX::DrawSprite(sprites_nds_shop_idx, 0, 0);
		C3D_FrameEnd(0);

		hidScanInput();
		Animation::HandleQueueEntryDone();
		if ((hidKeysDown() & KEY_START) || (hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_A)) doOut = true;
	}
}