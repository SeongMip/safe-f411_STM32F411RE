/****************************************************************************
 * @file    tc_030_timing_interval_accuracy_validation.c
 * @brief   주기 동작의 시간 정확도가 허용 오차 내에 있는지 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_030_timing_interval_accuracy_validation.h"
#include "log.h"
#include "platform_port.h"
#include "test_timer.h"

#define TC_030_INTERVAL_MS         1000U
#define TC_030_TOLERANCE_MIN_MS     950U
#define TC_030_TOLERANCE_MAX_MS    1050U
#define TC_030_REQUIRED_SAMPLES      20U

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    TestTimer timer;
    uint32_t last_log_ms;
    uint32_t sample_count;
    uint32_t violation_count;
    uint32_t sum_delta;
    uint32_t min_delta;
    uint32_t max_delta;
    TestResult result;
} TC030_Context;

static void TC_030_Setup(TC030_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->last_log_ms = now;
    ctx->sample_count = 0U;
    ctx->violation_count = 0U;
    ctx->sum_delta = 0U;
    ctx->min_delta = 0xFFFFFFFFU;
    ctx->max_delta = 0U;
    ctx->result = TEST_IN_REVIEW;
    TestTimer_Start(&ctx->timer, now, TC_030_INTERVAL_MS);

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_030 START interval=%lu tolerance=%lu~%lums samples=%u\r\n",
              (unsigned long)now,
              (unsigned long)TC_030_INTERVAL_MS,
              (unsigned long)TC_030_TOLERANCE_MIN_MS,
              (unsigned long)TC_030_TOLERANCE_MAX_MS,
              (unsigned int)TC_030_REQUIRED_SAMPLES);
}

static void TC_030_Observe(TC030_Context* ctx, uint32_t now)
{
    uint32_t delta_ms;
    const char* status;

    if (!TestTimer_ExpiredAndReload(&ctx->timer, now, TC_030_INTERVAL_MS))
    {
        return;
    }

    delta_ms = now - ctx->last_log_ms;
    ctx->last_log_ms = now;
    ctx->sample_count++;
    ctx->sum_delta += delta_ms;

    if (delta_ms < ctx->min_delta)
    {
        ctx->min_delta = delta_ms;
    }

    if (delta_ms > ctx->max_delta)
    {
        ctx->max_delta = delta_ms;
    }

    if ((delta_ms < TC_030_TOLERANCE_MIN_MS) || (delta_ms > TC_030_TOLERANCE_MAX_MS))
    {
        ctx->violation_count++;
        status = "OUT_OF_RANGE";
    }
    else
    {
        status = "OK";
    }

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_030 sample=%lu dt=%lu status=%s\r\n",
              (unsigned long)now,
              (unsigned long)ctx->sample_count,
              (unsigned long)delta_ms,
              status);
}

static TestResult TC_030_Verify(TC030_Context* ctx, uint32_t now)
{
    uint32_t mean_delta;
    uint32_t jitter;

    if (ctx->completed)
    {
        return ctx->result;
    }

    if (ctx->sample_count < TC_030_REQUIRED_SAMPLES)
    {
        return TEST_IN_REVIEW;
    }

    mean_delta = ctx->sum_delta / ctx->sample_count;
    jitter = ctx->max_delta - ctx->min_delta;
    ctx->completed = 1U;
    ctx->result = (ctx->violation_count == 0U) ? TEST_PASS : TEST_FAIL;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_030 RESULT=%s samples=%lu mean=%lums min=%lums max=%lums jitter=%lums violations=%lu\r\n",
              (unsigned long)now,
              (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
              (unsigned long)ctx->sample_count,
              (unsigned long)mean_delta,
              (unsigned long)ctx->min_delta,
              (unsigned long)ctx->max_delta,
              (unsigned long)jitter,
              (unsigned long)ctx->violation_count);

    return ctx->result;
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
TestResult TC_030_TimingIntervalAccuracyValidation_Run(void)
{
    static TC030_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_030_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_030_Observe(&ctx, now);
    return TC_030_Verify(&ctx, now);
}
