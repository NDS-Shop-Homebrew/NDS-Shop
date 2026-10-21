#include "tasks/cia.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"

#define CIA_CHUNK 0x8000u
#define CIA_HEADER_SIZE 0x20u

static const char *archive_path(char *out, size_t cap, const char *sd_path)
{
    if (strncmp(sd_path, "sdmc:", 5) == 0) snprintf(out, cap, "%s", sd_path + 5);
    else                                    snprintf(out, cap, "%s", sd_path);
    return out;
}

static u32 read_le32(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static u32 read_be32(const u8 *p)
{
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | (u32)p[3];
}

static u64 read_be64(const u8 *p)
{
    return ((u64)read_be32(p) << 32) | (u64)read_be32(p + 4);
}

static u64 align64(u64 value)
{
    return (value + 0x3Fu) & ~((u64)0x3Fu);
}

// Reads the TMD title id out of a CIA file. The archive stores its sections
// on 64-byte boundaries, so the TMD offset has to be realigned after each
// section. Returns false when the file does not look like a CIA.
static bool cia_title_id(Handle file, u64 *out_tid)
{
    u8  header[CIA_HEADER_SIZE];
    u8  tmd[0x400];
    u32 got = 0;
    u64 off;
    u32 header_size;
    u32 sig_type;
    u32 sig_data;

    if (R_FAILED(FSFILE_Read(file, &got, 0, header, sizeof(header))) || got != sizeof(header)) return false;

    header_size = read_le32(header + 0x00);
    if (header_size < CIA_HEADER_SIZE || header_size > 0x10000) return false;

    off = align64(header_size);
    off = align64(off + read_le32(header + 0x08)); // certificate chain
    off = align64(off + read_le32(header + 0x0C)); // ticket

    got = 0;
    if (R_FAILED(FSFILE_Read(file, &got, off, tmd, sizeof(tmd))) || got < 8) return false;

    sig_type = read_be32(tmd);
    switch (sig_type) {
        case 0x00010000:
        case 0x00010003: sig_data = align64(4 + 0x200); break;
        case 0x00010001:
        case 0x00010004: sig_data = align64(4 + 0x100); break;
        case 0x00010002:
        case 0x00010005: sig_data = align64(4 + 0x3C); break;
        default: return false;
    }

    if (sig_data + 0x4C + 8 > got) return false;
    *out_tid = read_be64(tmd + sig_data + 0x4C);
    return true;
}

// TWL (DS) titles, system titles and DLP children live on NAND; everything
// else goes to the SD card. AM rejects the install with 0xE0E083EE when the
// media type does not match the title, so this has to be right.
static FS_MediaType media_for_title(u64 tid)
{
    u16 category = (u16)((tid >> 32) & 0xFFFFu);

    return (category & (0x1u | 0x10u | 0x8000u)) ? MEDIATYPE_NAND : MEDIATYPE_SD;
}

bool cia_install(const char *sd_path, ProgressFn cb, void *user, char *err, size_t errcap)
{
    char    path_buf[1024];
    Handle  file_handle = 0;
    Handle  cia_handle = 0;
    u8     *buffer;
    u64     offset = 0;
    u64     total = 0;
    u32     bytes_read = 0;
    u64     title_id = 0;
    FS_MediaType media = MEDIATYPE_SD;
    Result  rc;

    if (err != NULL && errcap > 0) err[0] = '\0';

    if (sd_path == NULL || sd_path[0] == '\0') {
        if (err != NULL) snprintf(err, errcap, "invalid .cia path");
        return false;
    }

    archive_path(path_buf, sizeof(path_buf), sd_path);

    // ARCHIVE_SDMC is an archive *id*, so the file has to be opened directly:
    // FSUSER_OpenFile() expects a handle and would fail with 0xC8804465.
    rc = FSUSER_OpenFileDirectly(&file_handle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""),
                                 fsMakePath(PATH_ASCII, path_buf), FS_OPEN_READ, 0);
    if (R_FAILED(rc)) {
        if (err != NULL) snprintf(err, errcap, "cannot open .cia file (0x%08lX)", (unsigned long)rc);
        return false;
    }
    FSFILE_GetSize(file_handle, &total);

    rc = amInit();
    if (R_FAILED(rc)) {
        FSFILE_Close(file_handle);
        if (err != NULL) snprintf(err, errcap, "AM service unavailable (0x%08lX)", (unsigned long)rc);
        return false;
    }

    if (cia_title_id(file_handle, &title_id)) {
        AM_TitleEntry entry;

        media = media_for_title(title_id);

        // Reinstalling a title that is already present fails with "already
        // exists", so remove the previous copy first, like FBI does.
        if (R_SUCCEEDED(AM_GetTitleInfo(media, 1, &title_id, &entry))) {
            AM_DeleteTitle(media, title_id);
            AM_DeleteTicket(title_id);
            if (media == MEDIATYPE_SD) AM_QueryAvailableExternalTitleDatabase(NULL);
        }
    }

    rc = AM_StartCiaInstall(media, &cia_handle);
    if (R_FAILED(rc)) {
        amExit();
        FSFILE_Close(file_handle);
        if (err != NULL) snprintf(err, errcap, "install rejected (0x%08lX)", (unsigned long)rc);
        return false;
    }

    buffer = (u8 *)malloc(CIA_CHUNK);
    if (buffer == NULL) {
        AM_CancelCIAInstall(cia_handle);
        amExit();
        FSFILE_Close(file_handle);
        if (err != NULL) snprintf(err, errcap, "out of memory");
        return false;
    }

    for (;;) {
        rc = FSFILE_Read(file_handle, &bytes_read, offset, buffer, CIA_CHUNK);
        if (R_FAILED(rc) || bytes_read == 0) break;
        rc = FSFILE_Write(cia_handle, NULL, offset, buffer, bytes_read, FS_WRITE_FLUSH);
        if (R_FAILED(rc)) break;
        offset += bytes_read;
        if (cb != NULL) {
            float frac = (total > 0) ? (float)((double)offset / (double)total) : -1.0f;
            cb(NULL, frac, user);
        }
        if (g_app.cancel) break;
    }
    free(buffer);
    FSFILE_Close(file_handle);

    if (g_app.cancel) {
        AM_CancelCIAInstall(cia_handle);
        amExit();
        if (err != NULL) snprintf(err, errcap, "install cancelled");
        return false;
    }
    if (R_FAILED(rc)) {
        AM_CancelCIAInstall(cia_handle);
        amExit();
        if (err != NULL) snprintf(err, errcap, "title write failed (0x%08lX)", (unsigned long)rc);
        return false;
    }

    rc = AM_FinishCiaInstall(cia_handle);
    amExit();
    if (R_FAILED(rc)) {
        if (err != NULL) snprintf(err, errcap, "finalization failed (0x%08lX)", (unsigned long)rc);
        return false;
    }

    return true;
}
