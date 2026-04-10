/****************************************************************************
 * @file    rtos_button.c
 * @brief   버튼 이벤트를 RTOS task 문맥에서 판정하고 queue로 전달한다.
 *
 * @details
 * - polling 기반 FSM으로 버튼 이벤트를 생성한다.
 * - 이벤트 처리와 후속 로그/검증 경로를 분리하기 위해 queue를 사용한다.
 * - queue overflow는 즉시 폐기하지 않고 관찰 대상 통계로 남긴다.
 *
 * @note
 * - ISR 직접 UART 출력 대신 task 문맥 처리 구조를 유지한다.
 ****************************************************************************/

#include "rtos_button.h"
#include "cmsis_os.h"
#include "button_fsm.h"
#include "platform_port.h"
#include "log.h"
#include "rtos_monitor.h"
#include "rtos_probe.h"
#include "rtos_runtime.h"

static volatile uint32_t s_button_event_queued_count;
static volatile uint32_t s_button_queue_overflow_count;

static ButtonFsm s_button;

static RtosButtonEventType RtosButton_ConvertEvent(BtnEvent evt)
{
    switch (evt)
    {
    case BTN_EVT_CLICK:
        return RTOS_BUTTON_EVENT_CLICK;
    case BTN_EVT_LONG:
        return RTOS_BUTTON_EVENT_LONG;
    default:
        return RTOS_BUTTON_EVENT_NONE;
    }
}

void RtosButton_Init(void)
{
    s_button_event_queued_count = 0U;
    s_button_queue_overflow_count = 0U;

    /* press/release bounce를 줄이기 위해 30ms debounce와 1500ms long-press 기준을 적용 */
    ButtonFsm_Init(&s_button, 30U, 30U, 1500U);
}

uint32_t RtosButton_GetQueuedEventCount(void)
{
    return s_button_event_queued_count;
}

uint32_t RtosButton_GetQueueOverflowCount(void)
{
    return s_button_queue_overflow_count;
}

/**
 * @brief   버튼 FSM을 주기적으로 갱신하고 이벤트를 queue에 적재한다.
 *
 * @param   argument  RTOS task 인자
 *
 * @details
 * - 10ms 주기로 버튼 상태를 샘플링한다.
 * - 유효 이벤트가 발생하면 queue에 전달하고 overflow 통계를 기록한다.
 * - probe tick을 함께 갱신하여 scheduler 지연 관찰에 사용한다.
 */
void RtosButton_Task(void *argument)
{
    uint32_t now_ms;
    uint32_t last_probe_ms;
    BtnEvent btn_evt;
    RtosButtonEvent evt;

    (void)argument;
    last_probe_ms = Platform_NowMs();

    for (;;)
    {
        now_ms = Platform_NowMs();

        if ((now_ms - last_probe_ms) >= 50U)
        {
            OtaRtosProbe_NotifyHighPrioTaskTick(now_ms);
            last_probe_ms = now_ms;
        }

        btn_evt = ButtonFsm_Update(&s_button, now_ms);
        evt.type = RtosButton_ConvertEvent(btn_evt);
        evt.tick_ms = now_ms;

        if (evt.type != RTOS_BUTTON_EVENT_NONE)
        {
            if (buttonEventQueueHandle != NULL)
            {
                if (osMessageQueuePut(buttonEventQueueHandle, &evt, 0U, 0U) == osOK)
                {
                    s_button_event_queued_count++;
                }
                else
                {
                    /* queue overflow는 기능 실패가 아니라 관찰 대상이므로 통계를 남긴다. */
                    s_button_queue_overflow_count++;
                    RtosMonitor_NotifyQueueOverflow();

                    Log_Printf(LOG_LEVEL_WARN,
                               "[RTOS_BUTTON] queue full type=%lu overflow=%lu\r\n",
                               (unsigned long)evt.type,
                               (unsigned long)s_button_queue_overflow_count);
                }
            }
        }

        Platform_DelayMs(10U);
    }
}
