/****************************************************************************
 * @file    tc_021_boot_log_format.c
 * @brief   부팅 로그가 출력되는지 수동으로 확인할 수 있도록 안내 로그를 남긴다.
 *
 * @details
 * - 부트 직후 로그 존재 여부와 형식은 사람이 직접 확인하는 항목이다.
 * - 자동 판정이 없는 상태에서는 TEST_IN_REVIEW로 유지해 가짜 PASS를 막는다.
 ****************************************************************************/

#include "tc_021_boot_log_format.h"
#include "log.h"
#include "platform_port.h"
#include "test_timer.h"

#define TC_021_MANUAL_NOTICE_MS 3000U

typedef struct
{
    uint8_t initialized;
    uint8_t manual_notice_logged;
    TestTimer manual_notice_timer;
} TC021Context;

static void TC_021_Setup(TC021Context* ctx, uint32_t now)
{
    if (ctx->initialized)
    {
        return;
    }

    TestTimer_Reset(&ctx->manual_notice_timer);

    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_021 START boot log verification\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_021 OBSERVE: confirm boot log is printed after reset\r\n",
               (unsigned long)now);
    Log_Printf(LOG_LEVEL_INFO,
               "[ms=%lu] TC_021 EXPECT: boot log appears once and format is valid\r\n",
               (unsigned long)now);

    /* reset 직후 부트로그가 실제로 출력되는지 사람이 확인할 수 있게 남긴다. */
    Log_Printf(LOG_LEVEL_INFO,
               "BOOT ms=%lu\r\n",
               (unsigned long)now);

    ctx->initialized = 1U;
}

TestResult TC_021_BootLogFormat_Run(void)
{
    static TC021Context ctx = {0};
    uint32_t now = Platform_NowMs();

    TC_021_Setup(&ctx, now);

    /* 자동 판정이 없는 TC이므로 일정 시간 후 수동 확인 필요 상태를 명확히 남긴다. */
    if ((!ctx.manual_notice_logged) &&
        TestTimer_ExpiredAndReload(&ctx.manual_notice_timer,
                                   now,
                                   TC_021_MANUAL_NOTICE_MS))
    {
        Log_Printf(LOG_LEVEL_WARN,
                   "[ms=%lu] TC_021 MANUAL VERIFICATION REQUIRED\r\n",
                   (unsigned long)now);
        ctx.manual_notice_logged = 1U;
    }

    return TEST_IN_REVIEW;
}
