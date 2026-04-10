/****************************************************************************
 * @file    tc_001_debug_attach_reset.c
 * @brief   debug attach 시 reset/boot 로그 영향 여부를 관찰한다.
 *
 * @details
 * - 디버거 연결 환경에서 나타나는 중복 boot 로그나 초기화 재실행을 분리 관찰한다.
 * - 기능 결함과 디버그 환경 영향을 혼동하지 않도록 하기 위한 TC이다.
 ****************************************************************************/

#include "tc_001_debug_attach_reset.h"
#include "log.h"
#include "platform_port.h"
#include "test_timer.h"

#define TC_001_HEARTBEAT_INTERVAL_MS   1000U
#define TC_001_REQUIRED_HEARTBEATS        3U
#define TC_001_START_TICK_MAX_MS        500U

TestResult TC_001_DebugAttachReset_Run(void)
{
    static uint8_t started = 0;
    static uint8_t completed = 0;
    static TestTimer timer;
    static uint32_t first_tick_ms = 0;
    static uint32_t heartbeat_sample = 0;
    static uint8_t manual_logged = 0;

    uint32_t now = Platform_NowMs();

    if (!started)
    {
        started = 1;
        first_tick_ms = now;
        TestTimer_Start(&timer, now, TC_001_HEARTBEAT_INTERVAL_MS);

        Log_Printf(LOG_LEVEL_INFO,
                    "[TC-001] START first_tick=%lu\r\n",
                    (unsigned long)first_tick_ms);
        Log_Printf(LOG_LEVEL_INFO,
                    "[TC-001] NOTE duplicate BOOT logs during debug attach can be environment-related\r\n");

        return TEST_IN_REVIEW;
    }

    if (!manual_logged)
    {
    	manual_logged = 1;
    	Log_Raw("[TC-001] MANUAL VERIFICATION REQUIRED - observe Boot log\r\n");
    }

    if (completed)
    {
        return (first_tick_ms <= TC_001_START_TICK_MAX_MS) ? TEST_PASS : TEST_FAIL;
    }

    if (TestTimer_ExpiredAndReload(&timer, now, TC_001_HEARTBEAT_INTERVAL_MS))
    {
        heartbeat_sample++;
        Log_Printf(LOG_LEVEL_INFO,
                    "ATTACHED_ALIVE sample=%lu now=%lu\r\n",
                    (unsigned long)heartbeat_sample,
                    (unsigned long)now);

        if (heartbeat_sample >= TC_001_REQUIRED_HEARTBEATS)
        {
            completed = 1;
            Log_Printf(LOG_LEVEL_INFO,
            			"[TC-001] AUTO_CHECK=%s first_tick=%lu criterion=first_tick<=%lu && attached_alive final=%s\r\n",
                        (first_tick_ms <= TC_001_START_TICK_MAX_MS) ? "PASS" : "FAIL",
                        (unsigned long)first_tick_ms,
                        (unsigned long)TC_001_START_TICK_MAX_MS,
            			(first_tick_ms <= TC_001_START_TICK_MAX_MS) ? "PASS" : "FAIL");
            return (first_tick_ms <= TC_001_START_TICK_MAX_MS) ? TEST_PASS: TEST_FAIL;
        }
    }

    return TEST_IN_REVIEW;
}
