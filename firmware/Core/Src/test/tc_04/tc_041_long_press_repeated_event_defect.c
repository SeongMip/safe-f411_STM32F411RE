/****************************************************************************
 * @file    tc_041_long_press_repeated_event_defect.c
 * @brief   long-press 중 반복 토글 결함 재현 과정을 단계별 로그로 분리한다.
 *
 * @details
 * - 사용자가 hold 진행 상태와 release 이후 대기 이유를 이해할 수 있게 한다.
 * - toggle count 자체보다 결함 관찰 여부를 명확한 상태 로그로 남긴다.
 * - 2초 미만 release는 즉시 FAIL로 처리하지 않고 최종 settle 이후 판정한다.
 ****************************************************************************/

#include "tc_041_long_press_repeated_event_defect.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_041_HOLD_TARGET_MS         2000U
#define TC_041_HOLD_LOG_PERIOD_MS      500U
#define TC_041_SETTLE_MS              3000U
#define TC_041_TIMEOUT_MS            15000U
#define TC_041_SETTLE_LOG_PERIOD_MS   1000U

typedef struct
{
    uint8_t inited;
    uint8_t hold_started;
    uint8_t hold_target_logged;
    uint8_t release_logged;
    uint8_t defect_observed_logged;
    uint8_t defect_observed;
    PlatformButtonRawState prev_raw;
    uint32_t start_ms;
    uint32_t hold_start_ms;
    uint32_t last_hold_log_ms;
    uint32_t toggle_count;
    uint32_t release_ms;
    uint32_t last_settle_log_ms;
} TC041Context;

static void TC_041_Setup(TC041Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->hold_started = 0U;
    ctx->hold_target_logged = 0U;
    ctx->release_logged = 0U;
    ctx->defect_observed_logged = 0U;
    ctx->defect_observed = 0U;
    ctx->prev_raw = Platform_ButtonReadRaw();
    ctx->start_ms = now;
    ctx->hold_start_ms = 0U;
    ctx->last_hold_log_ms = 0U;
    ctx->toggle_count = 0U;
    ctx->release_ms = 0U;
    ctx->last_settle_log_ms = 0U;

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_041 START bug=no_release_detection\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_041 ACTION: Hold button for about 2 seconds, then release and wait for result\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_041 EXPECT: repeated toggles during a single hold indicate defect reproduction\r\n",
               (unsigned long)now);
}

static void TC_041_LogHoldProgress(TC041Context* ctx, uint32_t now)
{
    uint32_t elapsed_ms;

    if (!ctx->hold_started || ctx->release_logged)
    {
        return;
    }

    elapsed_ms = now - ctx->hold_start_ms;

    if ((!ctx->hold_target_logged) && (elapsed_ms >= TC_041_HOLD_TARGET_MS))
    {
        ctx->hold_target_logged = 1U;
        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_041 HOLD_TARGET_REACHED elapsed_ms=%lu release_when_ready=1\r\n",
                   (unsigned long)now,
                   (unsigned long)elapsed_ms);
        return;
    }

    if ((now - ctx->last_hold_log_ms) < TC_041_HOLD_LOG_PERIOD_MS)
    {
        return;
    }

    ctx->last_hold_log_ms = now;

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_041 HOLDING elapsed_ms=%lu target_ms=%u\r\n",
               (unsigned long)now,
               (unsigned long)elapsed_ms,
               (unsigned int)TC_041_HOLD_TARGET_MS);
}

static void TC_041_ObserveDefect(TC041Context* ctx, uint32_t now)
{
    if (ctx->defect_observed_logged)
    {
        return;
    }

    /* 단일 hold 동안 두 번째 이상 toggle이 발생하면 반복 토글 결함이 관찰된 것으로 본다. */
    if (ctx->toggle_count > 1U)
    {
        ctx->defect_observed = 1U;
        ctx->defect_observed_logged = 1U;

        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_041 DEFECT_OBSERVED repeated_toggle_during_single_hold\r\n",
                   (unsigned long)now);
    }
}

static void TC_041_Stimulus(TC041Context* ctx, uint32_t now)
{
    PlatformButtonRawState raw = Platform_ButtonReadRaw();
    uint32_t hold_elapsed_ms = 0U;

    if (raw == PLATFORM_BUTTON_DOWN)
    {
        if (!ctx->hold_started)
        {
            ctx->hold_started = 1U;
            ctx->hold_start_ms = now;
            ctx->last_hold_log_ms = now;

            Log_Printf(LOG_LEVEL_INFO,
                       "[ms=%lu] TC_041 HOLD_START target_ms=%u\r\n",
                       (unsigned long)now,
                       (unsigned int)TC_041_HOLD_TARGET_MS);
        }

        ctx->toggle_count++;
        Platform_LedToggle();

        TC_041_ObserveDefect(ctx, now);
        TC_041_LogHoldProgress(ctx, now);
    }
    else if (ctx->hold_started &&
             (!ctx->release_logged) &&
             (ctx->prev_raw == PLATFORM_BUTTON_DOWN))
    {
        ctx->release_logged = 1U;
        ctx->release_ms = now;
        ctx->last_settle_log_ms = now;
        hold_elapsed_ms = now - ctx->hold_start_ms;

        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_041 RELEASE_DETECTED hold_ms=%lu hold_target_reached=%u wait_settle_ms=%u\r\n",
                   (unsigned long)now,
                   (unsigned long)hold_elapsed_ms,
                   (unsigned int)ctx->hold_target_logged,
                   (unsigned int)TC_041_SETTLE_MS);
    }

    ctx->prev_raw = raw;
}

static void TC_041_LogSettleProgress(TC041Context* ctx, uint32_t now)
{
    uint32_t elapsed_ms;
    uint32_t remaining_ms;

    if (!ctx->release_logged)
    {
        return;
    }

    if ((now - ctx->last_settle_log_ms) < TC_041_SETTLE_LOG_PERIOD_MS)
    {
        return;
    }

    elapsed_ms = now - ctx->release_ms;
    if (elapsed_ms >= TC_041_SETTLE_MS)
    {
        return;
    }

    remaining_ms = TC_041_SETTLE_MS - elapsed_ms;
    ctx->last_settle_log_ms = now;

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_041 SETTLING remaining_ms=%lu\r\n",
               (unsigned long)now,
               (unsigned long)remaining_ms);
}

static TestResult TC_041_Verify(TC041Context* ctx, uint32_t now)
{
    if (ctx->release_logged)
    {
        TC_041_LogSettleProgress(ctx, now);

        if ((now - ctx->release_ms) > TC_041_SETTLE_MS)
        {
            if (ctx->defect_observed)
            {
                Log_Printf(LOG_LEVEL_INFO,
                           "[ms=%lu] TC_041 RESULT=PASS reproduction_success\r\n",
                           (unsigned long)now);
                return TEST_PASS;
            }

            Log_Printf(LOG_LEVEL_WARN,
                       "[ms=%lu] TC_041 RESULT=FAIL defect_not_reproduced\r\n",
                       (unsigned long)now);
            return TEST_FAIL;
        }
    }

    if ((now - ctx->start_ms) > TC_041_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_041 RESULT=FAIL reason=TIMEOUT\r\n",
                   (unsigned long)now);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_041_LongPressRepeatedEventDefect_Run(void)
{
    static TC041Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_041_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_041_Stimulus(&ctx, now);
    return TC_041_Verify(&ctx, now);
}
