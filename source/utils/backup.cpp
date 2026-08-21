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

#include "backup.hpp"
#include "common.hpp"
#include "files.hpp"
#include "meta.hpp"
#include "storeUtils.hpp"

void BackupSettings(bool force) {
	if (!force && !config->backup()) return;

	copyFile(_CONFIG_PATH, std::string(_CONFIG_PATH) + ".bak");
	copyFile(_META_PATH, std::string(_META_PATH) + ".bak");
}

bool RestoreSettings() {
	bool restored = false;

	if (restoreBackup(_CONFIG_PATH)) restored = true;
	if (restoreBackup(_META_PATH)) restored = true;

	if (restored) {
		/* Prevent the destructors from overwriting the restored files. */
		if (config) config->blockSave(true);
		if (StoreUtils::meta) StoreUtils::meta->blockSave(true);
	}

	return restored;
}
