#ifndef RTOS_LOG_SERVICE_H
#define RTOS_LOG_SERVICE_H

#include <stdint.h>

void RtosLogService_Init(void);
void RtosLogService_Task(void *argument);

void RtosLogService_Push(const char *msg);

uint32_t RtosLogService_GetDroppedCount(void);
uint32_t RtosLogService_GetProcessedCount(void);
uint32_t RtosLogService_GetPendingCount(void);

void RtosLogService_ResetStats(void);

void RtosLogger_Init(void);
void RtosLogger_Task(void *argument);
void RtosLogger_ResetStats(void);
void RtosLogger_Push(const char *msg);
uint32_t RtosLogger_GetDroppedCount(void);
uint32_t RtosLogger_GetProcessedCount(void);
uint32_t RtosLogger_GetPendingCount(void);

#endif
