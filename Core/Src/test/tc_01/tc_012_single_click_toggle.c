/****************************************************************************
 * @file    tc_012_single_click_toggle.c
 * @brief   단일 click 입력당 LED가 정확히 1회만 토글되는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_012_single_click_toggle.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"
#include "toggle_verifier.h"

#define TC_012_EXPECTED_CLICKS 10U
#define TC_012_LONG_MS         2000U


/**
 * @brief   TC-012 단일 click 입력의 1회 토글 보장을 검증한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 기대 click 수와 실제 LED 토글 상태를 비교한다.
 * - long 이벤트가 발생하거나 기대 LED 상태와 다르면 FAIL이다.
 */
TestResult TC_012_SingleClickToggle_Run(void)
{
    static ButtonFsm btn;
    static TestToggleVerifier verifier = {0};

    uint32_t now = Platform_NowMs();
    PlatformLedState actual_led_state;

    if (!verifier.inited)
    {
        ButtonFsm_Init(&btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_012_LONG_MS);
        TestToggleVerifier_Init(&verifier,
                                "TC_012",
                                "click",
                                TC_012_EXPECTED_CLICKS,
                                Platform_LedRead());
    }

    switch (ButtonFsm_Update(&btn, now))
    {
    case BTN_EVT_CLICK:
        Platform_LedToggle();
        actual_led_state = Platform_LedRead();
        return TestToggleVerifier_OnExpectedEvent(&verifier, actual_led_state);

    case BTN_EVT_LONG:
        return TestToggleVerifier_OnUnexpectedEvent(&verifier, "long");

    default:
        break;
    }

    return TEST_IN_REVIEW;
}
