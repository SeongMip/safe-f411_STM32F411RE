/****************************************************************************
 * @file    tc_022_button_event_uart_mapping.c
 * @brief   버튼 이벤트와 UART 로그 표현이 일관되게 매핑되는지 검증한다.
 *
 * @details
 * - LED 반응과 UART 로그 시점 차이를 구분해 관찰하는 기준 TC이다.
 ****************************************************************************/

#include "tc_022_button_event_uart_mapping.h"
#include "log.h"
#include "platform_port.h"
#include "button_fsm.h"
#include "test_config.h"
#include "platform_gpio.h"
#include "toggle_verifier.h"

#define TC_022_EXPECTED_CLICKS 10U
#define TC_022_LONG_MS 2000U

TestResult TC_022_ButtonEventUartMap_Run(void)
{
    static ButtonFsm btn;
    static TestToggleVerifier verifier = {0};
    static uint8_t manual_logged = 0;

    uint32_t now = Platform_NowMs();
    PlatformLedState actual_led;

    if (!verifier.inited)
    {
        ButtonFsm_Init(&btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_022_LONG_MS);
        TestToggleVerifier_Init(&verifier,
                                "TC_022",
                                "click",
                                TC_022_EXPECTED_CLICKS,
                                Platform_LedRead());
    }

    if (!manual_logged)
    {
    	manual_logged = 1;
        Log_Raw("[TC-022] MANUAL VERIFICATION REQUIRED - observe UART log\r\n");

    }

    switch (ButtonFsm_Update(&btn, now))
    {
    case BTN_EVT_CLICK:
        Platform_LedToggle();
        actual_led = Platform_LedRead();
        Log_Printf(LOG_LEVEL_INFO,
                  "[TC_022] EVT=BTN_CLICK LED=TOGGLE count=%lu/%lu expected_led=%d actual_led=%d\r\n",
                  (unsigned long)(verifier.count + 1U),
                  (unsigned long)TC_022_EXPECTED_CLICKS,
                  (int)verifier.expected_next_led_state,
                  (int)actual_led);
        return TestToggleVerifier_OnExpectedEvent(&verifier, actual_led);

    default:
        break;
    }

    return TEST_IN_REVIEW;
}
