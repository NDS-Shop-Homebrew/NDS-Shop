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

#ifndef _NDS_SHOP_FILES_HPP
#define _NDS_SHOP_FILES_HPP

#include "common.hpp"

Result makeDirs(std::string path);
Result openFile(Handle *fileHandle, const char *path, bool write);
Result deleteFile(const char *path);
Result removeDir(const char *path);
Result removeDirRecursive(const char *path);
u64 getAvailableSpace();

/*
	Copy a file from source to destination.
	@return True if the copy succeeded; false otherwise.
*/
bool copyFile(const std::string &source, const std::string &destination);

/*
	Restore a file from its backup (path + ".bak").
	@return True if a backup existed and was restored; false otherwise.
*/
bool restoreBackup(const std::string &path);

#endif