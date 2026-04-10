/****************************************************************************
 * @file    tc_030_timing_interval_accuracy_validation.c
 * @brief   타이밍 정확도, debounce 안정성, 부하 영향 등을 검증한다.
 *
 * @details
 * - 기능 동작 여부보다 시간/부하/노이즈 조건에서의 일관성을 보는 그룹이다.
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
