#ifndef RTOS_RUNTIME_H
#define RTOS_RUNTIME_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

extern osThreadId_t buttonTaskHandle;
extern osThreadId_t logServiceTaskHandle;
extern osThreadId_t rtosTestTaskHandle;
extern osMessageQueueId_t buttonEventQueueHandle;

#ifdef __cplusplus
}
#endif

#endif
