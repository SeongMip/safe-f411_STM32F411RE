/****************************************************************************
 * @file    tc_010_led_gpio.c
 * @brief   LED GPIO 제어와 가시 동작이 일치하는지 검증한다.
 *
 * @details
 * - 가장 기본적인 출력 경로를 확인하는 기능 TC이다.
 ****************************************************************************/

#include "tc_010_led_gpio.h"
#include "log.h"
#include "platform_gpio.h"
#include "platform_port.h"
#include "test_timer.h"

#define TC_010_TOGGLE_INTERVAL_MS   500U
#define TC_010_REQUIRED_TOGGLES     3U

TestResult TC_010_LedGpioOutput_Run(void)
{
    static uint8_t started = 0;
    static uint8_t toggle_count = 0;
    static PlatformLedState prev_state;
    static TestTimer timer;

    uint32_t now = Platform_NowMs();

    if (!started)
    {
        started = 1;
        toggle_count = 0;
        prev_state = Platform_LedRead();
        TestTimer_Start(&timer, now, TC_010_TOGGLE_INTERVAL_MS);

        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_010] START LED GPIO toggle verification initial=%s\r\n",
                   (prev_state == PLATFORM_LED_ON) ? "ON" : "OFF");
        return TEST_IN_REVIEW;
    }

    if (TestTimer_ExpiredAndReload(&timer, now, TC_010_TOGGLE_INTERVAL_MS))
    {
        PlatformLedState curr_state;

        Platform_LedToggle();
        curr_state = Platform_LedRead();

        if (curr_state == prev_state)
        {
            Log_Printf(LOG_LEVEL_INFO,
                       "[TC_010] AUTO_CHECK=FAIL toggle_count=%u state_not_changed\r\n",
                       (unsigned int)toggle_count);
            return TEST_FAIL;
        }

        toggle_count++;
        prev_state = curr_state;

        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_010] OBSERVE toggle=%u led=%s\r\n",
                   (unsigned int)toggle_count,
                   (curr_state == PLATFORM_LED_ON) ? "ON" : "OFF");

        if (toggle_count >= TC_010_REQUIRED_TOGGLES)
        {
            Log_Printf(LOG_LEVEL_INFO,
                       "[TC_010] AUTO_CHECK=PASS toggles=%u\r\n",
                       (unsigned int)toggle_count);
            return TEST_PASS;
        }
    }

    return TEST_IN_REVIEW;
}
