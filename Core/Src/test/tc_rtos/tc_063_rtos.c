/****************************************************************************
 * @file    tc_063_rtos.c
 * @brief   RTOS 환경에서 watchdog servicing 간격이 허용 범위 내인지 검증한다.
 *
 * @details
 * - probe를 이용해 관찰 구간 동안 watchdog feed 존재 여부와 간격 건전성을 확인한다.
 * - feed 자체가 없거나 허용 gap을 초과하면 FAIL로 판정한다.
 ****************************************************************************/

#include "tc_063_rtos.h"
#include "log.h"
#include "platform_port.h"
#include "rtos_probe.h"

#define TC_063_RTOS_OBSERVE_MS      3000U
#define TC_063_RTOS_MAX_WDG_GAP_MS   500U

typedef struct
{
    uint32_t start_ms;
    uint32_t now_ms;
    uint8_t feed_seen;
} TC063Rtos_Context;

static void TC_063_RTOS_Setup(void)
{
    OtaRtosProbe_Reset();

    Log_Printf(LOG_LEVEL_INFO,
              "[TC_063_RTOS] START watchdog servicing validation\r\n");
}

/**
 * @brief   관찰 구간 동안 watchdog feed가 지속되는지 확인한다.
 *
 * @param   ctx  관찰 상태 저장용 컨텍스트
 *
 * @return  TEST_FAIL 또는 TEST_IN_REVIEW
 *
 * @details
 * - 지정 시간 동안 probe 값을 반복 관찰한다.
 * - feed가 관찰된 뒤 허용 gap을 초과하면 즉시 FAIL 처리한다.
 */
static TestResult TC_063_RTOS_Observe(TC063Rtos_Context* ctx)
{
    ctx->start_ms = Platform_NowMs();
    ctx->feed_seen = 0U;

    while ((Platform_NowMs() - ctx->start_ms) < TC_063_RTOS_OBSERVE_MS)
    {
        ctx->now_ms = Platform_NowMs();

        if (OtaRtosProbe_GetWatchdogFeedCount() > 0U)
        {
            ctx->feed_seen = 1U;

            if (!OtaRtosProbe_IsWatchdogFeedHealthy(ctx->now_ms, TC_063_RTOS_MAX_WDG_GAP_MS))
            {
                Log_Printf(LOG_LEVEL_ERROR,
                          "[TC_063_RTOS] watchdog feed gap exceeded\r\n");
                return TEST_FAIL;
            }
        }

        Platform_DelayMs(10U);
    }

    return TEST_IN_REVIEW;
}

/**
 * @brief   관찰 종료 후 watchdog feed 존재 여부를 최종 판정한다.
 *
 * @param   ctx  관찰 상태 컨텍스트
 *
 * @return  TEST_PASS 또는 TEST_FAIL
 *
 * @details
 * - 관찰 시간 동안 feed가 단 한 번도 없으면 FAIL이다.
 * - feed 존재와 gap 건전성이 확인되면 PASS로 판정한다.
 */
static TestResult TC_063_RTOS_Verify(const TC063Rtos_Context* ctx)
{
    if (!ctx->feed_seen)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_063_RTOS] no watchdog feed observed\r\n");
        return TEST_FAIL;
    }

    return TEST_PASS;
}

/**
 * @brief   TC-063 RTOS watchdog servicing validation을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL
 *
 * @details
 * - probe를 초기화한 뒤 3초 동안 watchdog feed 존재와 gap 건전성을 관찰한다.
 * - feed 부재 또는 허용 gap 초과를 FAIL 기준으로 사용한다.
 */
TestResult TC_063_Rtos_Run(void)
{
    TC063Rtos_Context ctx;
    TestResult observe_result;

    TC_063_RTOS_Setup();
    observe_result = TC_063_RTOS_Observe(&ctx);
    if (observe_result == TEST_FAIL)
    {
        return TEST_FAIL;
    }

    return TC_063_RTOS_Verify(&ctx);
}
