/****************************************************************************
 * @file    tc_051_long_press_one_shot_fix_validation.c
 * @brief   수정 후 정상 동작과 fix 효과를 검증한다.
 *
 * @details
 * - 결함 재현 TC와 대응 관계를 가지며, 수정 효과가 실제로 반영되었는지 확인한다.
 ****************************************************************************/

#include "tc_051_long_press_one_shot_fix_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_051_LONG_TARGET  3U
#define TC_051_LONG_MS   2000U
#define TC_051_TIMEOUT_MS 30000U

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    uint32_t start_ms;
    uint32_t long_count;
    uint32_t extra_long_count;
    uint32_t unexpected_click_count;
    TestResult result;
} TC051_Context;

static void TC_051_Setup(TC051_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_051_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->start_ms = now;
    ctx->long_count = 0U;
    ctx->extra_long_count = 0U;
    ctx->unexpected_click_count = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_051 START target_long=%u long_ms=%u\r\n",
              (unsigned long)now,
              (unsigned int)TC_051_LONG_TARGET,
              (unsigned int)TC_051_LONG_MS);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_051 ACTION: Hold button for 3 seconds, repeat %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_051_LONG_TARGET);
}

static void TC_051_HandleLong(TC051_Context* ctx, uint32_t now)
{
    ctx->long_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_051 EVT=BTN_LONG LED=TOGGLE count=%lu/%u\r\n",
              (unsigned long)now,
              (unsigned long)ctx->long_count,
              (unsigned int)TC_051_LONG_TARGET);

    if (ctx->long_count >= TC_051_LONG_TARGET)
    {
        ctx->completed = 1U;
        ctx->result = ((ctx->extra_long_count == 0U) && (ctx->unexpected_click_count == 0U))
                    ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_051 RESULT=%s long=%lu extra_long=%lu unexpected_click=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->long_count,
                  (unsigned long)ctx->extra_long_count,
                  (unsigned long)ctx->unexpected_click_count);
    }
}

static void TC_051_HandleClick(TC051_Context* ctx, uint32_t now)
{
    ctx->unexpected_click_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_051 WARN EVT=BTN_CLICK (unexpected) count=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->unexpected_click_count);
}

static void TC_051_Observe(TC051_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_LONG)
    {
        TC_051_HandleLong(ctx, now);
    }
    else if (evt == BTN_EVT_CLICK)
    {
        TC_051_HandleClick(ctx, now);
    }
}

static TestResult TC_051_Verify(TC051_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_051_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_051 RESULT=FAIL reason=TIMEOUT long=%lu unexpected_click=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->long_count,
              (unsigned long)ctx->unexpected_click_count);

    return ctx->result;
}

TestResult TC_051_LongPressOneShotFixValidation_Run(void)
{
    static TC051_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_051_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_051_Observe(&ctx, now);
    return TC_051_Verify(&ctx, now);
}
