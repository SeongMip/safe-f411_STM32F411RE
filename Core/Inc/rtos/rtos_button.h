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
