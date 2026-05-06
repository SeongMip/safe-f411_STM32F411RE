/****************************************************************************
 * @file    tc_042_uart_event_omission_defect.c
 * @brief   여러 번의 가벼운 터치에서 LED 반응 횟수와 UART 이벤트 횟수 불일치를 재현한다.
 *
 * @details
 * - LED는 raw touch가 감지되는 즉시 반응시켜 사용자가 입력을 바로 확인할 수 있게 한다.
 * - UART는 충분히 안정된 눌림일 때만 event로 기록해 짧은 터치에서는 누락될 수 있게 한다.
 * - 최종적으로 LED 반응 횟수와 UART 이벤트 횟수를 비교해 symptom 재현 여부를 판정한다.
 ****************************************************************************/

#include "tc_042_uart_event_omission_defect.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_042_TARGET_TOUCHES      5U
#define TC_042_STABLE_PRESS_MS   300U
#define TC_042_SETTLE_MS         800U
#define TC_042_TIMEOUT_MS      10000U

typedef struct
{
    uint8_t inited;
    uint8_t touch_active;
    uint8_t touch_uart_confirmed;
    uint8_t defect_logged;
    uint32_t start_ms;
    uint32_t touch_start_ms;
    uint32_t last_release_ms;
    uint32_t led_react_count;
    uint32_t uart_event_count;
    uint32_t uart_omit_count;
    PlatformButtonRawState prev_raw;
} TC042Context;

/* 시험 시작 시 카운터와 입력 상태를 초기화하고 사용자 입력 조건을 안내한다. */
static void TC_042_Setup(TC042Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->touch_active = 0U;
    ctx->touch_uart_confirmed = 0U;
    ctx->defect_logged = 0U;
    ctx->start_ms = now;
    ctx->touch_start_ms = 0U;
    ctx->last_release_ms = 0U;
    ctx->led_react_count = 0U;
    ctx->uart_event_count = 0U;
    ctx->uart_omit_count = 0U;
    ctx->prev_raw = Platform_ButtonReadRaw();

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_042 START bug=raw_touch_led_reacts_before_uart_confirmation\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_042 ACTION: Touch button lightly %u times\r\n",
               (unsigned long)now,
               (unsigned int)TC_042_TARGET_TOUCHES);
}

/* raw touch가 들어오면 LED를 즉시 반응시켜 사용자가 입력 체감을 바로 확인하게 한다. */
static void TC_042_OnTouch(TC042Context* ctx, uint32_t now)
{
    ctx->touch_active = 1U;
    ctx->touch_uart_confirmed = 0U;
    ctx->touch_start_ms = now;
    ctx->led_react_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_042 PRESS_DETECTED led=%lu/%u uart=%lu target_ms=%u\r\n",
               (unsigned long)now,
               (unsigned long)ctx->led_react_count,
               (unsigned int)TC_042_TARGET_TOUCHES,
               (unsigned long)ctx->uart_event_count,
               (unsigned int)TC_042_STABLE_PRESS_MS);
}

/* 충분히 오래 눌린 입력만 UART event로 확정해 LED 즉시 반응 경로와 차이를 만든다. */
static void TC_042_CheckStablePress(TC042Context* ctx, uint32_t now)
{
    if ((!ctx->touch_active) || ctx->touch_uart_confirmed)
    {
        return;
    }

    if ((now - ctx->touch_start_ms) >= TC_042_STABLE_PRESS_MS)
    {
        ctx->touch_uart_confirmed = 1U;
        ctx->uart_event_count++;

        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_042 UART_EVENT led=%lu/%u uart=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->led_react_count,
                   (unsigned int)TC_042_TARGET_TOUCHES,
                   (unsigned long)ctx->uart_event_count);
    }
}

/* 짧은 터치는 LED 반응만 남고 UART는 누락된 것으로 집계한다. */
static void TC_042_OnRelease(TC042Context* ctx, uint32_t now)
{
    uint32_t hold_ms;

    if (!ctx->touch_active)
    {
        return;
    }

    hold_ms = now - ctx->touch_start_ms;
    ctx->last_release_ms = now;
    ctx->touch_active = 0U;

    if (!ctx->touch_uart_confirmed)
    {
        ctx->uart_omit_count++;

        if (!ctx->defect_logged)
        {
            ctx->defect_logged = 1U;
            Log_Printf(LOG_LEVEL_WARN,
                       "[ms=%lu] TC_042 DEFECT_OBSERVED hold_ms=%lu led=%lu uart=%lu omit=%lu\r\n",
                       (unsigned long)now,
                       (unsigned long)hold_ms,
                       (unsigned long)ctx->led_react_count,
                       (unsigned long)ctx->uart_event_count,
                       (unsigned long)ctx->uart_omit_count);
        }
    }

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_042 RELEASE_DETECTED hold_ms=%lu led=%lu/%u uart=%lu omit=%lu\r\n",
               (unsigned long)now,
               (unsigned long)hold_ms,
               (unsigned long)ctx->led_react_count,
               (unsigned int)TC_042_TARGET_TOUCHES,
               (unsigned long)ctx->uart_event_count,
               (unsigned long)ctx->uart_omit_count);
}

/* 목표 터치 수까지 입력을 누적하며 LED 반응 수와 UART event 수를 각각 집계한다. */
static void TC_042_Stimulus(TC042Context* ctx, uint32_t now)
{
    PlatformButtonRawState raw = Platform_ButtonReadRaw();

    if ((ctx->led_react_count < TC_042_TARGET_TOUCHES) &&
        (raw == PLATFORM_BUTTON_DOWN) &&
        (ctx->prev_raw == PLATFORM_BUTTON_UP))
    {
        TC_042_OnTouch(ctx, now);
    }

    if (raw == PLATFORM_BUTTON_DOWN)
    {
        TC_042_CheckStablePress(ctx, now);
    }

    if ((raw == PLATFORM_BUTTON_UP) &&
        (ctx->prev_raw == PLATFORM_BUTTON_DOWN))
    {
        TC_042_OnRelease(ctx, now);
    }

    ctx->prev_raw = raw;
}

/* 최종적으로 LED 반응 수와 UART event 수 불일치를 요약해 symptom 재현 여부를 판정한다. */
static TestResult TC_042_Verify(TC042Context* ctx, uint32_t now)
{
    if ((ctx->led_react_count >= TC_042_TARGET_TOUCHES) &&
        (!ctx->touch_active) &&
        ((now - ctx->last_release_ms) >= TC_042_SETTLE_MS))
    {
        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_042 SUMMARY touches=%lu led=%lu uart=%lu omit=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->led_react_count,
                   (unsigned long)ctx->led_react_count,
                   (unsigned long)ctx->uart_event_count,
                   (unsigned long)ctx->uart_omit_count);

        if ((ctx->led_react_count == TC_042_TARGET_TOUCHES) &&
            (ctx->uart_event_count < ctx->led_react_count) &&
            (ctx->uart_omit_count > 0U))
        {
            Log_Printf(LOG_LEVEL_INFO,
                       "[ms=%lu] TC_042 RESULT=PASS symptom_reproduced led=%lu uart=%lu omit=%lu\r\n",
                       (unsigned long)now,
                       (unsigned long)ctx->led_react_count,
                       (unsigned long)ctx->uart_event_count,
                       (unsigned long)ctx->uart_omit_count);
            return TEST_PASS;
        }

        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_042 RESULT=FAIL symptom_not_reproduced led=%lu uart=%lu omit=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->led_react_count,
                   (unsigned long)ctx->uart_event_count,
                   (unsigned long)ctx->uart_omit_count);
        return TEST_FAIL;
    }

    if ((now - ctx->start_ms) > TC_042_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_042 RESULT=FAIL reason=TIMEOUT led=%lu uart=%lu omit=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->led_react_count,
                   (unsigned long)ctx->uart_event_count,
                   (unsigned long)ctx->uart_omit_count);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_042_UartEventOmissionDefect_Run(void)
{
    static TC042Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_042_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_042_Stimulus(&ctx, now);
    now = Platform_NowMs();
    return TC_042_Verify(&ctx, now);
}
