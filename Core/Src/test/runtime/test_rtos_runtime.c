/****************************************************************************
 * @file    test_rtos_runtime.c
 * @brief   RTOS 테스트 전용 task를 공용 RTOS 서비스 위에 추가한다.
 *
 * @details
 * - queue와 공용 service task 생성은 rtos_app 계층에 위임하고 테스트 task만 별도로 생성한다.
 *
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

/**
 * @brief   RTOS 테스트 전용 task에서 선택된 RTOS TC를 시작한다.
 *
 * @details
 * - scheduler 기동 직후 300ms 대기 후 테스트를 시작해 초기 boot log와 object 생성 구간을 분리한다.
 */
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

/**
 * @brief   RTOS 테스트 경로에서 공용 queue object를 생성한다.
 *
 * @details
 * - TEST/RTOS 경로는 object 생성 자체가 아니라 공용 rtos_app 계층 재사용에 의미가 있다.
 */
void TestRtosRuntime_CreateObjects(void)
{
    RtosApp_CreateObjects();
}

/**
 * @brief   공용 service task 뒤에 RTOS 테스트 전용 task를 추가한다.
 *
 * @details
 * - button/logger task 생성은 rtos_app 계층에 위임하고 테스트 runner task만 별도로 올린다.
 */
void TestRtosRuntime_CreateTasks(void)
{
    RtosApp_CreateTasks();

    rtosTestTaskHandle = osThreadNew(StartRtosTestTask, NULL, &rtosTestTask_attributes);
    if (rtosTestTaskHandle == NULL)
    {
        Error_Handler();
    }
}
