/****************************************************************************
 * @file    tc_043_uart_event_overcount_cause.c
 * @brief   raw polling 입력을 즉시 UART event로 확정하는 취약 경로에서
 *          단일 물리 입력이 다중 UART event로 잘못 처리되는 원인을 재현한다.
 *
 * @details
 * - debounce와 release latch를 두지 않고 raw DOWN을 바로 event로 해석한다.
 * - 같은 눌림 상태가 loop 반복 동안 여러 번 UART event로 처리되는 원인 경로를 보여준다.
 * - symptom 재현인 TC_042와 달리, 이 TC는 직접적인 cause path 확인에 초점을 둔다.
 ****************************************************************************/

#include "tc_043_uart_event_overcount_cause.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_043_EXPECTED_PHYSICAL_CLICKS   1U
#define TC_043_SAMPLE_PERIOD_MS          15U
#define TC_043_SETTLE_MS                800U
#define TC_043_TIMEOUT_MS              7000U

typedef struct
{
    uint8_t inited;
    uint8_t press_active;
    uint8_t release_seen;
    uint8_t defect_logged;
    uint32_t start_ms;
    uint32_t press_start_ms;
    uint32_t release_ms;
    uint32_t last_sample_ms;
    uint32_t physical_click_count;
    uint32_t uart_event_count;
    PlatformButtonRawState prev_raw;
} TC043Context;

/* 시험 시작 시 카운터와 상태를 초기화하고 입력 조건을 안내한다. */
static void TC_043_Setup(TC043Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->press_active = 0U;
    ctx->release_seen = 0U;
    ctx->defect_logged = 0U;
    ctx->start_ms = now;
    ctx->press_start_ms = 0U;
    ctx->release_ms = 0U;
    ctx->last_sample_ms = 0U;
    ctx->physical_click_count = 0U;
    ctx->uart_event_count = 0U;
    ctx->prev_raw = Platform_ButtonReadRaw();

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_043 START bug=immediate_raw_uart_event_without_release_latch\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_043 ACTION: Press button once and hold for about 80~150ms\r\n",
               (unsigned long)now);
}

/* 같은 눌림 상태가 몇 번의 UART event로 처리됐는지 즉시 누적해 보여준다. */
static void TC_043_RecordUartEvent(TC043Context* ctx, uint32_t now)
{
    ctx->uart_event_count++;

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_043 UART_EVENT physical=%lu uart=%lu\r\n",
               (unsigned long)now,
               (unsigned long)ctx->physical_click_count,
               (unsigned long)ctx->uart_event_count);

    if ((!ctx->defect_logged) &&
        (ctx->physical_click_count == TC_043_EXPECTED_PHYSICAL_CLICKS) &&
        (ctx->uart_event_count > ctx->physical_click_count))
    {
        ctx->defect_logged = 1U;

        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_043 DEFECT_OBSERVED physical=%lu uart=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->physical_click_count,
                   (unsigned long)ctx->uart_event_count);
    }
}

/* raw DOWN을 즉시 event로 확정하고 release 전에도 반복 event 처리하는 취약 경로를 재현한다. */
static void TC_043_Stimulus(TC043Context* ctx, uint32_t now)
{
    PlatformButtonRawState raw = Platform_ButtonReadRaw();

    if (ctx->release_seen)
    {
        ctx->prev_raw = raw;
        return;
    }

    if ((raw == PLATFORM_BUTTON_DOWN) && (ctx->prev_raw == PLATFORM_BUTTON_UP))
    {
        ctx->press_active = 1U;
        ctx->press_start_ms = now;
        ctx->last_sample_ms = now;
        ctx->physical_click_count = 1U;

        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_043 PHYSICAL_CLICK count=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->physical_click_count);

        TC_043_RecordUartEvent(ctx, now);
        ctx->prev_raw = raw;
        return;
    }

    if (ctx->press_active && (raw == PLATFORM_BUTTON_DOWN))
    {
        if ((now - ctx->last_sample_ms) >= TC_043_SAMPLE_PERIOD_MS)
        {
            ctx->last_sample_ms = now;
            TC_043_RecordUartEvent(ctx, now);
        }

        ctx->prev_raw = raw;
        return;
    }

    if (ctx->press_active &&
        (raw == PLATFORM_BUTTON_UP) &&
        (ctx->prev_raw == PLATFORM_BUTTON_DOWN))
    {
        ctx->press_active = 0U;
        ctx->release_seen = 1U;
        ctx->release_ms = now;

        Log_Printf(LOG_LEVEL_INFO,
                   "[ms=%lu] TC_043 RELEASE_DETECTED hold_ms=%lu physical=%lu uart=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)(now - ctx->press_start_ms),
                   (unsigned long)ctx->physical_click_count,
                   (unsigned long)ctx->uart_event_count);
    }

    ctx->prev_raw = raw;
}

/* release 이후 물리 입력 수와 UART event 수를 비교해 원인 재현 여부를 판정한다. */
static TestResult TC_043_Verify(TC043Context* ctx, uint32_t now)
{
    if (ctx->release_seen && ((now - ctx->release_ms) >= TC_043_SETTLE_MS))
    {
        if ((ctx->physical_click_count == TC_043_EXPECTED_PHYSICAL_CLICKS) &&
            (ctx->uart_event_count > ctx->physical_click_count))
        {
            Log_Printf(LOG_LEVEL_INFO,
                       "[ms=%lu] TC_043 RESULT=PASS cause_reproduced physical=%lu uart=%lu\r\n",
                       (unsigned long)now,
                       (unsigned long)ctx->physical_click_count,
                       (unsigned long)ctx->uart_event_count);
            return TEST_PASS;
        }

        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_043 RESULT=FAIL cause_not_reproduced physical=%lu uart=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->physical_click_count,
                   (unsigned long)ctx->uart_event_count);
        return TEST_FAIL;
    }

    if ((now - ctx->start_ms) > TC_043_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_043 RESULT=FAIL reason=TIMEOUT physical=%lu uart=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)ctx->physical_click_count,
                   (unsigned long)ctx->uart_event_count);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_043_UartEventOvercountCause_Run(void)
{
    static TC043Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_043_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_043_Stimulus(&ctx, now);
    now = Platform_NowMs();
    return TC_043_Verify(&ctx, now);
}
