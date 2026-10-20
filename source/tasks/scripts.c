#include "tasks/scripts.h"
#include "tasks/cia.h"
#include "core/fsutil.h"
#include "core/i18n.h"
#include "net/net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/app.h"

struct Task {
    ProgressFn cb;
    void      *user;
    float      base;
    float      span;
    const char *label;
};

static const char *sd_of(char *out, size_t cap, const char *path)
{
    if (path[0] == '/') snprintf(out, cap, "sdmc:%s", path);
    else                snprintf(out, cap, "sdmc:/%s", path);
    return out;
}

static const char *basename_of(const char *path)
{
    const char *slash = strrchr(path, '/');
    return (slash != NULL) ? slash + 1 : path;
}

static void report(struct Task *t, float frac)
{
    if (t->cb == NULL) return;
    t->cb(t->label, t->base + (frac >= 0.0f ? frac : 0.0f) * t->span, t->user);
}

static void step_progress(const char *label, float frac, void *user)
{
    struct Task *t = (struct Task *)user;
    (void)label;
    if (t->cb != NULL) t->cb(t->label, t->base + (frac >= 0.0f ? frac : 0.0f) * t->span, t->user);
}

static bool ends_with_ci(const char *s, const char *suffix)
{
    size_t     ns = strlen(s);
    size_t     nf = strlen(suffix);
    size_t     i;
    if (ns < nf) return false;
    for (i = 0; i < nf; i++) {
        char a = s[ns - nf + i];
        char b = suffix[i];
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
        if (a != b) return false;
    }
    return true;
}

static bool is_optional(const char *output)
{
    return strncmp(output, "/_nds/", 5) == 0;
}

static bool run_step(const ScriptStep *s, struct Task *task, char *err, size_t errcap)
{
    char path[1400];

    if (strcmp(s->type, "downloadFile") == 0) {
        task->label = tr("task.download");
        if (!net_download(s->file, sd_of(path, sizeof(path), s->output), step_progress, task)) {
            if (is_optional(s->output)) return true;
            snprintf(err, errcap, "%s", net_error());
            return false;
        }
        return true;
    }

    if (strcmp(s->type, "installCia") == 0) {
        task->label = tr("task.install");
        g_app.download_speed = 0.0f;
        g_app.download_eta = 0.0f;
        report(task, 0.0f);
        if (!g_app.ask_forwarder) return true;   // user declined the HOME shortcut
        if (!cia_install(sd_of(path, sizeof(path), s->file), step_progress, task, err, errcap)) {
            return false;
        }
        return true;
    }

    if (strcmp(s->type, "deleteFile") == 0) {
        task->label = tr("task.cleanup");
        g_app.download_speed = 0.0f;
        g_app.download_eta = 0.0f;
        report(task, 0.5f);
        remove(sd_of(path, sizeof(path), s->file));
        return true;
    }

    return true;
}

static bool run_fallback(const Game *g, struct Task *task, AskForwarderFn ask_forwarder,
                         void *user, char *err, size_t errcap)
{
    const Download *rom = NULL;
    const Download *cia = NULL;
    char path[1400];
    int  i;

    for (i = 0; i < g->download_count; i++) {
        const char *name = g->downloads[i].filename;
        if (rom == NULL && ends_with_ci(name, ".nds")) rom = &g->downloads[i];
        if (cia == NULL && ends_with_ci(name, ".cia")) cia = &g->downloads[i];
    }

    if (rom == NULL) {
        snprintf(err, errcap, "%s", tr("detail.norom"));
        return false;
    }

    task->label = tr("task.download");
    task->base = 0.0f;
    task->span = (cia != NULL) ? 0.6f : 1.0f;
    snprintf(path, sizeof(path), APP_ROM_DIR "/%s", basename_of(rom->filename));
    if (!net_download(rom->url, path, step_progress, task)) {
        snprintf(err, errcap, "%s", net_error());
        return false;
    }

    if (cia != NULL) {
        g_app.ask_forwarder = (ask_forwarder != NULL) ? ask_forwarder(user) : true;
    }

    if (cia != NULL && g_app.ask_forwarder) {
        task->label = tr("task.install");
        task->base = 0.6f;
        task->span = 0.4f;
        snprintf(path, sizeof(path), "sdmc:/%s", basename_of(cia->filename));
        if (!net_download(cia->url, path, step_progress, task)) {
            snprintf(err, errcap, "%s", net_error());
            return false;
        }
        if (!cia_install(path, step_progress, task, err, errcap)) return false;
        remove(path);
    }

    return true;
}

bool scripts_run(const Game *g, ProgressFn cb, void *user, char *err, size_t errcap)
{
    return scripts_run_with_prompt(g, cb, NULL, user, err, errcap);
}

bool scripts_run_with_prompt(const Game *g, ProgressFn cb, AskForwarderFn ask_forwarder,
                             void *user, char *err, size_t errcap)
{
    struct Task task;
    bool has_forwarder = false;
    bool asked = false;
    int  i;

    if (err != NULL && errcap > 0) err[0] = '\0';

    task.cb = cb;
    task.user = user;
    task.base = 0.0f;
    task.span = 1.0f;
    task.label = tr("task.download");

    if (g->script_count == 0) return run_fallback(g, &task, ask_forwarder, user, err, errcap);

    for (i = 0; i < g->script_count; i++) {
        if (strcmp(g->scripts[i].type, "installCia") == 0) has_forwarder = true;
    }

    for (i = 0; i < g->script_count; i++) {
        const ScriptStep *step = &g->scripts[i];
        bool is_download = (strcmp(step->type, "downloadFile") == 0);
        bool is_rom = is_download && ends_with_ci(step->output, ".nds");
        bool is_forwarder = (strcmp(step->type, "installCia") == 0) ||
                            (is_download && ends_with_ci(step->output, ".cia")) ||
                            (strcmp(step->type, "deleteFile") == 0 && ends_with_ci(step->file, ".cia"));

        task.base = (float)i / (float)g->script_count;
        task.span = 1.0f / (float)g->script_count;

        if (g_app.cancel) {
            snprintf(err, errcap, "%s", tr("task.cancelled"));
            return false;
        }

        // The forwarder steps are silently skipped when the user declined them.
        if (is_forwarder && !g_app.ask_forwarder) continue;

        if (!run_step(step, &task, err, errcap)) return false;

        if (is_rom && has_forwarder && !asked) {
            asked = true;
            g_app.ask_forwarder = (ask_forwarder != NULL) ? ask_forwarder(user) : true;
        }
    }
    return true;
}
