/****************************************************************************
 * @file    tc_002_clock_sanity.c
 * @brief   시스템 tick 또는 주기 로그가 기대 시간 오차 내에 있는지 검증한다.
 *
 * @details
 * - clock 설정 이상이 다른 TC 결과를 왜곡하지 않도록 선행 확인한다.
 ****************************************************************************/

#include "tc_002_clock_sanity.h"
#include "log.h"
#include "platform_port.h"
#include "test_timer.h"

#define TC_002_INTERVAL_MS			1000U
#define TC_002_TOLERANCE_MIN_MS		950U
#define TC_002_TOLERANCE_MAX_MS		1050U
#define TC_002_REQUIRED_SAMPLES		10U

TestResult TC_002_ClockSanity_Run(void)
{
    static uint8_t started = 0;
    static uint8_t completed = 0;
    static TestTimer timer;
    static uint32_t last_tick_ms = 0;
    static uint32_t sample_count = 0;
    static uint32_t violation_count = 0;
    static uint8_t manual_logged = 0;
    uint32_t now = Platform_NowMs();

    if (!started)
    {
        started = 1;
        last_tick_ms = now;
        TestTimer_Start(&timer, now, TC_002_INTERVAL_MS);

        Log_Printf(LOG_LEVEL_INFO,
                    "[TC_002] NOTE: verify CubeMX Clock Configuration has no warnings/errors\r\n");
        Log_Printf(LOG_LEVEL_INFO,
                    "[TC_002] criterion: dt=%lu~%lums between adjacent [ms] TICK logs, samples=%u\r\n",
                    (unsigned long)TC_002_TOLERANCE_MIN_MS,
                    (unsigned long)TC_002_TOLERANCE_MAX_MS,
                    (unsigned int)TC_002_REQUIRED_SAMPLES);
        return TEST_IN_REVIEW;
    }


    if (!manual_logged)
    {
    	manual_logged = 1;
        Log_Raw("[TC-002] MANUAL VERIFICATION REQUIRED - observe TICK log\r\n");
    }

    if (completed)
    {
        return (violation_count == 0U) ? TEST_PASS : TEST_FAIL;
    }

    if (TestTimer_ExpiredAndReload(&timer, now, TC_002_INTERVAL_MS))
    {
        const char* status;
        uint32_t delta_ms = now - last_tick_ms;
        sample_count++;
        last_tick_ms = now;

        if ((delta_ms < TC_002_TOLERANCE_MIN_MS) || (delta_ms > TC_002_TOLERANCE_MAX_MS))
        {
            violation_count++;
            status = "OUT_OF_RANGE";
        }
        else
        {
            status = "OK";
        }

        Log_Printf(LOG_LEVEL_INFO,
                    "[TC_002] [ms=%lu] TICK sample=%lu dt=%lu status=%s\r\n",
                    (unsigned long)now,
                    (unsigned long)sample_count,
                    (unsigned long)delta_ms,
                    status);

        if (sample_count >= TC_002_REQUIRED_SAMPLES)
        {
            completed = 1;

            Log_Printf(LOG_LEVEL_INFO,
                        "[TC_002] [ms=%lu] AUTO_CHECK=%s samples=%lu violations=%lu\r\n",
                        (unsigned long)now,
                        (violation_count == 0U) ? "PASS" : "FAIL",
                        (unsigned long)sample_count,
                        (unsigned long)violation_count);
            return (violation_count == 0U) ? TEST_PASS : TEST_FAIL;
        }
    }
    
    return TEST_IN_REVIEW;
}
