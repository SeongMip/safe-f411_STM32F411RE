/****************************************************************************
 * @file    tc_060_regression_after_button_fix.c
 * @brief   수정 이후 회귀, 스트레스, 안정성, 복구 특성을 검증한다.
 *
 * @details
 * - 기능 fix 이후 다른 경로가 깨지지 않았는지 확인한다.
 * - 장시간 동작, reset/power cycle, safe-state 관련 관찰을 포함할 수 있다.
 ****************************************************************************/

#include "tc_060_regression_after_button_fix.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_060_CLICK_TARGET  10U
#define TC_060_LONG_TARGET    3U
#define TC_060_LONG_MS     2000U
#define TC_060_TIMEOUT_MS 60000U

typedef enum
{
    TC060_PHASE_CLICK = 0,
    TC060_PHASE_LONG,
    TC060_PHASE_DONE
} TC060_Phase;

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    TC060_Phase phase;
    uint32_t start_ms;
    uint32_t click_count;
    uint32_t long_count;
    uint32_t mismatch_count;
    uint32_t sequence_error_count;
    uint8_t expected_led;
    TestResult result;
} TC060_Context;

static void TC_060_Setup(TC060_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_060_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->phase = TC060_PHASE_CLICK;
    ctx->start_ms = now;
    ctx->click_count = 0U;
    ctx->long_count = 0U;
    ctx->mismatch_count = 0U;
    ctx->sequence_error_count = 0U;
    ctx->expected_led = (Platform_LedRead() == PLATFORM_LED_ON) ? 1U : 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_060 START related_scope=TC_012+TC_013 with partial TC_011/TC_022 coverage\r\n",
              (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_060 STEP1: Click button %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_060_CLICK_TARGET);
}

static void TC_060_CheckLed(TC060_Context* ctx, uint32_t now, const char* event_name)
{
    uint8_t actual;

    ctx->expected_led ^= 1U;
    Platform_LedToggle();
    actual = (Platform_LedRead() == PLATFORM_LED_ON) ? 1U : 0U;

    if (actual != ctx->expected_led)
    {
        ctx->mismatch_count++;
    }

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_060 EVT=%s expected=%u actual=%u\r\n",
              (unsigned long)now,
              event_name,
              (unsigned int)ctx->expected_led,
              (unsigned int)actual);
}

static void TC_060_HandleClick(TC060_Context* ctx, uint32_t now)
{
    TC_060_CheckLed(ctx, now, "BTN_CLICK");

    if (ctx->phase != TC060_PHASE_CLICK)
    {
        ctx->sequence_error_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_060 WARN click_out_of_phase phase=%d\r\n",
                  (unsigned long)now,
                  (int)ctx->phase);
        return;
    }

    ctx->click_count++;
    if (ctx->click_count >= TC_060_CLICK_TARGET)
    {
        ctx->phase = TC060_PHASE_LONG;
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_060 STEP1_DONE click=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->click_count);
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_060 STEP2: Hold button for 2 seconds, repeat %u times\r\n",
                  (unsigned long)now,
                  (unsigned int)TC_060_LONG_TARGET);
    }
}

static void TC_060_HandleLong(TC060_Context* ctx, uint32_t now)
{
    TC_060_CheckLed(ctx, now, "BTN_LONG");

    if (ctx->phase != TC060_PHASE_LONG)
    {
        ctx->sequence_error_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_060 WARN long_out_of_phase phase=%d\r\n",
                  (unsigned long)now,
                  (int)ctx->phase);
        return;
    }

    ctx->long_count++;
    if (ctx->long_count >= TC_060_LONG_TARGET)
    {
        ctx->phase = TC060_PHASE_DONE;
        ctx->completed = 1U;
        ctx->result = ((ctx->mismatch_count == 0U) &&
                       (ctx->sequence_error_count == 0U) &&
                       (ctx->click_count == TC_060_CLICK_TARGET) &&
                       (ctx->long_count == TC_060_LONG_TARGET)) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_060 RESULT=%s click=%lu long=%lu mismatch=%lu sequence_error=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->click_count,
                  (unsigned long)ctx->long_count,
                  (unsigned long)ctx->mismatch_count,
                  (unsigned long)ctx->sequence_error_count);
    }
}

static void TC_060_Observe(TC060_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_060_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_060_HandleLong(ctx, now);
    }
}

static TestResult TC_060_Verify(TC060_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_060_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_060 RESULT=FAIL reason=TIMEOUT click=%lu long=%lu mismatch=%lu sequence_error=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned long)ctx->long_count,
              (unsigned long)ctx->mismatch_count,
              (unsigned long)ctx->sequence_error_count);

    return ctx->result;
}

TestResult TC_060_RegressionAfterButtonFix_Run(void)
{
    static TC060_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_060_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_060_Observe(&ctx, now);
    return TC_060_Verify(&ctx, now);
}
