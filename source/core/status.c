#include "core/status.h"

#include <3ds.h>

#include "core/app.h"

static bool s_ptmu_ready = false;

void status_init(void)
{
    s_ptmu_ready = R_SUCCEEDED(ptmuInit());
}

void status_exit(void)
{
    if (s_ptmu_ready) {
        ptmuExit();
        s_ptmu_ready = false;
    }
}

void status_refresh(void)
{
    u8 level = 0;

    if (s_ptmu_ready && R_SUCCEEDED(PTMU_GetBatteryLevel(&level)) && level <= 5) {
        g_app.battery_percent = (int)level * 20;
    } else {
        g_app.battery_percent = -1;
    }

    g_app.wifi_strength = (int)osGetWifiStrength() * 33;
    if (g_app.wifi_strength > 100) g_app.wifi_strength = 100;
}
