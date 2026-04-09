/****************************************************************************
 * @file    rtos_log_service.h
 * @brief   RTOS 로그 서비스와 logger 호환 API를 선언한다.
 *
 ****************************************************************************/

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
