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

#ifndef _NDS_SHOP_EXTRACT_HPP
#define _NDS_SHOP_EXTRACT_HPP

#include "common.hpp"

enum ExtractError {
	EXTRACT_ERROR_NONE = 0,
	EXTRACT_ERROR_ARCHIVE,
	EXTRACT_ERROR_ALLOC,
	EXTRACT_ERROR_FIND,
	EXTRACT_ERROR_READFILE,
	EXTRACT_ERROR_OPENFILE,
	EXTRACT_ERROR_WRITEFILE,
};

Result getExtractedSize(const std::string &archivePath, const std::string &wantedFile);

Result extractArchive(const std::string &archivePath, const std::string &wantedFile, const std::string &outputPath);

#endif