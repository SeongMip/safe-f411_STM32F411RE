/****************************************************************************
 * @file    rtos_app.c
 * @brief   RTOS 공용 서비스 task와 queue 생성을 조합한다.
 *
 * @details
 * - button/logger/monitor/probe 역할 분리를 기준으로 공용 RTOS 실행 경로를 구성한다.
 * - TEST/RTOS에서는 이 공용 서비스 위에 테스트 전용 task만 추가한다.
 *
 ****************************************************************************/

#include "rtos_app.h"
#include "cmsis_os.h"
#include "main.h"
#include "rtos_button.h"
#include "rtos_log_service.h"
#include "rtos_monitor.h"
#include "rtos_probe.h"
#include "rtos_runtime.h"

/* button task는 입력 판정 지연을 줄이기 위해 logger보다 높은 우선순위를 사용한다. */
static const osThreadAttr_t buttonTask_attributes = {
    .name = "buttonTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
};

/* logger task는 blocking UART를 분리하되 입력 task를 선점하지 않도록 normal 우선순위를 사용한다. */
static const osThreadAttr_t logServiceTask_attributes = {
    .name = "logServiceTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

/* button event queue depth 8은 입력 burst 관찰과 자원 사용의 균형점으로 둔다. */
static const osMessageQueueAttr_t buttonEventQueue_attributes = {
    .name = "buttonEventQueue"
};

static void StartButtonTask(void *argument)
{
    RtosButton_Task(argument);
}

static void StartLogServiceTask(void *argument)
{
    RtosLogService_Task(argument);
}

/**
 * @brief   RTOS 공용 service 계층을 초기화한다.
 *
 * @details
 * - monitor, logger, button FSM, probe는 APP/TEST RTOS 경로가 공통으로 재사용한다.
 */
void RtosApp_Init(void)
{
    RtosMonitor_Init();
    RtosLogService_Init();
    RtosButton_Init();
    OtaRtosProbe_Reset();
}

/**
 * @brief   RTOS 공용 queue object를 생성한다.
 *
 * @details
 * - TEST/RTOS도 동일한 button event queue를 공유하므로 이 함수에 생성 책임을 모은다.
 */
void RtosApp_CreateObjects(void)
{
    buttonEventQueueHandle = osMessageQueueNew(8U,
                                               sizeof(RtosButtonEvent),
                                               &buttonEventQueue_attributes);
    if (buttonEventQueueHandle == NULL)
    {
        Error_Handler();
    }
}

/**
 * @brief   RTOS 공용 service task를 생성한다.
 *
 * @details
 * - button/logger task는 APP/TEST RTOS 경로가 공통으로 사용한다.
 */
void RtosApp_CreateTasks(void)
{
    buttonTaskHandle = osThreadNew(StartButtonTask, NULL, &buttonTask_attributes);
    if (buttonTaskHandle == NULL)
    {
        Error_Handler();
    }

    logServiceTaskHandle = osThreadNew(StartLogServiceTask, NULL, &logServiceTask_attributes);
    if (logServiceTaskHandle == NULL)
    {
        Error_Handler();
    }
}

void RtosApp_Start(void)
{
}
