/****************************************************************************
 * @file    rtos_app.c
 * @brief   RTOS 기반 실행 흐름과 주요 task 구성을 연결한다.
 *
 * @details
 * - button/logger/monitor/probe 역할 분리를 기준으로 task를 조합한다.
 * - bare-metal 대비 처리 경로를 분리해 이벤트 누락과 지연을 관찰 가능하게 한다.
 ****************************************************************************/

#include "rtos_app.h"
#include "cmsis_os.h"
#include "main.h"
#include "rtos_button.h"
#include "rtos_log_service.h"
#include "rtos_monitor.h"
#include "rtos_probe.h"
#include "rtos_runtime.h"

static const osThreadAttr_t buttonTask_attributes = {
    .name = "buttonTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
};

static const osThreadAttr_t logServiceTask_attributes = {
    .name = "logServiceTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

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

void RtosApp_Init(void)
{
    RtosMonitor_Init();
    RtosLogService_Init();
    RtosButton_Init();
    OtaRtosProbe_Reset();
}

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
