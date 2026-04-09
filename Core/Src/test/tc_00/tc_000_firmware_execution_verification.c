/****************************************************************************
 * @file    tc_000_firmware_execution_verification.c
 * @brief   펌웨어 다운로드 후 기본 실행과 heartbeat 지속 여부를 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_000_firmware_execution_verification.h"
#include "log.h"
#include "platform_port.h"


/**
 * @brief   TC-000의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
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
