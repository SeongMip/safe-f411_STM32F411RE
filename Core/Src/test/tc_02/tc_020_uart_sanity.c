/****************************************************************************
 * @file    tc_020_uart_sanity.c
 * @brief   UART 로그 출력 경로가 기본적으로 정상 동작하는지 검증한다.
 *
 * @details
 * - 이후 TC의 로그 해석 신뢰성을 확보하기 위한 선행 점검이다.
 ****************************************************************************/

#include "tc_020_uart_sanity.h"
#include "log.h"
#include "test_timer.h"
#include "platform_port.h"

TestResult TC_020_UartSanity_Run(void)
{
    static TestTimer t;
    static uint8_t manual_logged = 0;
    uint32_t now = Platform_NowMs();

    if (!manual_logged)
    {
    	manual_logged = 1;
    	Log_Raw("[TC-020] MANUAL VERIFICATION REQUIRED - observe UART log\r\n");
    }

    if (TestTimer_ExpiredAndReload(&t, now, 1000))
    {
        Log_Raw("PING\r\n");
    }

    return TEST_IN_REVIEW;
}
