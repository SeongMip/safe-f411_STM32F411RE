#ifndef TEST_RTOS_RUNNER_H
#define TEST_RTOS_RUNNER_H

#include "test_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef TestResult (*RtosTestRunFn)(void);

typedef enum {
    RUN_TC_030_RTOS = 30,
    RUN_TC_032_RTOS = 32,
    RUN_TC_042_RTOS = 42,
    RUN_TC_063_RTOS = 63,
    RUN_TC_066_RTOS = 66,
} RtosRunnerSelection;

typedef struct {
    RtosRunnerSelection selection;
    const char* id;
    RtosTestRunFn run;
} RtosTestCaseEntry;

void TestRtosRunner_RunSelected(void);

#ifdef __cplusplus
}
#endif

#endif
