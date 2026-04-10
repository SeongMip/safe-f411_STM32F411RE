/****************************************************************************
 * @file    test_timer.c
 * @brief   테스트용 시간 경과 판정 유틸리티를 제공한다.
 *
 * @details
 * - polling 기반 테스트에서 timeout과 주기 판정을 공통화한다.
 * - 시간 비교 기준을 통일해 TC별 구현 편차를 줄인다.
 ****************************************************************************/

#include "test_timer.h"

void TestTimer_Reset(TestTimer* t)
{
    t->next_ms = 0U;
    t->started = 0;
}

void TestTimer_Start(TestTimer* t, uint32_t now, uint32_t interval_ms)
{
    t->next_ms = now + interval_ms;
    t->started = 1;
}

int TestTimer_ExpiredAndReload(TestTimer* t, uint32_t now, uint32_t interval_ms)
{
    if (!t->started)
    {
        TestTimer_Start(t, now, interval_ms);
        return 0;
    }

    if ((int32_t)(now - t->next_ms) >= 0)
    {
        t->next_ms += interval_ms;
        return 1;
    }

    return 0;
}
