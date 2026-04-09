/****************************************************************************
 * @file    tc_020_uart_sanity.c
 * @brief   UART 로그 출력 경로가 기본적으로 정상 동작하는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_020_uart_sanity.h"
#include "log.h"
#include "test_timer.h"
#include "platform_port.h"


/**
 * @brief   TC-020의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
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
