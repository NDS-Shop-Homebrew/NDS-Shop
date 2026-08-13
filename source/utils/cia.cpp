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

#include "cia.hpp"
#include "files.hpp"

#include <cstdarg>
#include <cstdio>

/* Write a line to sdmc:/nds-dl.log for remote debugging. */
static void dbgLog(const char *fmt, ...) {
	FILE *f = fopen("sdmc:/nds-dl.log", "a");
	if (!f) return;
	fprintf(f, "[INSTALL] ");
	va_list args;
	va_start(args, fmt);
	vfprintf(f, fmt, args);
	va_end(args);
	fprintf(f, "\n");
	fclose(f);
}

Result Title::Launch(u64 titleId, FS_MediaType mediaType) {
	Result ret = 0;
	u8 param[0x300];
	u8 hmac[0x20];

	if (R_FAILED(ret = APT_PrepareToDoApplicationJump(0, titleId, mediaType))) {
		printf("Error In:\nAPT_PrepareToDoApplicationJump");
		return ret;
	}

	if (R_FAILED(ret = APT_DoApplicationJump(param, sizeof(param), hmac))) {
		printf("Error In:\nAPT_DoApplicationJump");
		return ret;
	}

	return 0;
}

Result Title::DeletePrevious(u64 titleid, FS_MediaType media) {
	Result ret = 0;
	u32 titles_amount = 0;

	ret = AM_GetTitleCount(media, &titles_amount);
	if (R_FAILED(ret)) {
		printf("Error in:\nAM_GetTitleCount\n");
		return ret;
	}

	u32 read_titles = 0;
	u64 *titleIDs = (u64 *)malloc(titles_amount * sizeof(u64));

	ret = AM_GetTitleList(&read_titles, media, titles_amount, titleIDs);
	if (R_FAILED(ret)) {
		free(titleIDs);
		printf("Error in:\nAM_GetTitleList\n");
		return ret;
	}

	for (u32 i = 0; i < read_titles; i++) {
		if (titleIDs[i] == titleid) {
			ret = AM_DeleteTitle(media, titleid);
			break;
		}
	}

	free(titleIDs);

	if (R_FAILED(ret)) {
		printf("Error in:\nAM_DeleteTitle\n");
		return ret;
	}

	return 0;
}

static FS_MediaType getTitleDestination(u64 titleId) {
	u16 platform = (u16) ((titleId >> 48) & 0xFFFF);
	u16 category = (u16) ((titleId >> 32) & 0xFFFF);
	u8 variation = (u8) (titleId & 0xFF);

	//     DSiWare                3DS                    DSiWare, System, DLP         Application           System Title
	return platform == 0x0003 || (platform == 0x0004 && ((category & 0x8011) != 0 || (category == 0x0000 && variation == 0x02))) ? MEDIATYPE_NAND : MEDIATYPE_SD;
}

u32 installSize = 0, installOffset = 0;

Result Title::Install(const char *ciaPath, bool updatingSelf) {
	u32 bytes_read = 0, bytes_written;
	installSize = 0, installOffset = 0; u64 size = 0;
	Handle ciaHandle, fileHandle;
	AM_TitleEntry info;
	Result ret = 0;
	FS_MediaType media = MEDIATYPE_SD;

	dbgLog("Install: %s (updatingSelf=%d)", ciaPath, updatingSelf);

	ret = openFile(&fileHandle, ciaPath, false);
	if (R_FAILED(ret)) {
		dbgLog("openFile failed: %08lX", (unsigned long)ret);
		printf("Error in:\nopenFile\n");
		return ret;
	}

	ret = AM_GetCiaFileInfo(media, &info, fileHandle);
	if (R_FAILED(ret)) {
		dbgLog("AM_GetCiaFileInfo failed: %08lX", (unsigned long)ret);
		printf("Error in:\nAM_GetCiaFileInfo\n");
		return ret;
	}
	dbgLog("titleID: %016llX", (unsigned long long)info.titleID);

	media = getTitleDestination(info.titleID);

	/* ponytail: comme Universal-Updater, on supprime TOUJOURS le titre
	 * precedent avant d'installer, meme pour l'auto-update.
	 * Le Launch auto (updatingSelf) est retire : l'app quitte et le user
	 * relance manuellement, c'est plus fiable sur 3DS. */
	(void)updatingSelf;
	dbgLog("DeletePrevious media=%d", media);
	ret = Title::DeletePrevious(info.titleID, media);
	if (R_FAILED(ret)) {
		dbgLog("DeletePrevious failed: %08lX", (unsigned long)ret);
		return ret;
	}

	ret = FSFILE_GetSize(fileHandle, &size);
	if (R_FAILED(ret)) {
		dbgLog("FSFILE_GetSize failed: %08lX", (unsigned long)ret);
		printf("Error in:\nFSFILE_GetSize\n");
		FSFILE_Close(fileHandle);
		return ret;
	}
	dbgLog("cia size: %llu, space: %llu", (unsigned long long)size, (unsigned long long)getAvailableSpace());

	if (getAvailableSpace() >= size) {
		dbgLog("AM_StartCiaInstall media=%d", media);
		ret = AM_StartCiaInstall(media, &ciaHandle);
		if (R_FAILED(ret)) {
			dbgLog("AM_StartCiaInstall failed: %08lX", (unsigned long)ret);
			printf("Error in:\nAM_StartCiaInstall\n");
			FSFILE_Close(fileHandle);
			return ret;
		}

		u32 toRead = 0x200000;
		u8 *buf = new u8[toRead];

		if (!buf) {
			FSFILE_Close(fileHandle);
			return -1;
		}

		installSize = size;
		do {
			FSFILE_Read(fileHandle, &bytes_read, installOffset, buf, toRead);
			FSFILE_Write(ciaHandle, &bytes_written, installOffset, buf, toRead, FS_WRITE_FLUSH);
			installOffset += bytes_read;
		} while(installOffset < installSize);
		delete[] buf;

		dbgLog("AM_FinishCiaInstall");
		ret = AM_FinishCiaInstall(ciaHandle);
		if (R_FAILED(ret)) {
			dbgLog("AM_FinishCiaInstall failed: %08lX", (unsigned long)ret);
			printf("Error in:\nAM_FinishCiaInstall\n");
			FSFILE_Close(fileHandle);
			return ret;
		}
	}

	ret = FSFILE_Close(fileHandle);
	if (R_FAILED(ret)) {
		dbgLog("FSFILE_Close failed: %08lX", (unsigned long)ret);
		printf("Error in:\nFSFILE_Close\n");
		return ret;
	}

	dbgLog("Install OK");
	return 0;
}