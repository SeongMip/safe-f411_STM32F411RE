/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "config.h"
#include "log.h"
#include "rtos_app.h"
#include "rtos_runtime.h"
#include "test_rtos_runtime.h"

osThreadId_t buttonTaskHandle;
osThreadId_t logServiceTaskHandle;
osThreadId_t rtosTestTaskHandle;
osMessageQueueId_t buttonEventQueueHandle;

void MX_FREERTOS_Init(void);

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  (void)pcTaskName;
  Log_Printf(LOG_LEVEL_ERROR, "[RTOS] stack overflow\r\n");
  Error_Handler();
}

void vApplicationMallocFailedHook(void)
{
  Log_Printf(LOG_LEVEL_ERROR, "[RTOS] malloc failed\r\n");
  Error_Handler();
}

void MX_FREERTOS_Init(void)
{
  /* 공용 service object/task 생성은 실행 역할에 따라 APP/TEST 경로로 나누되 handle 저장공간은 freertos.c에 둔다. */
#if (EXEC_ROLE == EXEC_ROLE_APP)
  RtosApp_CreateObjects();
  RtosApp_CreateTasks();
#elif (EXEC_ROLE == EXEC_ROLE_TEST)
  TestRtosRuntime_CreateObjects();
  TestRtosRuntime_CreateTasks();
#else
#error "Invalid EXEC_ROLE"
#endif
}
