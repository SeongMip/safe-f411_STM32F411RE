/****************************************************************************
 * @file    rtos_runtime.h
 * @brief   RTOS 공용 object handle을 extern으로 공유한다.
 *
 * @details
 * - 실제 저장공간은 freertos.c에서 정의하고 각 task 모듈은 이 헤더로 접근한다.
 *
 ****************************************************************************/

#ifndef RTOS_RUNTIME_H
#define RTOS_RUNTIME_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 실제 handle 저장공간은 freertos.c에 정의되고 각 RTOS 모듈은 extern으로 공유한다. */
extern osThreadId_t buttonTaskHandle;
extern osThreadId_t logServiceTaskHandle;
extern osThreadId_t rtosTestTaskHandle;
extern osMessageQueueId_t buttonEventQueueHandle;

#ifdef __cplusplus
}
#endif

#endif
