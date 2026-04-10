/****************************************************************************
 * @file    tc_012_single_click_toggle.c
 * @brief   단일 click 입력당 LED가 정확히 1회만 토글되는지 검증한다.
 *
 * @details
 * - debounce 이후 확정된 click 이벤트만 LED 반영 대상으로 본다.
 * - bounce나 상태 전이 오류로 1 click당 2회 이상 토글되면 FAIL이다.
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
 * - 예상 click 횟수와 실제 LED 상태 변화를 비교한다.
 * - long 이벤트가 발생하면 잘못된 판정으로 보고 FAIL 처리한다.
 *
 * @observe
 * - button FSM 이벤트
 * - actual LED state
 * - expected click count 대비 verifier 상태
 *
 * @pass
 * - 정해진 click 횟수 동안 모든 입력이 1회 토글로 끝난다.
 *
 * @fail
 * - long 이벤트 발생
 * - 중복 toggle
 * - click 미검출
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
