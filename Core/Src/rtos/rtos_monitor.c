/****************************************************************************
 * @file    rtos_monitor.c
 * @brief   RTOS 실행 중 queue overflow, 지연, 이상 상태를 감시한다.
 *
 * @details
 * - 제품 제어 로직보다 관찰 및 이상 징후 기록에 초점을 둔다.
 * - monitor 결과는 결함 재현 및 시험 로그의 근거로 사용한다.
 ****************************************************************************/

#include "rtos_monitor.h"

#define RTOS_MONITOR_STACK_WARNING_MIN_WATERMARK 64U

static volatile uint8_t s_safe_state_requested;
static volatile uint32_t s_queue_overflow_count;
static volatile uint32_t s_last_stack_warning_task_id;
static volatile uint32_t s_last_stack_warning_watermark;

static uint8_t RtosMonitor_ShouldRequestSafeStateOnStackWarning(uint32_t watermark)
{
    return (watermark < RTOS_MONITOR_STACK_WARNING_MIN_WATERMARK) ? 1U : 0U;
}

void RtosMonitor_Init(void)
{
    s_safe_state_requested = 0U;
    s_queue_overflow_count = 0U;
    s_last_stack_warning_task_id = 0U;
    s_last_stack_warning_watermark = 0U;
}

void RtosMonitor_NotifyQueueOverflow(void)
{
    s_queue_overflow_count++;
    s_safe_state_requested = 1U;
}

void RtosMonitor_NotifyStackWarning(uint32_t task_id, uint32_t watermark)
{
    s_last_stack_warning_task_id = task_id;
    s_last_stack_warning_watermark = watermark;

    if (RtosMonitor_ShouldRequestSafeStateOnStackWarning(watermark))
    {
        s_safe_state_requested = 1U;
    }
}

void RtosMonitor_RequestSafeState(void)
{
    s_safe_state_requested = 1U;
}

uint8_t RtosMonitor_IsSafeStateRequested(void)
{
    return s_safe_state_requested;
}
