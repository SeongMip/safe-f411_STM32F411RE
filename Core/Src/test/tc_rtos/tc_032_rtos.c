/****************************************************************************
 * @file    tc_032_rtos.c
 * @brief   RTOS 부하 조건에서 button/logger 분리 구조의 관찰 값을 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_032_rtos.h"
#include "log.h"
#include "platform_port.h"
#include "rtos_probe.h"

#define TC_032_RTOS_OBSERVE_MS             5000U
#define TC_032_RTOS_HIGH_PRIO_EXPECTED_MS    50U
#define TC_032_RTOS_MAX_LATENCY_MS           20U

typedef struct
{
    OtaRtosCriteria criteria;
    uint32_t start_ms;
    uint32_t high_prio_count;
} TC032Rtos_Context;

static void TC_032_RTOS_Setup(TC032Rtos_Context* ctx)
{
    ctx->criteria.heartbeat_expected_ms = 0U;
    ctx->criteria.heartbeat_max_jitter_ms = 0U;
    ctx->criteria.high_prio_expected_ms = TC_032_RTOS_HIGH_PRIO_EXPECTED_MS;
    ctx->criteria.high_prio_max_latency_ms = TC_032_RTOS_MAX_LATENCY_MS;
    ctx->criteria.ota_timeout_ms = 0U;
    ctx->high_prio_count = 0U;

    OtaRtosProbe_Reset();

    Log_Printf(LOG_LEVEL_INFO,
              "[TC_032_RTOS] START load impact under RTOS\r\n");
}

static void TC_032_RTOS_Observe(TC032Rtos_Context* ctx)
{
    ctx->start_ms = Platform_NowMs();

    while ((Platform_NowMs() - ctx->start_ms) < TC_032_RTOS_OBSERVE_MS)
    {
        Platform_DelayMs(10U);
    }

    ctx->high_prio_count = OtaRtosProbe_GetHighPrioCount();
}

static TestResult TC_032_RTOS_Verify(TC032Rtos_Context* ctx)
{
    Log_Printf(LOG_LEVEL_INFO,
              "[TC_032_RTOS] high_prio_count=%lu max_interval=%lu\r\n",
              (unsigned long)ctx->high_prio_count,
              (unsigned long)OtaRtosProbe_GetHighPrioMaxInterval());

    if (ctx->high_prio_count < 2U)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_032_RTOS] no valid high-prio samples\r\n");
        return TEST_FAIL;
    }

    if (!OtaRtosProbe_IsHighPrioHealthy(&ctx->criteria))
    {
        return TEST_FAIL;
    }

    return TEST_PASS;
}


/**
 * @brief   TC-032의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_032_Rtos_Run(void)
{
    TC032Rtos_Context ctx;

    TC_032_RTOS_Setup(&ctx);
    TC_032_RTOS_Observe(&ctx);
    return TC_032_RTOS_Verify(&ctx);
}
