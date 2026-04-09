/****************************************************************************
 * @file    toggle_verifier.c
 * @brief   기대 이벤트 수와 실제 LED 토글 결과를 비교 검증한다.
 *
 * @details
 * - click/long 입력에 대한 LED 반응을 정량적으로 판정하기 위한 보조 모듈이다.
 *
 ****************************************************************************/

#include "toggle_verifier.h"
#include "log.h"

static PlatformLedState TestToggleVerifier_InvertLedState(PlatformLedState state)
{
    return (state == PLATFORM_LED_OFF) ? PLATFORM_LED_ON : PLATFORM_LED_OFF;
}

/**
 * @brief   LED toggle 검증 상태를 초기화한다.
 *
 * @details
 * - initial LED state를 기준으로 다음 기대 상태를 계산해 1 click/1 long 대비 실제 토글을 정량 비교한다.
 */
void TestToggleVerifier_Init(TestToggleVerifier* verifier,
                             const char* tc_id,
                             const char* expected_event_name,
                             uint32_t target_count,
                             PlatformLedState initial_led_state)
{
    verifier->inited = 1U;
    verifier->count = 0U;
    verifier->target_count = target_count;
    verifier->expected_next_led_state = TestToggleVerifier_InvertLedState(initial_led_state);
    verifier->tc_id = tc_id;
    verifier->expected_event_name = expected_event_name;
}

/**
 * @brief   기대한 이벤트가 발생했을 때 실제 LED 상태가 예상과 일치하는지 검증한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 */
TestResult TestToggleVerifier_OnExpectedEvent(TestToggleVerifier* verifier,
                                              PlatformLedState actual_led_state)
{
    Log_Printf(LOG_LEVEL_INFO,
              "[%s] event=%s count=%lu/%lu expected_led=%d actual_led=%d\r\n",
              verifier->tc_id,
              verifier->expected_event_name,
              (unsigned long)(verifier->count + 1U),
              (unsigned long)verifier->target_count,
              (int)verifier->expected_next_led_state,
              (int)actual_led_state);

    if (actual_led_state != verifier->expected_next_led_state)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[%s] FAIL: event=%s count=%lu/%lu expected_led=%d actual_led=%d\r\n",
                  verifier->tc_id,
                  verifier->expected_event_name,
                  (unsigned long)(verifier->count + 1U),
                  (unsigned long)verifier->target_count,
                  (int)verifier->expected_next_led_state,
                  (int)actual_led_state);
        return TEST_FAIL;
    }

    verifier->count++;
    verifier->expected_next_led_state = TestToggleVerifier_InvertLedState(verifier->expected_next_led_state);

    if (verifier->count >= verifier->target_count)
    {
        Log_Printf(LOG_LEVEL_INFO,
                  "[%s] PASS: %lu %s events matched expected LED toggle behavior\r\n",
                  verifier->tc_id,
                  (unsigned long)verifier->count,
                  verifier->expected_event_name);
        return TEST_PASS;
    }

    return TEST_IN_REVIEW;
}

/**
 * @brief   기대하지 않은 이벤트가 들어오면 즉시 FAIL로 판정한다.
 */
TestResult TestToggleVerifier_OnUnexpectedEvent(TestToggleVerifier* verifier,
                                                const char* actual_event_name)
{
    Log_Printf(LOG_LEVEL_ERROR,
              "[%s] FAIL: unexpected_event=%s while_expected=%s\r\n",
              verifier->tc_id,
              actual_event_name,
              verifier->expected_event_name);
    return TEST_FAIL;
}
