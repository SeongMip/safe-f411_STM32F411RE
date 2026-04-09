/****************************************************************************
 * @file    tc_066_rtos.c
 * @brief   RTOS monitor와 safe-state 요청 경로가 기대대로 집계되는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_066_rtos.h"
#include "log.h"
#include "rtos_monitor.h"

static void TC_066_RTOS_Setup(void)
{
    Log_Printf(LOG_LEVEL_INFO,
              "[TC_066_RTOS] START safe-state transition validation\r\n");
}

static void TC_066_RTOS_Stimulus(void)
{
    RtosMonitor_RequestSafeState();
}

static TestResult TC_066_RTOS_Verify(void)
{
    if (!RtosMonitor_IsSafeStateRequested())
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_066_RTOS] safe-state request not latched\r\n");
        return TEST_FAIL;
    }

    return TEST_PASS;
}


/**
 * @brief   TC-066의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_066_Rtos_Run(void)
{
    TC_066_RTOS_Setup();
    TC_066_RTOS_Stimulus();
    return TC_066_RTOS_Verify();
}
