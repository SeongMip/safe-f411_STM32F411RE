/****************************************************************************
 * @file    tc_032_cpu_load_impact_on_button_detection.c
 * @brief   타이밍 정확도, debounce 안정성, 부하 영향 등을 검증한다.
 *
 * @details
 * - 기능 동작 여부보다 시간/부하/노이즈 조건에서의 일관성을 보는 그룹이다.
 ****************************************************************************/

#include "tc_032_cpu_load_impact_on_button_detection.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_032_CLICK_TARGET        10U
#define TC_032_LONG_MS           2000U
#define TC_032_ALLOWED_DELAY_MS    50U
#define TC_032_TEST_TIMEOUT_MS  60000U
#define TC_032_CPU_BUSY_LOOP    50000

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    uint32_t start_ms;
    uint32_t click_count;
    uint32_t long_count;
    uint32_t delay_violation_count;
    uint32_t max_click_delay_ms;
    PlatformButtonRawState prev_raw;
    uint32_t release_edge_ms;
    uint8_t release_edge_armed;
    TestResult result;
} TC032_Context;

static void TC_032_Setup(TC032_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_032_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->start_ms = now;
    ctx->click_count = 0U;
    ctx->long_count = 0U;
    ctx->delay_violation_count = 0U;
    ctx->max_click_delay_ms = 0U;
    ctx->prev_raw = Platform_ButtonReadRaw();
    ctx->release_edge_ms = 0U;
    ctx->release_edge_armed = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_032 START load=busy_loop+delay(10) target_click=%u allowed_delay=%ums\r\n",
              (unsigned long)now,
              (unsigned int)TC_032_CLICK_TARGET,
              (unsigned int)TC_032_ALLOWED_DELAY_MS);
}

static void TC_032_Stimulus(TC032_Context* ctx)
{
    volatile int i;
    (void)ctx;

    for (i = 0; i < TC_032_CPU_BUSY_LOOP; i++)
    {
    }

    Platform_DelayMs(10U);
}

static void TC_032_ObserveReleaseEdge(TC032_Context* ctx, uint32_t now)
{
    PlatformButtonRawState raw = Platform_ButtonReadRaw();

    if ((ctx->prev_raw == PLATFORM_BUTTON_DOWN) && (raw == PLATFORM_BUTTON_UP))
    {
        ctx->release_edge_ms = now;
        ctx->release_edge_armed = 1U;
    }

    ctx->prev_raw = raw;
}

static void TC_032_HandleClick(TC032_Context* ctx, uint32_t now)
{
    uint32_t click_delay_ms = 0U;

    if (ctx->release_edge_armed)
    {
        click_delay_ms = now - ctx->release_edge_ms;
        ctx->release_edge_armed = 0U;
    }

    if (click_delay_ms > ctx->max_click_delay_ms)
    {
        ctx->max_click_delay_ms = click_delay_ms;
    }

    if (click_delay_ms > TC_032_ALLOWED_DELAY_MS)
    {
        ctx->delay_violation_count++;
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_032 WARN click_delay=%lums over_limit=%ums\r\n",
                  (unsigned long)now,
                  (unsigned long)click_delay_ms,
                  (unsigned int)TC_032_ALLOWED_DELAY_MS);
    }

    ctx->click_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_032 EVT=BTN_CLICK LED=TOGGLE count=%lu/%lu delay=%lums\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned long)TC_032_CLICK_TARGET,
              (unsigned long)click_delay_ms);

    if (ctx->click_count >= TC_032_CLICK_TARGET)
    {
        ctx->completed = 1U;
        ctx->result = ((ctx->delay_violation_count == 0U) && (ctx->long_count == 0U))
                    ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_032 RESULT=%s click=%lu long=%lu max_delay=%lums delay_violation=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->click_count,
                  (unsigned long)ctx->long_count,
                  (unsigned long)ctx->max_click_delay_ms,
                  (unsigned long)ctx->delay_violation_count);
    }
}

static void TC_032_HandleLong(TC032_Context* ctx, uint32_t now)
{
    ctx->long_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_032 WARN EVT=BTN_LONG LED=TOGGLE\r\n",
              (unsigned long)now);
}

static void TC_032_Observe(TC032_Context* ctx, uint32_t now)
{
    BtnEvent evt;

    TC_032_ObserveReleaseEdge(ctx, now);
    evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_032_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_032_HandleLong(ctx, now);
    }
}

static TestResult TC_032_Verify(TC032_Context* ctx, uint32_t now)
{
    uint32_t miss_count;

    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_032_TEST_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    miss_count = (ctx->click_count < TC_032_CLICK_TARGET)
               ? (TC_032_CLICK_TARGET - ctx->click_count)
               : 0U;

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_032 RESULT=FAIL reason=TIMEOUT click=%lu miss=%lu long=%lu max_delay=%lums delay_violation=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->click_count,
              (unsigned long)miss_count,
              (unsigned long)ctx->long_count,
              (unsigned long)ctx->max_click_delay_ms,
              (unsigned long)ctx->delay_violation_count);

    return ctx->result;
}

TestResult TC_032_CpuLoadImpactOnButtonDetection_Run(void)
{
    static TC032_Context ctx;
    uint32_t now;

    if (!ctx.inited)
    {
        TC_032_Setup(&ctx, Platform_NowMs());
        return TEST_IN_REVIEW;
    }

    TC_032_Stimulus(&ctx);
    now = Platform_NowMs();
    TC_032_Observe(&ctx, now);
    return TC_032_Verify(&ctx, now);
}
