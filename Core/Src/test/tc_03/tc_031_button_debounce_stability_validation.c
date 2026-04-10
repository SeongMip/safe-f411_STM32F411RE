/****************************************************************************
 * @file    tc_031_button_debounce_stability_validation.c
 * @brief   타이밍 정확도, debounce 안정성, 부하 영향 등을 검증한다.
 *
 * @details
 * - 기능 동작 여부보다 시간/부하/노이즈 조건에서의 일관성을 보는 그룹이다.
 ****************************************************************************/

#include "tc_031_button_debounce_stability_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_031_SLOW_CLICK_TARGET  10U
#define TC_031_FAST_CLICK_TARGET  10U
#define TC_031_LONG_TARGET         1U
#define TC_031_LONG_MS          2000U
#define TC_031_TIMEOUT_MS      90000U

typedef enum
{
    TC031_PHASE_SLOW = 0,
    TC031_PHASE_FAST,
    TC031_PHASE_LONG,
    TC031_PHASE_DONE
} TC031_Phase;

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    TC031_Phase phase;
    uint32_t start_ms;
    uint32_t slow_click_count;
    uint32_t fast_click_count;
    uint32_t long_count;
    uint32_t error_count;
    TestResult result;
} TC031_Context;

static void TC_031_Setup(TC031_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_031_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->phase = TC031_PHASE_SLOW;
    ctx->start_ms = now;
    ctx->slow_click_count = 0U;
    ctx->fast_click_count = 0U;
    ctx->long_count = 0U;
    ctx->error_count = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_031 START\r\n",
              (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_031 STEP1: Press button slowly 10 times (1 sec interval)\r\n",
              (unsigned long)now);
}

static void TC_031_HandleClick(TC031_Context* ctx, uint32_t now)
{
    Platform_LedToggle();
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] EVT=BTN_CLICK LED=TOGGLE\r\n",
              (unsigned long)now);

    if (ctx->phase == TC031_PHASE_SLOW)
    {
        ctx->slow_click_count++;
        if (ctx->slow_click_count >= TC_031_SLOW_CLICK_TARGET)
        {
            ctx->phase = TC031_PHASE_FAST;
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_031 STEP1_DONE slow_click=%lu\r\n",
                      (unsigned long)now,
                      (unsigned long)ctx->slow_click_count);
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_031 STEP2: Press button rapidly 10 times\r\n",
                      (unsigned long)now);
        }
        return;
    }

    if (ctx->phase == TC031_PHASE_FAST)
    {
        ctx->fast_click_count++;
        if (ctx->fast_click_count >= TC_031_FAST_CLICK_TARGET)
        {
            ctx->phase = TC031_PHASE_LONG;
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_031 STEP2_DONE fast_click=%lu\r\n",
                      (unsigned long)now,
                      (unsigned long)ctx->fast_click_count);
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_031 STEP3: Hold button for 2 seconds\r\n",
                      (unsigned long)now);
        }
        return;
    }

    ctx->error_count++;

    if (ctx->phase == TC031_PHASE_LONG)
    {
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_031 WARN unexpected_click_during_long_phase\r\n",
                  (unsigned long)now);
        return;
    }

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_031 WARN unexpected_click_after_done\r\n",
              (unsigned long)now);
}

static void TC_031_HandleLong(TC031_Context* ctx, uint32_t now)
{
    Platform_LedToggle();
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] EVT=BTN_LONG LED=TOGGLE\r\n",
              (unsigned long)now);

    if (ctx->phase != TC031_PHASE_LONG)
    {
        ctx->error_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_031 WARN unexpected_long_event phase=%d\r\n",
                  (unsigned long)now,
                  (int)ctx->phase);
        return;
    }

    ctx->long_count++;
    if (ctx->long_count >= TC_031_LONG_TARGET)
    {
        ctx->phase = TC031_PHASE_DONE;
        ctx->completed = 1U;
        ctx->result = (ctx->error_count == 0U) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_031 RESULT=%s slow=%lu fast=%lu long=%lu errors=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->slow_click_count,
                  (unsigned long)ctx->fast_click_count,
                  (unsigned long)ctx->long_count,
                  (unsigned long)ctx->error_count);
    }
}

static void TC_031_Observe(TC031_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_031_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_031_HandleLong(ctx, now);
    }
}

static TestResult TC_031_Verify(TC031_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) >= TC_031_TIMEOUT_MS)
    {
        ctx->completed = 1U;
        ctx->result = TEST_FAIL;

        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_031 RESULT=FAIL reason=TIMEOUT slow=%lu fast=%lu long=%lu errors=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->slow_click_count,
                  (unsigned long)ctx->fast_click_count,
                  (unsigned long)ctx->long_count,
                  (unsigned long)ctx->error_count);
        return ctx->result;
    }

    return TEST_IN_REVIEW;
}

TestResult TC_031_ButtonDebounceStabilityValidation_Run(void)
{
    static TC031_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_031_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_031_Observe(&ctx, now);
    return TC_031_Verify(&ctx, now);
}
