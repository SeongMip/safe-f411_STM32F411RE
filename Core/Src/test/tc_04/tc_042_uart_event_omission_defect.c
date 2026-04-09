/****************************************************************************
 * @file    tc_042_uart_event_omission_defect.c
 * @brief   버튼 이벤트와 UART 로그 누락 가능성을 결함 관점에서 관찰한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
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


/**
 * @brief   TC-042의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
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
