/****************************************************************************
 * @file    tc_061_related_function_regression_summary.c
 * @brief   연관 기능 회귀 결과를 요약 관점에서 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_061_related_function_regression_summary.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_061_CLICK_TARGET  10U
#define TC_061_LONG_TARGET    3U
#define TC_061_LONG_MS     2000U
#define TC_061_TIMEOUT_MS 60000U

typedef enum
{
    TC061_PHASE_LED_CHECK = 0,
    TC061_PHASE_CLICK,
    TC061_PHASE_LONG,
    TC061_PHASE_DONE
} TC061_Phase;

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    TC061_Phase phase;
    uint32_t start_ms;
    uint32_t click_count;
    uint32_t long_count;
    uint32_t click_mismatch_count;
    uint32_t long_mismatch_count;
    uint32_t sequence_error_count;
    uint8_t expected_led;
    uint8_t led_check_pass;
    uint8_t click_pass;
    uint8_t long_pass;
    TestResult result;
} TC061_Context;

static void TC_061_Setup(TC061_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_061_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->phase = TC061_PHASE_LED_CHECK;
    ctx->start_ms = now;
    ctx->click_count = 0U;
    ctx->long_count = 0U;
    ctx->click_mismatch_count = 0U;
    ctx->long_mismatch_count = 0U;
    ctx->sequence_error_count = 0U;
    ctx->expected_led = (Platform_LedRead() == PLATFORM_LED_ON) ? 1U : 0U;
    ctx->led_check_pass = 0U;
    ctx->click_pass = 0U;
    ctx->long_pass = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_061 START regression_scope=TC010+TC011+TC012+TC013+TC022\r\n",
              (unsigned long)now);
}

static void TC_061_RunLedCheck(TC061_Context* ctx, uint32_t now)
{
    PlatformLedState before = Platform_LedRead();
    PlatformLedState after;

    Platform_LedToggle();
    after = Platform_LedRead();
    Platform_LedToggle();

    ctx->led_check_pass = (before != after) ? 1U : 0U;
    ctx->expected_led = (Platform_LedRead() == PLATFORM_LED_ON) ? 1U : 0U;
    ctx->phase = TC061_PHASE_CLICK;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_061 SUB=TC010_LED_GPIO result=%s\r\n",
              (unsigned long)now,
              ctx->led_check_pass ? "PASS" : "FAIL");
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_061 STEP: Click button %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_061_CLICK_TARGET);
}

static uint8_t TC_061_CheckLed(TC061_Context* ctx, uint32_t now, const char* event_name)
{
    uint8_t actual;

    ctx->expected_led ^= 1U;
    Platform_LedToggle();
    actual = (Platform_LedRead() == PLATFORM_LED_ON) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_061 EVT=%s expected=%u actual=%u\r\n",
              (unsigned long)now,
              event_name,
              (unsigned int)ctx->expected_led,
              (unsigned int)actual);

    return (actual == ctx->expected_led) ? 1U : 0U;
}

static void TC_061_HandleClick(TC061_Context* ctx, uint32_t now)
{
    if (!TC_061_CheckLed(ctx, now, "BTN_CLICK"))
    {
        if (ctx->phase == TC061_PHASE_CLICK)
        {
            ctx->click_mismatch_count++;
        }
        else
        {
            ctx->long_mismatch_count++;
        }
    }

    if (ctx->phase != TC061_PHASE_CLICK)
    {
        ctx->sequence_error_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_061 WARN click_out_of_phase phase=%d\r\n",
                  (unsigned long)now,
                  (int)ctx->phase);
        return;
    }

    ctx->click_count++;
    if (ctx->click_count >= TC_061_CLICK_TARGET)
    {
        ctx->click_pass = (ctx->click_mismatch_count == 0U) ? 1U : 0U;
        ctx->phase = TC061_PHASE_LONG;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_061 SUB=TC012_CLICK result=%s click=%lu\r\n",
                  (unsigned long)now,
                  ctx->click_pass ? "PASS" : "FAIL",
                  (unsigned long)ctx->click_count);
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_061 STEP: Hold button for 2 seconds, repeat %u times\r\n",
                  (unsigned long)now,
                  (unsigned int)TC_061_LONG_TARGET);
    }
}

static void TC_061_HandleLong(TC061_Context* ctx, uint32_t now)
{
    if (!TC_061_CheckLed(ctx, now, "BTN_LONG"))
    {
        ctx->long_mismatch_count++;
    }

    if (ctx->phase != TC061_PHASE_LONG)
    {
        ctx->sequence_error_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_061 WARN long_out_of_phase phase=%d\r\n",
                  (unsigned long)now,
                  (int)ctx->phase);
        return;
    }

    ctx->long_count++;
    if (ctx->long_count >= TC_061_LONG_TARGET)
    {
        ctx->long_pass = (ctx->long_mismatch_count == 0U) ? 1U : 0U;
        ctx->phase = TC061_PHASE_DONE;
        ctx->completed = 1U;
        ctx->result = (ctx->led_check_pass && ctx->click_pass && ctx->long_pass &&
                       (ctx->sequence_error_count == 0U)) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_061 SUB=TC013_LONG result=%s long=%lu\r\n",
                  (unsigned long)now,
                  ctx->long_pass ? "PASS" : "FAIL",
                  (unsigned long)ctx->long_count);
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_061 RESULT=%s LED=%s CLICK=%s LONG=%s sequence_error=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  ctx->led_check_pass ? "PASS" : "FAIL",
                  ctx->click_pass ? "PASS" : "FAIL",
                  ctx->long_pass ? "PASS" : "FAIL",
                  (unsigned long)ctx->sequence_error_count);
    }
}

static void TC_061_Observe(TC061_Context* ctx, uint32_t now)
{
    BtnEvent evt;

    if (ctx->phase == TC061_PHASE_LED_CHECK)
    {
        TC_061_RunLedCheck(ctx, now);
        return;
    }

    evt = ButtonFsm_Update(&ctx->btn, now);
    if (evt == BTN_EVT_CLICK)
    {
        TC_061_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_061_HandleLong(ctx, now);
    }
}

static TestResult TC_061_Verify(TC061_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_061_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_061 RESULT=FAIL reason=TIMEOUT LED=%s CLICK=%s LONG=%s sequence_error=%lu\r\n",
              (unsigned long)now,
              ctx->led_check_pass ? "PASS" : "FAIL",
              ctx->click_pass ? "PASS" : "FAIL",
              ctx->long_pass ? "PASS" : "FAIL",
              (unsigned long)ctx->sequence_error_count);

    return ctx->result;
}


/**
 * @brief   TC-061의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_061_RelatedFunctionRegressionSummary_Run(void)
{
    static TC061_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_061_Setup(&ctx, now);
    }

    TC_061_Observe(&ctx, now);
    return TC_061_Verify(&ctx, now);
}
