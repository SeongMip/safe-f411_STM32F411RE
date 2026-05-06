/****************************************************************************
 * @file    tc_062_rapid_input_stress_test.c
 * @brief   수정 이후 회귀, 스트레스, 안정성, 복구 특성을 검증한다.
 *
 * @details
 * - 기능 fix 이후 다른 경로가 깨지지 않았는지 확인한다.
 * - 장시간 동작, reset/power cycle, safe-state 관련 관찰을 포함할 수 있다.
 ****************************************************************************/

#include "tc_062_rapid_input_stress_test.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_062_CLICK_TARGET   50U
#define TC_062_LONG_MS      2000U
#define TC_062_TIMEOUT_MS 120000U

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    uint32_t start_ms;
    uint32_t click_count;
    uint32_t long_count;
    TestResult result;
} TC062_Context;

static void TC_062_Setup(TC062_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_062_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->start_ms = now;
    ctx->click_count = 0U;
    ctx->long_count = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_062 START target_click=%u\r\n",
              (unsigned long)now,
              (unsigned int)TC_062_CLICK_TARGET);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_062 ACTION: Press button rapidly %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_062_CLICK_TARGET);
}

static void TC_062_HandleClick(TC062_Context* ctx, uint32_t now)
{
    ctx->click_count++;
    Platform_LedToggle();

    if (((ctx->click_count % 10U) == 0U) || (ctx->click_count >= TC_062_CLICK_TARGET))
    {
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_062 EVT=BTN_CLICK count=%lu/%u\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->click_count,
                  (unsigned int)TC_062_CLICK_TARGET);
    }

    if (ctx->click_count >= TC_062_CLICK_TARGET)
    {
        ctx->completed = 1U;
        ctx->result = (ctx->long_count == 0U) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_062 RESULT=%s click=%lu long=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->click_count,
                  (unsigned long)ctx->long_count);
    }
}

static void TC_062_HandleLong(TC062_Context* ctx, uint32_t now)
{
    ctx->long_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_062 WARN EVT=BTN_LONG (unexpected)\r\n",
              (unsigned long)now);
}

static void TC_062_Observe(TC062_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_062_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_062_HandleLong(ctx, now);
    }
}

static TestResult TC_062_Verify(TC062_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_062_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_062 RESULT=FAIL reason=TIMEOUT click=%lu/%u long=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned int)TC_062_CLICK_TARGET,
              (unsigned long)ctx->long_count);

    return ctx->result;
}

TestResult TC_062_RapidInputStressTest_Run(void)
{
    static TC062_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_062_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_062_Observe(&ctx, now);
    return TC_062_Verify(&ctx, now);
}
