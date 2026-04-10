/****************************************************************************
 * @file    tc_042_rtos.c
 * @brief   RTOS 구조에서 task/queue/probe/watchdog 관련 동작을 검증한다.
 *
 * @details
 * - bare-metal에서 드러난 한계를 RTOS 구조가 어떻게 완화하는지 확인한다.
 * - queue 지연, logger 누락, watchdog feed, safe-state 경로를 관찰한다.
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

/**
 * @brief   logger queue가 관찰 시간 안에 모두 처리되는지 확인한다.
 *
 * @param   ctx  처리량과 queue 상태를 저장하는 관찰 컨텍스트
 *
 * @return  TEST_FAIL 또는 TEST_IN_REVIEW
 *
 * @details
 * - 지정 시간 안에 processed가 목표 수에 도달하고 pending이 0이 되면 다음 판정 단계로 넘긴다.
 * - timeout까지 queue가 비워지지 않으면 logger 누락 또는 지연으로 본다.
 */
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

/**
 * @brief   logger 통계가 기대 조건과 일치하는지 최종 판정한다.
 *
 * @param   ctx  처리량과 queue 상태를 저장한 관찰 컨텍스트
 *
 * @return  TEST_PASS 또는 TEST_FAIL
 *
 * @details
 * - dropped가 0이어야 하며 processed는 목표 개수와 일치해야 한다.
 * - pending이 남아 있으면 queue drain 실패로 FAIL 처리한다.
 */
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
 * @brief   TC-042 RTOS logger omission validation을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL
 *
 * @details
 * - logger queue에 8개 메시지를 밀어 넣고 timeout 내 처리 여부를 관찰한다.
 * - dropped, processed, pending 통계를 이용해 누락 여부를 판정한다.
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
