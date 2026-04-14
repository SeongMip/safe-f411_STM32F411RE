/****************************************************************************
 * @file    tc_040_button_bounce_bug_injection.c
 * @brief   결함 재현을 위해 의도적으로 취약 조건을 관찰하거나 결함 시나리오를 분리한다.
 *
 * @details
 * - 기능 구현보다 재현성과 원인 분리를 목적으로 한다.
 ****************************************************************************/

#include "tc_040_button_bounce_bug_injection.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_040_TIMEOUT_MS  10000U
#define TC_040_SETTLE_MS    3000U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t event_count;
    uint32_t last_event_ms;
} TC040_Context;

static void TC_040_Setup(TC040_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->event_count = 0U;
    ctx->last_event_ms = 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_040 START bug=no_debounce_raw_polling\r\n",
              (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_040 ACTION: Press button once shortly\r\n",
              (unsigned long)now);
}

static void TC_040_Stimulus(TC040_Context* ctx, uint32_t now)
{
    if (Platform_ButtonReadRaw() != PLATFORM_BUTTON_DOWN)
    {
        return;
    }

    ctx->event_count++;
    ctx->last_event_ms = now;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_040 EVT=PRESS LED=TOGGLE count=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->event_count);
}

static TestResult TC_040_Verify(TC040_Context* ctx, uint32_t now)
{
    if ((ctx->event_count > 0U) && ((now - ctx->last_event_ms) > TC_040_SETTLE_MS))
    {
        if (ctx->event_count > 1U)
        {
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_040 RESULT=PASS reproduction_success count=%lu\r\n",
                      (unsigned long)now,
                      (unsigned long)ctx->event_count);
            return TEST_PASS;
        }

        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_040 RESULT=FAIL defect_not_reproduced count=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->event_count);
        return TEST_FAIL;
    }

    if ((now - ctx->start_ms) > TC_040_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_040 RESULT=FAIL reason=TIMEOUT count=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->event_count);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_040_ButtonBounceBugInjection_Run(void)
{
    static TC040_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_040_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    if ((now - ctx.start_ms) < TC_040_TIMEOUT_MS)
    {
        TC_040_Stimulus(&ctx, now);
    }

    return TC_040_Verify(&ctx, now);
}
