/****************************************************************************
 * @file    tc_042_rtos.c
 * @brief   RTOS logger queue 처리량과 누락 여부를 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_042_rtos.h"
#include "log.h"
#include "platform_port.h"
#include "rtos_log_service.h"

#define TC_042_RTOS_PUSH_COUNT  8U
#define TC_042_RTOS_TIMEOUT_MS 1500U

typedef struct
{
    uint32_t start_ms;
    uint32_t processed;
    uint32_t dropped;
    uint32_t pending;
} TC042Rtos_Context;

static const char *kTc042RtosMsgs[TC_042_RTOS_PUSH_COUNT] =
{
    "TC_042_RTOS msg_00",
    "TC_042_RTOS msg_01",
    "TC_042_RTOS msg_02",
    "TC_042_RTOS msg_03",
    "TC_042_RTOS msg_04",
    "TC_042_RTOS msg_05",
    "TC_042_RTOS msg_06",
    "TC_042_RTOS msg_07"
};

static void TC_042_RTOS_Setup(void)
{
    RtosLogService_ResetStats();

    Log_Printf(LOG_LEVEL_INFO,
              "[TC_042_RTOS] START logger omission validation under RTOS\r\n");
}

static void TC_042_RTOS_Stimulus(void)
{
    uint32_t i;

    for (i = 0U; i < TC_042_RTOS_PUSH_COUNT; i++)
    {
        RtosLogService_Push(kTc042RtosMsgs[i]);
    }
}

static TestResult TC_042_RTOS_Observe(TC042Rtos_Context* ctx)
{
    ctx->start_ms = Platform_NowMs();

    for (;;)
    {
        ctx->processed = RtosLogService_GetProcessedCount();
        ctx->dropped = RtosLogService_GetDroppedCount();
        ctx->pending = RtosLogService_GetPendingCount();

        if ((ctx->processed >= TC_042_RTOS_PUSH_COUNT) && (ctx->pending == 0U))
        {
            return TEST_IN_REVIEW;
        }

        if ((Platform_NowMs() - ctx->start_ms) >= TC_042_RTOS_TIMEOUT_MS)
        {
            Log_Printf(LOG_LEVEL_ERROR,
                      "[TC_042_RTOS] timeout processed=%lu dropped=%lu pending=%lu\r\n",
                      (unsigned long)ctx->processed,
                      (unsigned long)ctx->dropped,
                      (unsigned long)ctx->pending);
            return TEST_FAIL;
        }

        Platform_DelayMs(20U);
    }
}

static TestResult TC_042_RTOS_Verify(TC042Rtos_Context* ctx)
{
    ctx->processed = RtosLogService_GetProcessedCount();
    ctx->dropped = RtosLogService_GetDroppedCount();
    ctx->pending = RtosLogService_GetPendingCount();

    Log_Printf(LOG_LEVEL_INFO,
              "[TC_042_RTOS] processed=%lu dropped=%lu pending=%lu\r\n",
              (unsigned long)ctx->processed,
              (unsigned long)ctx->dropped,
              (unsigned long)ctx->pending);

    if (ctx->dropped != 0U)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_042_RTOS] logger dropped messages\r\n");
        return TEST_FAIL;
    }

    if (ctx->processed != TC_042_RTOS_PUSH_COUNT)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_042_RTOS] processed mismatch expected=%u actual=%lu\r\n",
                  (unsigned)TC_042_RTOS_PUSH_COUNT,
                  (unsigned long)ctx->processed);
        return TEST_FAIL;
    }

    if (ctx->pending != 0U)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TC_042_RTOS] logger queue not drained\r\n");
        return TEST_FAIL;
    }

    return TEST_PASS;
}


/**
 * @brief   TC-042-RTOS logger queue 처리량과 누락 여부를 검증한다.
 *
 * @return  TEST_PASS / TEST_FAIL
 *
 * @details
 * - push된 메시지 수와 processed/dropped/pending 통계를 비교해 logger 경로 건전성을 본다.
 */
TestResult TC_042_Rtos_Run(void)
{
    TC042Rtos_Context ctx;
    TestResult observe_result;

    TC_042_RTOS_Setup();
    TC_042_RTOS_Stimulus();
    observe_result = TC_042_RTOS_Observe(&ctx);
    if (observe_result == TEST_FAIL)
    {
        return TEST_FAIL;
    }

    return TC_042_RTOS_Verify(&ctx);
}
