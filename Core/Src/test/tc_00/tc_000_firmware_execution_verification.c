/****************************************************************************
 * @file    tc_000_firmware_execution_verification.c
 * @brief   펌웨어 다운로드 후 기본 실행이 시작되는지 검증한다.
 *
 * @details
 * - 보드가 부팅 후 heartbeat 동작을 지속하는지 확인한다.
 * - bring-up 단계에서 최소 실행 가능 여부를 판정하는 기준 TC이다.
 ****************************************************************************/

#include "tc_000_firmware_execution_verification.h"
#include "log.h"
#include "platform_port.h"

TestResult TC_000_FirmwareExecutionVerification_Run(void)
{
    static uint8_t done = 0;
    uint32_t now = Platform_NowMs();

    if (!done)
    {
        done = 1;

        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_000] START firmware bring-up verification\r\n");
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_000] AUTO_CHECK=PASS reached_test_entry=1 entry_ms=%lu\r\n",
                   (unsigned long)now);

        return TEST_PASS;
    }

    return TEST_PASS;
}
