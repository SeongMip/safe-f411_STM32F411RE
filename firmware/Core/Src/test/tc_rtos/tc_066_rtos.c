/****************************************************************************
 * @file    tc_066_rtos.c
 * @brief   RTOS 구조에서 task/queue/probe/watchdog 관련 동작을 검증한다.
 *
 * @details
 * - bare-metal에서 드러난 한계를 RTOS 구조가 어떻게 완화하는지 확인한다.
 * - queue 지연, logger 누락, watchdog feed, safe-state 경로를 관찰한다.
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

TestResult TC_066_Rtos_Run(void)
{
    TC_066_RTOS_Setup();
    TC_066_RTOS_Stimulus();
    return TC_066_RTOS_Verify();
}
