/****************************************************************************
 * @file    tc_021_boot_log_format.c
 * @brief   부팅 로그 형식이 규약에 맞게 출력되는지 검증한다.
 *
 * @details
 * - 시험 결과서와 host tool에서 동일한 태그 체계를 사용할 수 있게 하기 위한 TC이다.
 ****************************************************************************/

#include "tc_021_boot_log_format.h"
#include "log.h"
#include "platform_port.h"

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
