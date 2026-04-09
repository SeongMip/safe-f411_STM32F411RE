/****************************************************************************
 * @file    tc_013_long_press_toggle_once.c
 * @brief   long-press 입력이 1회만 반영되는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_013_long_press_toggle_once.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"
#include "toggle_verifier.h"

#define TC_013_EXPECTED_LONGS 3U
#define TC_013_LONG_MS        2000U


/**
 * @brief   TC-013의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_013_LongPressToggleOnce_Run(void)
{
    static ButtonFsm btn;
    static TestToggleVerifier verifier = {0};

    uint32_t now = Platform_NowMs();
    PlatformLedState actual_led_state;

    if (!verifier.inited)
    {
        ButtonFsm_Init(&btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_013_LONG_MS);
        TestToggleVerifier_Init(&verifier,
                                "TC_013",
                                "long",
                                TC_013_EXPECTED_LONGS,
                                Platform_LedRead());
    }

    switch (ButtonFsm_Update(&btn, now))
    {
    case BTN_EVT_LONG:
        Platform_LedToggle();
        actual_led_state = Platform_LedRead();
        return TestToggleVerifier_OnExpectedEvent(&verifier, actual_led_state);

    case BTN_EVT_CLICK:
        return TestToggleVerifier_OnUnexpectedEvent(&verifier, "click");

    default:
        break;
    }

    return TEST_IN_REVIEW;
}
