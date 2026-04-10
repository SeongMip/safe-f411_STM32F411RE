#ifndef TOGGLE_VERIFIER_H
#define TOGGLE_VERIFIER_H

#include <stdint.h>
#include "test_result.h"
#include "platform_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t inited;
    uint32_t count;
    uint32_t target_count;
    PlatformLedState expected_next_led_state;
    const char* tc_id;
    const char* expected_event_name;
} TestToggleVerifier;

void TestToggleVerifier_Init(TestToggleVerifier* verifier,
                             const char* tc_id,
                             const char* expected_event_name,
                             uint32_t target_count,
                             PlatformLedState initial_led_state);

TestResult TestToggleVerifier_OnExpectedEvent(TestToggleVerifier* verifier,
                                              PlatformLedState actual_led_state);

TestResult TestToggleVerifier_OnUnexpectedEvent(TestToggleVerifier* verifier,
                                                const char* actual_event_name);

#ifdef __cplusplus
}
#endif

#endif
