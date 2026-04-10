/****************************************************************************
 * @file    tc_064_power_cycle_recovery_validation.c
 * @brief   수정 이후 회귀, 스트레스, 안정성, 복구 특성을 검증한다.
 *
 * @details
 * - 기능 fix 이후 다른 경로가 깨지지 않았는지 확인한다.
 * - 장시간 동작, reset/power cycle, safe-state 관련 관찰을 포함할 수 있다.
 ****************************************************************************/

#include "tc_064_power_cycle_recovery_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_064_HEARTBEAT_MS       200U
#define TC_064_VERIFY_DURATION_MS 3000U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t last_toggle_ms;
    uint32_t toggle_count;
} TC064_Context;

static void TC_064_Setup(TC064_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->last_toggle_ms = now;
    ctx->toggle_count = 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 BOOT_OK verify_duration=%u_ms\r\n",
              (unsigned long)now,
              (unsigned int)TC_064_VERIFY_DURATION_MS);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 ACTION: Observe LED heartbeat for 3 seconds, then power-cycle\r\n",
              (unsigned long)now);
}

static void TC_064_Stimulus(TC064_Context* ctx, uint32_t now)
{
    if ((now - ctx->last_toggle_ms) < TC_064_HEARTBEAT_MS)
    {
        return;
    }

    ctx->last_toggle_ms = now;
    ctx->toggle_count++;
    Platform_LedToggle();
}

static TestResult TC_064_Verify(TC064_Context* ctx, uint32_t now)
{
    uint32_t expected_toggles;
    uint8_t pass;

    if ((now - ctx->start_ms) < TC_064_VERIFY_DURATION_MS)
    {
        return TEST_IN_REVIEW;
    }

    expected_toggles = TC_064_VERIFY_DURATION_MS / TC_064_HEARTBEAT_MS;
    pass = (ctx->toggle_count >= (expected_toggles - 2U)) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 RESULT=%s toggles=%lu expected>=%lu\r\n",
              (unsigned long)now,
              pass ? "PASS" : "FAIL",
              (unsigned long)ctx->toggle_count,
              (unsigned long)(expected_toggles - 2U));
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 INFO: Power-cycle the board now. Repeat 10 times total.\r\n",
              (unsigned long)now);

    return pass ? TEST_PASS : TEST_FAIL;
}

TestResult TC_064_PowerCycleRecoveryValidation_Run(void)
{
    static TC064_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_064_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_064_Stimulus(&ctx, now);
    return TC_064_Verify(&ctx, now);
}
