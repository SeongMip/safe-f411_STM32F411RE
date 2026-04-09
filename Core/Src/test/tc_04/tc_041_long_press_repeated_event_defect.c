/****************************************************************************
 * @file    tc_041_long_press_repeated_event_defect.c
 * @brief   long-press 중복 발행 결함 시나리오를 분리 관찰한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_041_long_press_repeated_event_defect.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"

#define TC_041_SETTLE_MS    3000U
#define TC_041_TIMEOUT_MS  15000U

typedef struct
{
    uint8_t inited;
    uint8_t hold_started;
    uint32_t start_ms;
    uint32_t toggle_count;
    uint32_t last_toggle_ms;
} TC041_Context;

static void TC_041_Setup(TC041_Context* ctx, uint32_t now)
{
    ctx->inited = 1U;
    ctx->hold_started = 0U;
    ctx->start_ms = now;
    ctx->toggle_count = 0U;
    ctx->last_toggle_ms = 0U;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_041 START bug=no_release_detection\r\n",
              (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_041 ACTION: Hold button for 2 seconds\r\n",
              (unsigned long)now);
}

static void TC_041_Stimulus(TC041_Context* ctx, uint32_t now)
{
    if (Platform_ButtonReadRaw() != PLATFORM_BUTTON_DOWN)
    {
        return;
    }

    ctx->toggle_count++;
    ctx->last_toggle_ms = now;
    Platform_LedToggle();

    if (!ctx->hold_started)
    {
        ctx->hold_started = 1U;
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_041 HOLD_START\r\n",
                  (unsigned long)now);
    }

    if ((ctx->toggle_count % 100U) == 0U)
    {
        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_041 rapid_toggle count=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->toggle_count);
    }
}

static TestResult TC_041_Verify(TC041_Context* ctx, uint32_t now)
{
    if (ctx->hold_started && (ctx->toggle_count > 0U) && ((now - ctx->last_toggle_ms) > TC_041_SETTLE_MS))
    {
        if (ctx->toggle_count > 1U)
        {
            Log_Printf(LOG_LEVEL_INFO,
                      "[ms=%lu] TC_041 RESULT=PASS defect_reproduced toggle_count=%lu\r\n",
                      (unsigned long)now,
                      (unsigned long)ctx->toggle_count);
            return TEST_PASS;
        }

        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_041 RESULT=FAIL defect_not_reproduced toggle_count=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->toggle_count);
        return TEST_FAIL;
    }

    if ((now - ctx->start_ms) > TC_041_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_WARN,
                  "[ms=%lu] TC_041 RESULT=FAIL reason=TIMEOUT toggle_count=%lu\r\n",
                  (unsigned long)now,
                  (unsigned long)ctx->toggle_count);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}


/**
 * @brief   TC-041의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_041_LongPressRepeatedEventDefect_Run(void)
{
    static TC041_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_041_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_041_Stimulus(&ctx, now);
    return TC_041_Verify(&ctx, now);
}
