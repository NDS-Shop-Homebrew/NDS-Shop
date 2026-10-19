#include "tasks/cia.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"

#define CIA_CHUNK 0x8000u

static const char *archive_path(char *out, size_t cap, const char *sd_path)
{
    if (strncmp(sd_path, "sdmc:", 5) == 0) snprintf(out, cap, "%s", sd_path + 5);
    else                                    snprintf(out, cap, "%s", sd_path);
    return out;
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
    Result  rc;

    if (err != NULL && errcap > 0) err[0] = '\0';

    if (sd_path == NULL || sd_path[0] == '\0') {
        if (err != NULL) snprintf(err, errcap, "invalid .cia path");
        return false;
    }

    archive_path(path_buf, sizeof(path_buf), sd_path);

    rc = FSUSER_OpenFile(&file_handle, ARCHIVE_SDMC, fsMakePath(PATH_ASCII, path_buf),
                         FS_OPEN_READ, 0);
    if (R_FAILED(rc)) {
        if (err != NULL) snprintf(err, errcap, ".cia file not found (0x%08lX)", (unsigned long)rc);
        return false;
    }
    FSFILE_GetSize(file_handle, &total);

    rc = amInit();
    if (R_FAILED(rc)) {
        FSFILE_Close(file_handle);
        if (err != NULL) snprintf(err, errcap, "AM service unavailable (0x%08lX)", (unsigned long)rc);
        return false;
    }

    rc = AM_StartCiaInstall(MEDIATYPE_SD, &cia_handle);
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
