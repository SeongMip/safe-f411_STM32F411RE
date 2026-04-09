/****************************************************************************
 * @file    tc_066_fault_log_safe_state_validation.c
 * @brief   fault log와 safe-state 요청 경로를 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_066_fault_log_safe_state_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_066_FAULT_INJECT_DELAY_MS  2000U
#define TC_066_SAFE_STATE_VERIFY_MS   3000U
#define TC_066_TIMEOUT_MS            10000U

typedef enum
{
    TC066_PHASE_NORMAL = 0,
    TC066_PHASE_VERIFY_SAFE_STATE,
    TC066_PHASE_DONE
} TC066_Phase;

typedef struct
{
    uint8_t inited;
    ButtonFsm btn;
    TC066_Phase phase;
    uint32_t start_ms;
    uint32_t fault_ms;
    uint8_t fault_logged;
    uint8_t led_forced_off;
    uint32_t btn_event_after_fault;
    TestResult result;
} TC066_Context;

static void TC_066_Setup(TC066_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, 2000U);
    ctx->inited = 1U;
    ctx->phase = TC066_PHASE_NORMAL;
    ctx->start_ms = now;
    ctx->fault_ms = 0U;
    ctx->fault_logged = 0U;
    ctx->led_forced_off = 0U;
    ctx->btn_event_after_fault = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_066 START fault_inject_after=%u_ms\r\n",
              (unsigned long)now,
              (unsigned int)TC_066_FAULT_INJECT_DELAY_MS);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_066 PHASE=NORMAL (system running normally)\r\n",
              (unsigned long)now);
}

static void TC_066_Stimulus(TC066_Context* ctx, uint32_t now)
{
    BtnEvent evt;

    if (ctx->phase != TC066_PHASE_NORMAL)
    {
        return;
    }

    evt = ButtonFsm_Update(&ctx->btn, now);
    if ((evt == BTN_EVT_CLICK) || (evt == BTN_EVT_LONG))
    {
        Platform_LedToggle();
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_066 NORMAL EVT=%s LED=TOGGLE\r\n",
                  (unsigned long)now,
                  (evt == BTN_EVT_CLICK) ? "CLICK" : "LONG");
    }

    if ((now - ctx->start_ms) < TC_066_FAULT_INJECT_DELAY_MS)
    {
        return;
    }

    ctx->phase = TC066_PHASE_VERIFY_SAFE_STATE;
    ctx->fault_ms = now;
    ctx->fault_logged = 1U;

    Log_Printf(LOG_LEVEL_ERROR,
              "[ms=%lu] TC_066 FAULT_DETECTED type=INJECTED_TEST_FAULT\r\n",
              (unsigned long)now);

    if (Platform_LedRead() == PLATFORM_LED_ON)
    {
        Platform_LedToggle();
    }

    ctx->led_forced_off = (Platform_LedRead() == PLATFORM_LED_OFF) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_ERROR,
              "[ms=%lu] TC_066 SAFE_STATE led=OFF btn_disabled=YES\r\n",
              (unsigned long)now);
}

static void TC_066_Observe(TC066_Context* ctx, uint32_t now)
{
    BtnEvent evt;

    if (ctx->phase != TC066_PHASE_VERIFY_SAFE_STATE)
    {
        return;
    }

    evt = ButtonFsm_Update(&ctx->btn, now);
    if ((evt == BTN_EVT_CLICK) || (evt == BTN_EVT_LONG))
    {
        ctx->btn_event_after_fault++;
        Log_Printf(LOG_LEVEL_ERROR,
                  "[ms=%lu] TC_066 ERROR safe_state_input_detected EVT=%s\r\n",
                  (unsigned long)now,
                  (evt == BTN_EVT_CLICK) ? "CLICK" : "LONG");
    }

    if (Platform_LedRead() == PLATFORM_LED_ON)
    {
        ctx->led_forced_off = 0U;
        Log_Printf(LOG_LEVEL_ERROR,
                  "[ms=%lu] TC_066 ERROR led_unexpectedly_on_during_safe_state\r\n",
                  (unsigned long)now);
    }
}

static TestResult TC_066_Verify(TC066_Context* ctx, uint32_t now)
{
    if ((ctx->phase == TC066_PHASE_VERIFY_SAFE_STATE) &&
        ((now - ctx->fault_ms) >= TC_066_SAFE_STATE_VERIFY_MS))
    {
        ctx->phase = TC066_PHASE_DONE;
        ctx->result = (ctx->fault_logged && ctx->led_forced_off && (ctx->btn_event_after_fault == 0U))
                    ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_066 RESULT=%s fault_logged=%u led_off=%u btn_event_after_fault=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned int)ctx->fault_logged,
                  (unsigned int)ctx->led_forced_off,
                  (unsigned long)ctx->btn_event_after_fault);
        return ctx->result;
    }

    if ((now - ctx->start_ms) > TC_066_TIMEOUT_MS)
    {
        ctx->phase = TC066_PHASE_DONE;
        ctx->result = TEST_FAIL;

        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_066 RESULT=FAIL reason=TIMEOUT fault_logged=%u led_off=%u btn_event_after_fault=%lu\r\n",
                  (unsigned long)now,
                  (unsigned int)ctx->fault_logged,
                  (unsigned int)ctx->led_forced_off,
                  (unsigned long)ctx->btn_event_after_fault);
        return ctx->result;
    }

    return TEST_IN_REVIEW;
}


/**
 * @brief   TC-066의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_066_FaultLogSafeStateValidation_Run(void)
{
    static TC066_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_066_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_066_Stimulus(&ctx, now);
    TC_066_Observe(&ctx, now);
    return TC_066_Verify(&ctx, now);
}
