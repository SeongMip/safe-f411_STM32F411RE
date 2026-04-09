/****************************************************************************
 * @file    rtos_probe.h
 * @brief   RTOS probe 기준값 구조체와 관찰 API를 선언한다.
 *
 ****************************************************************************/

#ifndef RTOS_PROBE_H
#define RTOS_PROBE_H

#include <stdint.h>

typedef struct
{
    uint32_t heartbeat_expected_ms;
    uint32_t heartbeat_max_jitter_ms;
    uint32_t high_prio_expected_ms;
    uint32_t high_prio_max_latency_ms;
    uint32_t ota_timeout_ms;
} OtaRtosCriteria;

void OtaRtosProbe_Reset(void);

void OtaRtosProbe_NotifyHeartbeatTick(uint32_t now_ms);
void OtaRtosProbe_NotifyHighPrioTaskTick(uint32_t now_ms);
void OtaRtosProbe_NotifyOtaPacketReceived(uint32_t now_ms);
void OtaRtosProbe_NotifyWatchdogFed(uint32_t now_ms);

uint32_t OtaRtosProbe_GetHeartbeatMaxInterval(void);
uint32_t OtaRtosProbe_GetHighPrioMaxInterval(void);
uint32_t OtaRtosProbe_GetLastPacketTime(void);
uint32_t OtaRtosProbe_GetLastWatchdogFeedTime(void);

uint32_t OtaRtosProbe_GetHeartbeatCount(void);
uint32_t OtaRtosProbe_GetHighPrioCount(void);
uint32_t OtaRtosProbe_GetWatchdogFeedCount(void);

uint8_t OtaRtosProbe_IsHeartbeatHealthy(const OtaRtosCriteria* criteria);
uint8_t OtaRtosProbe_IsHighPrioHealthy(const OtaRtosCriteria* criteria);
uint8_t OtaRtosProbe_IsOtaTimeout(uint32_t now_ms, const OtaRtosCriteria* criteria);
uint8_t OtaRtosProbe_IsWatchdogFeedHealthy(uint32_t now_ms, uint32_t max_feed_gap_ms);

#endif
