/****************************************************************************
 * @file    tc_064_power_cycle_recovery_validation.c
 * @brief   power cycle 이후 초기화와 복구 동작을 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_064_power_cycle_recovery_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_064_HEARTBEAT_MS       200U
#define TC_064_VERIFY_DURATION_MS 3000U

typedef struct
{
    uint8_t inited;
    uint32_t start_ms;
    uint32_t last_toggle_ms;
    uint32_t toggle_count;
} TC064_Context;

static void TC_064_Setup(TC064_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->start_ms = now;
    ctx->last_toggle_ms = now;
    ctx->toggle_count = 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 BOOT_OK verify_duration=%u_ms\r\n",
              (unsigned long)now,
              (unsigned int)TC_064_VERIFY_DURATION_MS);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 ACTION: Observe LED heartbeat for 3 seconds, then power-cycle\r\n",
              (unsigned long)now);
}

static void TC_064_Stimulus(TC064_Context* ctx, uint32_t now)
{
    if ((now - ctx->last_toggle_ms) < TC_064_HEARTBEAT_MS)
    {
        return;
    }

    ctx->last_toggle_ms = now;
    ctx->toggle_count++;
    Platform_LedToggle();
}

static TestResult TC_064_Verify(TC064_Context* ctx, uint32_t now)
{
    uint32_t expected_toggles;
    uint8_t pass;

    if ((now - ctx->start_ms) < TC_064_VERIFY_DURATION_MS)
    {
        return TEST_IN_REVIEW;
    }

    expected_toggles = TC_064_VERIFY_DURATION_MS / TC_064_HEARTBEAT_MS;
    pass = (ctx->toggle_count >= (expected_toggles - 2U)) ? 1U : 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 RESULT=%s toggles=%lu expected>=%lu\r\n",
              (unsigned long)now,
              pass ? "PASS" : "FAIL",
              (unsigned long)ctx->toggle_count,
              (unsigned long)(expected_toggles - 2U));
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_064 INFO: Power-cycle the board now. Repeat 10 times total.\r\n",
              (unsigned long)now);

    return pass ? TEST_PASS : TEST_FAIL;
}


/**
 * @brief   TC-064의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_064_PowerCycleRecoveryValidation_Run(void)
{
    static TC064_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_064_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_064_Stimulus(&ctx, now);
    return TC_064_Verify(&ctx, now);
}
