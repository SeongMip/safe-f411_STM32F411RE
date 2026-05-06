#ifndef TEST_RUNTIME_TEST_TIMER_H
#define TEST_RUNTIME_TEST_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t next_ms;
    uint8_t  started;
} TestTimer;

void TestTimer_Reset(TestTimer* t);
void TestTimer_Start(TestTimer* t, uint32_t now, uint32_t interval_ms);
int TestTimer_ExpiredAndReload(TestTimer* t, uint32_t now, uint32_t interval_ms);

#ifdef __cplusplus
}
#endif

#endif
