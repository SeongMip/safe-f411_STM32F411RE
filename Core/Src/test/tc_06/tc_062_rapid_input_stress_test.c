/****************************************************************************
 * @file    tc_062_rapid_input_stress_test.c
 * @brief   빠른 연속 입력 조건에서 안정성을 관찰한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
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


/**
 * @brief   TC-062의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
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
