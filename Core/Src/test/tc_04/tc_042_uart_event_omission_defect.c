/****************************************************************************
 * @file    tc_042_uart_event_omission_defect.c
 * @brief   결함 재현을 위해 의도적으로 취약 조건을 관찰하거나 결함 시나리오를 분리한다.
 *
 * @details
 * - 기능 구현보다 재현성과 원인 분리를 목적으로 한다.
 ****************************************************************************/

#include "tc_042_uart_event_omission_defect.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_042_TARGET_CLICKS  10U
#define TC_042_SETTLE_MS    5000U
#define TC_042_TIMEOUT_MS  30000U
#define TC_042_LOG_BLOCK_MS   50U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t led_toggle_count;
    uint32_t uart_log_count;
    uint32_t last_event_ms;
    PlatformButtonRawState prev_raw;
} TC042_Context;

static void TC_042_Setup(TC042_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->led_toggle_count = 0U;
    ctx->uart_log_count = 0U;
    ctx->last_event_ms = 0U;
    ctx->prev_raw = Platform_ButtonReadRaw();

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_042 START bug=uart_omission\r\n",
              (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_042 ACTION: Click button %u times quickly\r\n",
              (unsigned long)now,
              (unsigned int)TC_042_TARGET_CLICKS);
}

static void TC_042_Stimulus(TC042_Context* ctx)
{
    PlatformButtonRawState raw = Platform_ButtonReadRaw();

    if ((raw == PLATFORM_BUTTON_DOWN) && (ctx->prev_raw == PLATFORM_BUTTON_UP))
    {
        ctx->led_toggle_count++;
        ctx->last_event_ms = Platform_NowMs();
        Platform_LedToggle();

        Platform_DelayMs(TC_042_LOG_BLOCK_MS);

        ctx->uart_log_count++;
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_042 LED_toggle=%lu UART_log=%lu\r\n",
                  (unsigned long)Platform_NowMs(),
                  (unsigned long)ctx->led_toggle_count,
                  (unsigned long)ctx->uart_log_count);
    }

    ctx->prev_raw = raw;
}

static TestResult TC_042_Verify(TC042_Context* ctx, uint32_t now)
{
    uint8_t mismatch;
    uint8_t count_off;

    if ((ctx->led_toggle_count > 0U) && ((now - ctx->last_event_ms) > TC_042_SETTLE_MS))
    {
        mismatch = (ctx->led_toggle_count != ctx->uart_log_count) ? 1U : 0U;
        count_off = ((ctx->led_toggle_count != TC_042_TARGET_CLICKS) ||
                     (ctx->uart_log_count != TC_042_TARGET_CLICKS)) ? 1U : 0U;

        if (mismatch || count_off)
        {
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_042 RESULT=PASS defect_reproduced LED=%lu UART=%lu target=%u\r\n",
                      (unsigned long)now,
                      (unsigned long)ctx->led_toggle_count,
                      (unsigned long)ctx->uart_log_count,
                      (unsigned int)TC_042_TARGET_CLICKS);
            return TEST_PASS;
        }

        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_042 RESULT=FAIL defect_not_reproduced LED=%lu UART=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->led_toggle_count,
                  (unsigned long)ctx->uart_log_count);
        return TEST_FAIL;
    }

    if ((now - ctx->start_ms) > TC_042_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_042 RESULT=FAIL reason=TIMEOUT LED=%lu UART=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->led_toggle_count,
                  (unsigned long)ctx->uart_log_count);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_042_UartEventOmissionDefect_Run(void)
{
    static TC042_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_042_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_042_Stimulus(&ctx);
    now = Platform_NowMs();
    return TC_042_Verify(&ctx, now);
}
