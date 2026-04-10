/****************************************************************************
 * @file    test_rtos_runtime.c
 * @brief   RTOS 테스트 실행에 필요한 공통 runtime 상태를 관리한다.
 *
 * @details
 * - 테스트 실행 정책, 상태값, 공통 헬퍼를 RTOS 전용으로 제공한다.
 ****************************************************************************/

#include "test_rtos_runtime.h"
#include "cmsis_os.h"
#include "main.h"
#include "rtos_app.h"
#include "rtos_runtime.h"
#include "runtime/test_rtos_runner.h"

static const osThreadAttr_t rtosTestTask_attributes = {
    .name = "rtosTestTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityLow,
};

/* RTOS 테스트 태스크가 기동 후 잠시 대기한 뒤 전체 RTOS 테스트를 수행한다. */
static void StartRtosTestTask(void *argument)
{
    (void)argument;

    osDelay(300U);
    TestRtosRunner_RunSelected();

    for (;;)
    {
        osDelay(1000U);
    }
}

/* RTOS 테스트에 필요한 공용 큐 객체를 RTOS 앱 계층에서 생성한다. */
void TestRtosRuntime_CreateObjects(void)
{
    RtosApp_CreateObjects();
}

/* RTOS 테스트에 필요한 공용 태스크를 생성한 뒤 테스트 전용 태스크를 추가한다. */
void TestRtosRuntime_CreateTasks(void)
{
    RtosApp_CreateTasks();

    rtosTestTaskHandle = osThreadNew(StartRtosTestTask, NULL, &rtosTestTask_attributes);
    if (rtosTestTaskHandle == NULL)
    {
        Error_Handler();
    }
}
