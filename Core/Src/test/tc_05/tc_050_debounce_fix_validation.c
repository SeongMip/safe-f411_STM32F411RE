/****************************************************************************
 * @file    tc_050_debounce_fix_validation.c
 * @brief   수정 후 정상 동작과 fix 효과를 검증한다.
 *
 * @details
 * - 결함 재현 TC와 대응 관계를 가지며, 수정 효과가 실제로 반영되었는지 확인한다.
 ****************************************************************************/

#include "tc_050_debounce_fix_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_050_CLICK_TARGET  10U
#define TC_050_LONG_MS     2000U
#define TC_050_TIMEOUT_MS 30000U

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    uint32_t start_ms;
    uint32_t click_count;
    uint32_t long_count;
    TestResult result;
} TC050_Context;

static void TC_050_Setup(TC050_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_050_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->start_ms = now;
    ctx->click_count = 0U;
    ctx->long_count = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_050 START target_click=%u\r\n",
              (unsigned long)now,
              (unsigned int)TC_050_CLICK_TARGET);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_050 ACTION: Press button %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_050_CLICK_TARGET);
}

static void TC_050_HandleClick(TC050_Context* ctx, uint32_t now)
{
    ctx->click_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_050 EVT=BTN_CLICK LED=TOGGLE count=%lu/%u\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned int)TC_050_CLICK_TARGET);

    if (ctx->click_count >= TC_050_CLICK_TARGET)
    {
        ctx->completed = 1U;
        ctx->result = (ctx->long_count == 0U) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_050 RESULT=%s click=%lu long=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->click_count,
                  (unsigned long)ctx->long_count);
    }
}

static void TC_050_HandleLong(TC050_Context* ctx, uint32_t now)
{
    ctx->long_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_050 WARN EVT=BTN_LONG (unexpected)\r\n",
              (unsigned long)now);
}

static void TC_050_Observe(TC050_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_050_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_050_HandleLong(ctx, now);
    }
}

static TestResult TC_050_Verify(TC050_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_050_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_050 RESULT=FAIL reason=TIMEOUT click=%lu long=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned long)ctx->long_count);

    return ctx->result;
}

TestResult TC_050_DebounceFixValidation_Run(void)
{
    static TC050_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_050_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_050_Observe(&ctx, now);
    return TC_050_Verify(&ctx, now);
}
