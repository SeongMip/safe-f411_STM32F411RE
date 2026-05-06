/****************************************************************************
 * @file    tc_065_reset_recovery_observation.c
 * @brief   수정 이후 회귀, 스트레스, 안정성, 복구 특성을 검증한다.
 *
 * @details
 * - 기능 fix 이후 다른 경로가 깨지지 않았는지 확인한다.
 * - 장시간 동작, reset/power cycle, safe-state 관련 관찰을 포함할 수 있다.
 ****************************************************************************/

#include "tc_065_reset_recovery_observation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_065_MAX_BOOT_MS  100U
#define TC_065_VERIFY_MS   2000U
#define TC_065_HEARTBEAT_MS 200U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t last_toggle_ms;
    uint8_t boot_tick_ok;
    uint8_t led_toggle_ok;
} TC065_Context;

static void TC_065_Setup(TC065_Context* ctx, uint32_t now)
{
    PlatformLedState before;
    PlatformLedState after;

    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->last_toggle_ms = now;
    ctx->boot_tick_ok = (now <= TC_065_MAX_BOOT_MS) ? 1U : 0U;

    before = Platform_LedRead();
    Platform_LedToggle();
    after = Platform_LedRead();
    Platform_LedToggle();
    ctx->led_toggle_ok = (before != after) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_065 BOOT boot_tick_ok=%u led_toggle_ok=%u\r\n",
              (unsigned long)now,
              (unsigned int)ctx->boot_tick_ok,
              (unsigned int)ctx->led_toggle_ok);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_065 ACTION: Observe for 2 seconds, then press reset button. Repeat 5 times.\r\n",
              (unsigned long)now);
}

static void TC_065_Stimulus(TC065_Context* ctx, uint32_t now)
{
    if ((now - ctx->last_toggle_ms) < TC_065_HEARTBEAT_MS)
    {
        return;
    }

    ctx->last_toggle_ms = now;
    Platform_LedToggle();
}

static TestResult TC_065_Verify(TC065_Context* ctx, uint32_t now)
{
    uint8_t pass;

    if ((now - ctx->start_ms) < TC_065_VERIFY_MS)
    {
        return TEST_IN_REVIEW;
    }

    pass = (ctx->boot_tick_ok && ctx->led_toggle_ok) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_065 RESULT=%s boot_tick_ok=%u led_ok=%u\r\n",
              (unsigned long)now,
              pass ? "PASS" : "FAIL",
              (unsigned int)ctx->boot_tick_ok,
              (unsigned int)ctx->led_toggle_ok);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_065 INFO: Press reset button now. Repeat 5 times total.\r\n",
              (unsigned long)now);

    return pass ? TEST_PASS : TEST_FAIL;
}

TestResult TC_065_ResetRecoveryObservation_Run(void)
{
    static TC065_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_065_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_065_Stimulus(&ctx, now);
    return TC_065_Verify(&ctx, now);
}
