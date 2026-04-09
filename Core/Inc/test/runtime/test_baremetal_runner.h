/****************************************************************************
 * @file    test_baremetal_runner.h
 * @brief   Bare-metal 테스트 선택 및 실행 API를 선언한다.
 *
 ****************************************************************************/

#ifndef TEST_BAREMETAL_RUNNER_H
#define TEST_BAREMETAL_RUNNER_H

#include "test_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef TestResult (*TestRunFn)(void);

typedef enum {
    RUN_TC_000 = 0,
    RUN_TC_001 = 1,
    RUN_TC_002 = 2,
    RUN_TC_010 = 10,
    RUN_TC_011 = 11,
    RUN_TC_012 = 12,
    RUN_TC_013 = 13,
    RUN_TC_020 = 20,
    RUN_TC_021 = 21,
    RUN_TC_022 = 22,
    RUN_TC_030 = 30,
    RUN_TC_031 = 31,
    RUN_TC_032 = 32,
    RUN_TC_040 = 40,
    RUN_TC_041 = 41,
    RUN_TC_042 = 42,
    RUN_TC_050 = 50,
    RUN_TC_051 = 51,
    RUN_TC_052 = 52,
    RUN_TC_060 = 60,
    RUN_TC_061 = 61,
    RUN_TC_062 = 62,
    RUN_TC_063 = 63,
    RUN_TC_064 = 64,
    RUN_TC_065 = 65,
    RUN_TC_066 = 66,
} RunnerSelection;

typedef struct {
    RunnerSelection selection;
    const char* id;
    TestRunFn run;
} TestCaseEntry;

void TestBaremetalRunner_RunSelected(void);


#ifdef __cplusplus
}
#endif

#endif
