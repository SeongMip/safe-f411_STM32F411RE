/****************************************************************************
 * @file    rtos_button.h
 * @brief   RTOS 버튼 이벤트 형식과 버튼 task API를 선언한다.
 *
 ****************************************************************************/

#ifndef RTOS_BUTTON_H
#define RTOS_BUTTON_H

#include <stdint.h>

typedef enum
{
    RTOS_BUTTON_EVENT_NONE = 0,
    RTOS_BUTTON_EVENT_CLICK,
    RTOS_BUTTON_EVENT_LONG
} RtosButtonEventType;

typedef struct
{
    RtosButtonEventType type;
    uint32_t tick_ms;
} RtosButtonEvent;

void RtosButton_Init(void);
void RtosButton_Task(void *argument);

uint32_t RtosButton_GetQueuedEventCount(void);
uint32_t RtosButton_GetQueueOverflowCount(void);

#endif
