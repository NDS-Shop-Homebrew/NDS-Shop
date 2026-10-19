#include "core/status.h"

#include <3ds.h>

#include "core/app.h"

static bool s_ptmu_ready = false;
static bool s_ac_ready   = false;

void status_init(void)
{
    s_ptmu_ready = R_SUCCEEDED(ptmuInit());
    s_ac_ready   = R_SUCCEEDED(acInit());
}

void status_exit(void)
{
    if (s_ac_ready) {
        acExit();
        s_ac_ready = false;
    }
    if (s_ptmu_ready) {
        ptmuExit();
        s_ptmu_ready = false;
    }
}

static bool wifi_connected(void)
{
    u32 status = 0;

    if (!s_ac_ready) return false;
    if (R_FAILED(ACU_GetStatus(&status))) return false;
    return (status & 0xFFFF) >= 3;
}

void status_refresh(void)
{
    u8 level = 0;
    u8 charging = 0;

    if (s_ptmu_ready && R_SUCCEEDED(PTMU_GetBatteryLevel(&level)) && level <= 5) {
        g_app.battery_percent = (int)level * 20;
    } else {
        g_app.battery_percent = -1;
    }
    if (s_ptmu_ready) {
        g_app.battery_charging =
            R_SUCCEEDED(PTMU_GetBatteryChargeState(&charging)) && charging != 0;
    }

    if (wifi_connected()) {
        g_app.wifi_bars = (int)osGetWifiStrength();
        if (g_app.wifi_bars > 3) g_app.wifi_bars = 3;
    } else {
        g_app.wifi_bars = -1;
    }
}
