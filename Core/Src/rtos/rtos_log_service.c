/****************************************************************************
 * @file    rtos_log_service.c
 * @brief   로그 출력과 버튼 이벤트 기록을 별도 task로 처리한다.
 *
 * @details
 * - 버튼 처리 경로와 blocking UART 출력을 분리해 event 지연을 줄인다.
 * - 로컬 큐 backlog와 dropped count는 시험 관찰 포인트로 남긴다.
 *
 ****************************************************************************/

#include "rtos_log_service.h"
#include "cmsis_os.h"
#include "log.h"
#include "platform_port.h"
#include "rtos_button.h"
#include "rtos_probe.h"
#include "rtos_runtime.h"

/* logger task backlog는 관찰 대상이므로 작은 ring buffer로 dropped count를 함께 기록한다. */
#define RTOS_LOG_SERVICE_LOCAL_QUEUE_DEPTH 16U

static const char *s_queue[RTOS_LOG_SERVICE_LOCAL_QUEUE_DEPTH];
static volatile uint32_t s_head;
static volatile uint32_t s_tail;
static volatile uint32_t s_dropped_count;
static volatile uint32_t s_processed_count;

static const char* RtosLogService_ButtonEventToString(RtosButtonEventType type)
{
    switch (type)
    {
    case RTOS_BUTTON_EVENT_CLICK:
        return "CLICK";
    case RTOS_BUTTON_EVENT_LONG:
        return "LONG";
    default:
        return "NONE";
    }
}

void RtosLogService_Init(void)
{
    s_head = 0U;
    s_tail = 0U;
    s_dropped_count = 0U;
    s_processed_count = 0U;
}

void RtosLogService_ResetStats(void)
{
    s_head = 0U;
    s_tail = 0U;
    s_dropped_count = 0U;
    s_processed_count = 0U;
}

void RtosLogService_Push(const char *msg)
{
    uint32_t next = (s_head + 1U) % RTOS_LOG_SERVICE_LOCAL_QUEUE_DEPTH;

    if (next == s_tail)
    {
        s_dropped_count++;
        return;
    }

    s_queue[s_head] = msg;
    s_head = next;
}

uint32_t RtosLogService_GetDroppedCount(void)
{
    return s_dropped_count;
}

uint32_t RtosLogService_GetProcessedCount(void)
{
    return s_processed_count;
}

uint32_t RtosLogService_GetPendingCount(void)
{
    if (s_head >= s_tail)
    {
        return (s_head - s_tail);
    }

    return (RTOS_LOG_SERVICE_LOCAL_QUEUE_DEPTH - s_tail + s_head);
}

/**
 * @brief   버튼 이벤트와 logger backlog를 RTOS task 문맥에서 기록한다.
 *
 * @param   argument  RTOS task 인자
 *
 * @details
 * - buttonEventQueue에서 이벤트를 수신해 공통 로그 형식으로 출력한다.
 * - 로컬 ring buffer backlog를 비우면서 processed count를 갱신한다.
 * - heartbeat와 watchdog feed tick은 probe 관찰 값으로만 사용한다.
 */
void RtosLogService_Task(void *argument)
{
    uint32_t now_ms;
    uint32_t last_heartbeat_ms;
    uint32_t last_watchdog_ms;
    RtosButtonEvent evt;
    osStatus_t qstatus;

    (void)argument;

    last_heartbeat_ms = Platform_NowMs();
    last_watchdog_ms = last_heartbeat_ms;

    for (;;)
    {
        qstatus = osErrorTimeout;

        /* queue timeout은 오류보다 idle 구간 관찰을 위한 정상 경로로 본다. */
        if (buttonEventQueueHandle != NULL)
        {
            qstatus = osMessageQueueGet(buttonEventQueueHandle, &evt, NULL, 50U);
        }
        else
        {
            Platform_DelayMs(50U);
        }

        now_ms = Platform_NowMs();

        if (qstatus == osOK)
        {
            Log_Printf(LOG_LEVEL_INFO,
                       "[RTOS_EVENT] type=%s tick=%lu\r\n",
                       RtosLogService_ButtonEventToString(evt.type),
                       (unsigned long)evt.tick_ms);
        }

        while (s_tail != s_head)
        {
            const char *msg = s_queue[s_tail];
            s_tail = (s_tail + 1U) % RTOS_LOG_SERVICE_LOCAL_QUEUE_DEPTH;

            Log_Printf(LOG_LEVEL_INFO,
                       "[RTOS_LOG_SERVICE] %s\r\n",
                       (msg != 0) ? msg : "(null)");

            s_processed_count++;
        }

        /* 200ms heartbeat tick은 RTOS 생존성과 logger 지연 관찰 기준이다. */
        if ((now_ms - last_heartbeat_ms) >= 200U)
        {
            OtaRtosProbe_NotifyHeartbeatTick(now_ms);
            last_heartbeat_ms = now_ms;
        }

        /* watchdog feed는 제어 로직이 아니라 servicing 간격 관찰용 probe 값으로 남긴다. */
        if ((now_ms - last_watchdog_ms) >= 100U)
        {
            OtaRtosProbe_NotifyWatchdogFed(now_ms);
            last_watchdog_ms = now_ms;
        }
    }
}

void RtosLogger_Init(void)
{
    RtosLogService_Init();
}

void RtosLogger_ResetStats(void)
{
    RtosLogService_ResetStats();
}

void RtosLogger_Push(const char *msg)
{
    RtosLogService_Push(msg);
}

uint32_t RtosLogger_GetDroppedCount(void)
{
    return RtosLogService_GetDroppedCount();
}

uint32_t RtosLogger_GetProcessedCount(void)
{
    return RtosLogService_GetProcessedCount();
}

uint32_t RtosLogger_GetPendingCount(void)
{
    return RtosLogService_GetPendingCount();
}

void RtosLogger_Task(void *argument)
{
    RtosLogService_Task(argument);
}
