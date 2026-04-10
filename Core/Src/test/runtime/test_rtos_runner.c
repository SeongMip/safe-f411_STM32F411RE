/****************************************************************************
 * @file    test_rtos_runner.c
 * @brief   RTOS 환경에서 선택된 테스트를 실행하고 결과를 기록한다.
 *
 * @details
 * - selection 값에 따라 대상 TC를 결정한다.
 * - TEST_IN_REVIEW 상태를 반복 평가한 뒤 최종 PASS/FAIL을 출력한다.
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

#define RTOS_TEST_REVIEW_RETRY_DELAY_MS 10U
#define RTOS_TEST_CASE_COUNT (sizeof(g_rtos_test_cases) / sizeof(g_rtos_test_cases[0]))

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

    for (i = 0U; i < RTOS_TEST_CASE_COUNT; i++)
    {
        if (g_rtos_test_cases[i].selection == selection)
        {
            return &g_rtos_test_cases[i];
        }
    }

    return 0;
}

static void TestRtosRunner_LogInvalidSelection(RtosRunnerSelection selection)
{
    Log_Printf(LOG_LEVEL_ERROR,
               "[RTOS_TEST] invalid selection=%d\r\n",
               (int)selection);
}

static void TestRtosRunner_LogStart(const RtosTestCaseEntry* test_case)
{
    Log_Printf(LOG_LEVEL_INFO,
               "[RTOS_TEST] START %s\r\n",
               test_case->id);
}

static void TestRtosRunner_LogEnd(const RtosTestCaseEntry* test_case, TestResult result)
{
    Log_Printf(LOG_LEVEL_INFO,
               "[RTOS_TEST] %s => %s\r\n",
               test_case->id,
               (result == TEST_PASS) ? "PASS" : "FAIL");

    Log_Printf(LOG_LEVEL_INFO, "[RTOS_TEST] END\r\n");
}

static TestResult TestRtosRunner_RunUntilFinalResult(const RtosTestCaseEntry* test_case)
{
    TestResult result;

    do
    {
        result = test_case->run();
        if (result == TEST_IN_REVIEW)
        {
            /* TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태 */
            Platform_DelayMs(RTOS_TEST_REVIEW_RETRY_DELAY_MS);
        }
    } while (result == TEST_IN_REVIEW);

    return result;
}

/**
 * @brief   선택된 RTOS 테스트를 실행한다.
 *
 * @details
 * - 유효하지 않은 selection은 즉시 오류로 기록한다.
 * - 테스트가 리뷰 중 상태이면 일정 주기로 재호출하여 종료 조건을 기다린다.
 * - 최종 결과는 공통 로그 형식으로 출력한다.
 */
void TestRtosRunner_RunSelected(void)
{
    const RtosTestCaseEntry* test_case;
    TestResult result;

    test_case = FindSelectedRtosTestCase(TEST_RTOS_TARGET);
    if (test_case == 0)
    {
        TestRtosRunner_LogInvalidSelection(TEST_RTOS_TARGET);
        return;
    }

    TestRtosRunner_LogStart(test_case);
    result = TestRtosRunner_RunUntilFinalResult(test_case);
    TestRtosRunner_LogEnd(test_case, result);
}
