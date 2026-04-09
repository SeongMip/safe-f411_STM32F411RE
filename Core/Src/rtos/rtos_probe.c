/****************************************************************************
 * @file    rtos_probe.c
 * @brief   RTOS heartbeat, high-priority tick, watchdog feed 관찰 값을 수집한다.
 *
 * @details
 * - probe 값은 제품 제어가 아니라 시험 관찰용 통계로 사용한다.
 *
 ****************************************************************************/

#include "rtos_probe.h"

static uint32_t s_last_heartbeat_ms;
static uint32_t s_last_high_prio_ms;
static uint32_t s_last_packet_ms;
static uint32_t s_last_watchdog_feed_ms;

static uint32_t s_heartbeat_max_interval_ms;
static uint32_t s_high_prio_max_interval_ms;

static uint32_t s_heartbeat_count;
static uint32_t s_high_prio_count;
static uint32_t s_watchdog_feed_count;

/**
 * @brief   RTOS 관찰용 probe 통계를 초기화한다.
 *
 * @details
 * - probe 값은 제품 제어가 아니라 TC와 monitor의 관찰 근거로 사용한다.
 */
void OtaRtosProbe_Reset(void)
{
    s_last_heartbeat_ms = 0U;
    s_last_high_prio_ms = 0U;
    s_last_packet_ms = 0U;
    s_last_watchdog_feed_ms = 0U;

    s_heartbeat_max_interval_ms = 0U;
    s_high_prio_max_interval_ms = 0U;

    s_heartbeat_count = 0U;
    s_high_prio_count = 0U;
    s_watchdog_feed_count = 0U;
}

void OtaRtosProbe_NotifyHeartbeatTick(uint32_t now_ms)
{
    uint32_t interval;

    if (s_last_heartbeat_ms != 0U)
    {
        interval = now_ms - s_last_heartbeat_ms;
        if (interval > s_heartbeat_max_interval_ms)
        {
            s_heartbeat_max_interval_ms = interval;
        }
    }

    s_last_heartbeat_ms = now_ms;
    s_heartbeat_count++;
}

void OtaRtosProbe_NotifyHighPrioTaskTick(uint32_t now_ms)
{
    uint32_t interval;

    if (s_last_high_prio_ms != 0U)
    {
        interval = now_ms - s_last_high_prio_ms;
        if (interval > s_high_prio_max_interval_ms)
        {
            s_high_prio_max_interval_ms = interval;
        }
    }

    s_last_high_prio_ms = now_ms;
    s_high_prio_count++;
}

void OtaRtosProbe_NotifyOtaPacketReceived(uint32_t now_ms)
{
    s_last_packet_ms = now_ms;
}

void OtaRtosProbe_NotifyWatchdogFed(uint32_t now_ms)
{
    s_last_watchdog_feed_ms = now_ms;
    s_watchdog_feed_count++;
}

uint32_t OtaRtosProbe_GetHeartbeatMaxInterval(void)
{
    return s_heartbeat_max_interval_ms;
}

uint32_t OtaRtosProbe_GetHighPrioMaxInterval(void)
{
    return s_high_prio_max_interval_ms;
}

uint32_t OtaRtosProbe_GetLastPacketTime(void)
{
    return s_last_packet_ms;
}

uint32_t OtaRtosProbe_GetLastWatchdogFeedTime(void)
{
    return s_last_watchdog_feed_ms;
}

uint32_t OtaRtosProbe_GetHeartbeatCount(void)
{
    return s_heartbeat_count;
}

uint32_t OtaRtosProbe_GetHighPrioCount(void)
{
    return s_high_prio_count;
}

uint32_t OtaRtosProbe_GetWatchdogFeedCount(void)
{
    return s_watchdog_feed_count;
}

uint8_t OtaRtosProbe_IsHeartbeatHealthy(const OtaRtosCriteria* criteria)
{
    if (s_heartbeat_count < 2U)
    {
        return 0U;
    }

    return (s_heartbeat_max_interval_ms <=
            (criteria->heartbeat_expected_ms + criteria->heartbeat_max_jitter_ms));
}

uint8_t OtaRtosProbe_IsHighPrioHealthy(const OtaRtosCriteria* criteria)
{
    if (s_high_prio_count < 2U)
    {
        return 0U;
    }

    return (s_high_prio_max_interval_ms <=
            (criteria->high_prio_expected_ms + criteria->high_prio_max_latency_ms));
}

uint8_t OtaRtosProbe_IsOtaTimeout(uint32_t now_ms, const OtaRtosCriteria* criteria)
{
    if (s_last_packet_ms == 0U)
    {
        return 0U;
    }

    return ((now_ms - s_last_packet_ms) > criteria->ota_timeout_ms);
}

/**
 * @brief   마지막 watchdog feed 이후 간격이 허용 범위 내인지 확인한다.
 *
 * @param   now_ms           현재 tick(ms)
 * @param   max_feed_gap_ms  허용 가능한 최대 feed 공백
 *
 * @return  1이면 healthy, 0이면 unhealthy
 */
uint8_t OtaRtosProbe_IsWatchdogFeedHealthy(uint32_t now_ms, uint32_t max_feed_gap_ms)
{
    if (s_watchdog_feed_count == 0U)
    {
        return 0U;
    }

    return ((now_ms - s_last_watchdog_feed_ms) <= max_feed_gap_ms);
}
