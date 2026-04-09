/****************************************************************************
 * @file    rtos_monitor.c
 * @brief   queue overflow와 stack warning을 safe-state 요청으로 집계한다.
 *
 * @details
 * - 제품 제어보다 이상 징후 관찰과 안전 상태 요청 여부 기록에 초점을 둔다.
 *
 ****************************************************************************/

#include "rtos_monitor.h"

static volatile uint8_t s_safe_state_requested;
static volatile uint32_t s_queue_overflow_count;
static volatile uint32_t s_last_stack_warning_task_id;
static volatile uint32_t s_last_stack_warning_watermark;

void RtosMonitor_Init(void)
{
    s_safe_state_requested = 0U;
    s_queue_overflow_count = 0U;
    s_last_stack_warning_task_id = 0U;
    s_last_stack_warning_watermark = 0U;
}

/**
 * @brief   queue overflow를 이상 징후로 집계하고 safe-state 요청을 올린다.
 *
 * @details
 * - overflow 자체는 즉시 복구 정책보다 시험 관찰과 이상 상태 누적 근거로 사용한다.
 */
void RtosMonitor_NotifyQueueOverflow(void)
{
    s_queue_overflow_count++;
    s_safe_state_requested = 1U;
}

void RtosMonitor_NotifyStackWarning(uint32_t task_id, uint32_t watermark)
{
    s_last_stack_warning_task_id = task_id;
    s_last_stack_warning_watermark = watermark;

    /* watermark 64 미만은 stack 여유 부족 경고로 보고 safe-state 요청을 남긴다. */
    if (watermark < 64U)
        s_safe_state_requested = 1U;
}

void RtosMonitor_RequestSafeState(void)
{
    s_safe_state_requested = 1U;
}

uint8_t RtosMonitor_IsSafeStateRequested(void)
{
    return s_safe_state_requested;
}
