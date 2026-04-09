/****************************************************************************
 * @file    tc_052_uart_event_mapping_fix_validation.c
 * @brief   UART 이벤트 매핑 수정 후 로그 일관성을 검증한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_052_uart_event_mapping_fix_validation.h"
#include "log.h"
#include "platform_port.h"
#include "platform_gpio.h"
#include "button_fsm.h"
#include "test_config.h"

#define TC_052_CLICK_TARGET  10U
#define TC_052_LONG_MS     2000U
#define TC_052_TIMEOUT_MS 30000U

typedef struct
{
    uint8_t inited;
    uint8_t completed;
    ButtonFsm btn;
    uint32_t start_ms;
    uint32_t led_count;
    uint32_t uart_count;
    uint32_t long_count;
    TestResult result;
} TC052_Context;

static void TC_052_Setup(TC052_Context* ctx, uint32_t now)
{
    ButtonFsm_Init(&ctx->btn, TEST_DEBOUNCE_MS, TEST_RELEASE_MS, TC_052_LONG_MS);
    ctx->inited = 1U;
    ctx->completed = 0U;
    ctx->start_ms = now;
    ctx->led_count = 0U;
    ctx->uart_count = 0U;
    ctx->long_count = 0U;
    ctx->result = TEST_IN_REVIEW;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_052 START target_click=%u\r\n",
              (unsigned long)now,
              (unsigned int)TC_052_CLICK_TARGET);
    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_052 ACTION: Click button exactly %u times\r\n",
              (unsigned long)now,
              (unsigned int)TC_052_CLICK_TARGET);
}

static void TC_052_HandleClick(TC052_Context* ctx, uint32_t now)
{
    ctx->led_count++;
    Platform_LedToggle();
    ctx->uart_count++;

    Log_Printf(LOG_LEVEL_INFO,
              "[ms=%lu] TC_052 EVT=BTN_CLICK LED=TOGGLE led=%lu uart=%lu/%u\r\n",
              (unsigned long)now,
              (unsigned long)ctx->led_count,
              (unsigned long)ctx->uart_count,
              (unsigned int)TC_052_CLICK_TARGET);

    if (ctx->uart_count >= TC_052_CLICK_TARGET)
    {
        ctx->completed = 1U;
        ctx->result = ((ctx->led_count == TC_052_CLICK_TARGET) &&
                       (ctx->uart_count == TC_052_CLICK_TARGET) &&
                       (ctx->long_count == 0U)) ? TEST_PASS : TEST_FAIL;

        Log_Printf(LOG_LEVEL_INFO,
                  "[ms=%lu] TC_052 RESULT=%s led=%lu uart=%lu long=%lu\r\n",
                  (unsigned long)now,
                  (ctx->result == TEST_PASS) ? "PASS" : "FAIL",
                  (unsigned long)ctx->led_count,
                  (unsigned long)ctx->uart_count,
                  (unsigned long)ctx->long_count);
    }
}

static void TC_052_HandleLong(TC052_Context* ctx, uint32_t now)
{
    ctx->long_count++;
    Platform_LedToggle();

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_052 WARN EVT=BTN_LONG (unexpected)\r\n",
              (unsigned long)now);
}

static void TC_052_Observe(TC052_Context* ctx, uint32_t now)
{
    BtnEvent evt = ButtonFsm_Update(&ctx->btn, now);

    if (evt == BTN_EVT_CLICK)
    {
        TC_052_HandleClick(ctx, now);
    }
    else if (evt == BTN_EVT_LONG)
    {
        TC_052_HandleLong(ctx, now);
    }
}

static TestResult TC_052_Verify(TC052_Context* ctx, uint32_t now)
{
    if (ctx->completed)
    {
        return ctx->result;
    }

    if ((now - ctx->start_ms) <= TC_052_TIMEOUT_MS)
    {
        return TEST_IN_REVIEW;
    }

    ctx->completed = 1U;
    ctx->result = TEST_FAIL;

    Log_Printf(LOG_LEVEL_WARN,
              "[ms=%lu] TC_052 RESULT=FAIL reason=TIMEOUT led=%lu uart=%lu long=%lu\r\n",
              (unsigned long)now,
              (unsigned long)ctx->led_count,
              (unsigned long)ctx->uart_count,
              (unsigned long)ctx->long_count);

    return ctx->result;
}


/**
 * @brief   TC-052의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_052_UartEventMappingFixValidation_Run(void)
{
    static TC052_Context ctx;
    uint32_t now = Platform_NowMs();

    if (!ctx.inited)
    {
        TC_052_Setup(&ctx, now);
        return TEST_IN_REVIEW;
    }

    TC_052_Observe(&ctx, now);
    return TC_052_Verify(&ctx, now);
}
