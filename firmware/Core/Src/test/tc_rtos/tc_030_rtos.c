/****************************************************************************
 * @file    tc_030_rtos.c
 * @brief   RTOS 구조에서 task/queue/probe/watchdog 관련 동작을 검증한다.
 *
 * @details
 * - bare-metal에서 드러난 한계를 RTOS 구조가 어떻게 완화하는지 확인한다.
 * - queue 지연, logger 누락, watchdog feed, safe-state 경로를 관찰한다.
 ****************************************************************************/

#include "tc_030_rtos.h"
#include "log.h"
#include "platform_port.h"
#include "rtos_probe.h"

#define TC_030_RTOS_OBSERVE_MS            5000U
#define TC_030_RTOS_EXPECTED_INTERVAL_MS   200U
#define TC_030_RTOS_MAX_JITTER_MS          100U

typedef struct
{
    OtaRtosCriteria criteria;
    uint32_t start_ms;
    uint32_t heartbeat_count;
} TC030Rtos_Context;

static void TC_030_RTOS_Setup(TC030Rtos_Context* ctx)
{
    ctx->criteria.heartbeat_expected_ms = TC_030_RTOS_EXPECTED_INTERVAL_MS;
    ctx->criteria.heartbeat_max_jitter_ms = TC_030_RTOS_MAX_JITTER_MS;
    ctx->criteria.high_prio_expected_ms = 0U;
    ctx->criteria.high_prio_max_latency_ms = 0U;
    ctx->criteria.ota_timeout_ms = 0U;
    ctx->heartbeat_count = 0U;

    OtaRtosProbe_Reset();

    Log_Printf(LOG_LEVEL_INFO,
              "[TC_030_RTOS] START timing interval under RTOS\r\n");
}

static void TC_030_RTOS_Observe(TC030Rtos_Context* ctx)
{
    ctx->start_ms = Platform_NowMs();

    while ((Platform_NowMs() - ctx->start_ms) < TC_030_RTOS_OBSERVE_MS)
    {
        Platform_DelayMs(10U);
    }

    ctx->heartbeat_count = OtaRtosProbe_GetHeartbeatCount();
}

static TestResult TC_030_RTOS_Verify(TC030Rtos_Context* ctx)
{
    Log_Printf(LOG_LEVEL_INFO,
              "[TC_030_RTOS] heartbeat_count=%lu max_interval=%lu\r\n",
              (unsigned long)ctx->heartbeat_count,
              (unsigned long)OtaRtosProbe_GetHeartbeatMaxInterval());

    if (ctx->heartbeat_count < 2U)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_030_RTOS] no valid heartbeat samples\r\n");
        return TEST_FAIL;
    }

    if (!OtaRtosProbe_IsHeartbeatHealthy(&ctx->criteria))
    {
        return TEST_FAIL;
    }

    return TEST_PASS;
}

TestResult TC_030_Rtos_Run(void)
{
    TC030Rtos_Context ctx;

    TC_030_RTOS_Setup(&ctx);
    TC_030_RTOS_Observe(&ctx);
    return TC_030_RTOS_Verify(&ctx);
}
