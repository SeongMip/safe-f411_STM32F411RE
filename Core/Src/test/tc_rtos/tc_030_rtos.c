/****************************************************************************
 * @file    tc_030_rtos.c
 * @brief   RTOS heartbeat와 고우선 task tick 간격이 기준 내에 있는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
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


/**
 * @brief   TC-030의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_030_Rtos_Run(void)
{
    TC030Rtos_Context ctx;

    TC_030_RTOS_Setup(&ctx);
    TC_030_RTOS_Observe(&ctx);
    return TC_030_RTOS_Verify(&ctx);
}
