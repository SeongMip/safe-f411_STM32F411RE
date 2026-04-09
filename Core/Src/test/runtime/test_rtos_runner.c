/****************************************************************************
 * @file    test_rtos_runner.c
 * @brief   RTOS 환경에서 선택된 테스트를 실행하고 결과를 기록한다.
 *
 * @details
 * - selection 값에 따라 대상 TC를 결정한다.
 * - TEST_IN_REVIEW 상태이면 주기적으로 재호출하여 종료 조건을 기다린다.
 *
 ****************************************************************************/

#include "test_rtos_runner.h"
#include "test_config.h"
#include "test_result.h"
#include "log.h"
#include "platform_port.h"
#include "tc_030_rtos.h"
#include "tc_032_rtos.h"
#include "tc_042_rtos.h"
#include "tc_063_rtos.h"
#include "tc_066_rtos.h"

typedef TestResult (*RtosTestRunFn)(void);

static const RtosTestCaseEntry g_rtos_test_cases[] =
{
    { RUN_TC_030_RTOS, "TC_030_RTOS", TC_030_Rtos_Run },
    { RUN_TC_032_RTOS, "TC_032_RTOS", TC_032_Rtos_Run },
    { RUN_TC_042_RTOS, "TC_042_RTOS", TC_042_Rtos_Run },
    { RUN_TC_063_RTOS, "TC_063_RTOS", TC_063_Rtos_Run },
    { RUN_TC_066_RTOS, "TC_066_RTOS", TC_066_Rtos_Run },
};

static const RtosTestCaseEntry* FindSelectedRtosTestCase(RtosRunnerSelection selection)
{
    uint32_t i;

    for (i = 0U; i < (sizeof(g_rtos_test_cases) / sizeof(g_rtos_test_cases[0])); i++)
    {
        if (g_rtos_test_cases[i].selection == selection)
        {
            return &g_rtos_test_cases[i];
        }
    }
    return 0;
}

/**
 * @brief   선택된 RTOS TC를 실행하고 최종 PASS/FAIL을 기록한다.
 *
 * @details
 * - 유효하지 않은 selection은 즉시 오류로 기록한다.
 * - TEST_IN_REVIEW 상태는 10ms 간격으로 재평가한다.
 */
void TestRtosRunner_RunSelected(void)
{
    const RtosTestCaseEntry* test_case;
    TestResult result;

    test_case = FindSelectedRtosTestCase(TEST_RTOS_TARGET);
    if (test_case == 0)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                   "[RTOS_TEST] invalid selection=%d\r\n",
                   (int)TEST_RTOS_TARGET);
        return;
    }

    Log_Printf(LOG_LEVEL_INFO,
               "[RTOS_TEST] START %s\r\n",
               test_case->id);

    do
    {
        result = test_case->run();
        if (result == TEST_IN_REVIEW)
        {
            Platform_DelayMs(10U);
        }
    } while (result == TEST_IN_REVIEW);

    Log_Printf(LOG_LEVEL_INFO,
               "[RTOS_TEST] %s => %s\r\n",
               test_case->id,
               (result == TEST_PASS) ? "PASS" : "FAIL");

    Log_Printf(LOG_LEVEL_INFO, "[RTOS_TEST] END\r\n");
}
