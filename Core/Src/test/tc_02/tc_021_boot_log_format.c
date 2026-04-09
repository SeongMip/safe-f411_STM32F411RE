/****************************************************************************
 * @file    tc_021_boot_log_format.c
 * @brief   부팅 로그 형식이 규약에 맞게 출력되는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_021_boot_log_format.h"
#include "log.h"
#include "platform_port.h"


/**
 * @brief   TC-021의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_021_BootLogFormat_Run(void)
{
    static uint8_t printed = 0;

    if (!printed)
    {
        printed = 1;
        Log_Printf(LOG_LEVEL_INFO,
        		"BOOT ms=%lu\r\n",
				(unsigned long)Platform_NowMs());
        Log_Raw("[TC-021] MANUAL VERIFICATION REQUIRED - observe Boot log\r\n");
    }

    return TEST_IN_REVIEW;
}
