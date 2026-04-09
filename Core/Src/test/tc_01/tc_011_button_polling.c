/****************************************************************************
 * @file    tc_011_button_polling.c
 * @brief   polling 기반 버튼 입력 감지가 일관되게 동작하는지 확인한다.
 *
 * @details
 * - 시험 절차보다 관찰 포인트와 PASS/FAIL 판정 기준 설명을 우선한다.
 *
 ****************************************************************************/

#include "tc_011_button_polling.h"
#include "log.h"
#include "button_fsm.h"
#include "platform_port.h"

#define TC_011_TIMEOUT_MS  10000U


/**
 * @brief   TC-011의 최종 PASS/FAIL 판정을 수행한다.
 *
 * @return  TEST_PASS / TEST_FAIL / TEST_IN_REVIEW
 *
 * @details
 * - 이 TC는 파일 상단에 정의된 관찰 포인트를 기준으로 결과를 반환한다.
 * - TEST_IN_REVIEW는 시간 경과 후 재평가가 필요한 상태를 의미한다.
 */
TestResult TC_011_ButtonPolling_Run(void)
{
    static uint8_t started = 0;
    static uint8_t down_detected = 0;
    static uint8_t up_detected = 0;
    static uint32_t start_ms = 0;
    static BtnLevel last_level = BTN_LEVEL_UP;

    uint32_t now = Platform_NowMs();
    BtnLevel current_level = ButtonFsm_Level();

    if (!started)
    {
        started = 1;
        start_ms = now;
        last_level = current_level;

        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] START button polling verification\r\n");
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] ACTION press and release the user button once within %lu ms\r\n",
                   (unsigned long)TC_011_TIMEOUT_MS);
    }

    if ((last_level == BTN_LEVEL_UP) && (current_level == BTN_LEVEL_DOWN))
    {
        down_detected = 1;
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] OBSERVE button=DOWN\r\n");
    }
    else if ((last_level == BTN_LEVEL_DOWN) && (current_level == BTN_LEVEL_UP))
    {
        up_detected = 1;
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] OBSERVE button=UP\r\n");
    }

    last_level = current_level;

    if ((down_detected != 0U) && (up_detected != 0U))
    {
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] AUTO_CHECK=PASS down_detected=1 up_detected=1\r\n");
        return TEST_PASS;
    }

    if ((int32_t)(now - start_ms) >= (int32_t)TC_011_TIMEOUT_MS)
    {
        Log_Printf(LOG_LEVEL_INFO,
                   "[TC_011] AUTO_CHECK=FAIL timeout=%lu down_detected=%u up_detected=%u\r\n",
                   (unsigned long)TC_011_TIMEOUT_MS,
                   (unsigned int)down_detected,
                   (unsigned int)up_detected);
        return TEST_FAIL;
    }

    return TEST_IN_REVIEW;
}
