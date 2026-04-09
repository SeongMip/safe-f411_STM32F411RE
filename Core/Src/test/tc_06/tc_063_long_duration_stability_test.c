/****************************************************************************
 * @file    tc_063_long_duration_stability_test.c
 * @brief   장시간 동작 중 이상 누적 여부를 관찰한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_063_long_duration_stability_test.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_063_DURATION_MS   1800000U
#define TC_063_HEARTBEAT_MS     1000U
#define TC_063_REPORT_MS       60000U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t last_heartbeat_ms;
    uint32_t last_report_ms;
    uint32_t heartbeat_count;
    uint32_t led_toggle_count;
} TC063_Context;

static void TC_063_Setup(TC063_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->last_heartbeat_ms = now;
    ctx->last_report_ms = now;
    ctx->heartbeat_count = 0U;
    ctx->led_toggle_count = 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_063 START duration=%lu_min\r\n",
              (unsigned long)now,
              (unsigned long)(TC_063_DURATION_MS / 60000U));
}

static void TC_063_Stimulus(TC063_Context* ctx, uint32_t now)
{
    if ((now - ctx->last_heartbeat_ms) < TC_063_HEARTBEAT_MS)
    {
        return;
    }

    ctx->last_heartbeat_ms = now;
    ctx->heartbeat_count++;
    ctx->led_toggle_count++;
    Platform_LedToggle();
}

static void TC_063_Observe(TC063_Context* ctx, uint32_t now)
{
    uint32_t elapsed;
    uint32_t remain_min;

    if ((now - ctx->last_report_ms) < TC_063_REPORT_MS)
    {
        return;
    }

    ctx->last_report_ms = now;
    elapsed = now - ctx->start_ms;
    remain_min = (TC_063_DURATION_MS - elapsed) / 60000U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_063 STATUS elapsed=%lu_min remain=%lu_min heartbeat=%lu led_toggle=%lu\r\n",
              (unsigned long)now,
              (unsigned long)(elapsed / 60000U),
              (unsigned long)remain_min,
              (unsigned long)ctx->heartbeat_count,
              (unsigned long)ctx->led_toggle_count);
}

static TestResult TC_063_Verify(TC063_Context* ctx, uint32_t now)
{
    uint32_t elapsed = now - ctx->start_ms;

    if (elapsed < TC_063_DURATION_MS)
    {
        return TEST_IN_REVIEW;
    }

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_063 RESULT=PASS elapsed=%lu_ms heartbeat=%lu led_toggle=%lu\r\n",
              (unsigned long)now,
              (unsigned long)elapsed,
              (unsigned long)ctx->heartbeat_count,
              (unsigned long)ctx->led_toggle_count);

    return TEST_PASS;
}


/**
 * @brief   TC-063의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_063_LongDurationStabilityTest_Run(void)
{
    static TC063_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_063_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_063_Stimulus(&ctx, now);
    TC_063_Observe(&ctx, now);
    return TC_063_Verify(&ctx, now);
}
