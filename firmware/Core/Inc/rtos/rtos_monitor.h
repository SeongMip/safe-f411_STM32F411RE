#ifndef RTOS_MONITOR_H
#define RTOS_MONITOR_H

#include <stdint.h>

void RtosMonitor_Init(void);
void RtosMonitor_NotifyQueueOverflow(void);
void RtosMonitor_NotifyStackWarning(uint32_t task_id, uint32_t watermark);
void RtosMonitor_RequestSafeState(void);
uint8_t RtosMonitor_IsSafeStateRequested(void);

#endif
