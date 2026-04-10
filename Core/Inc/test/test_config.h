#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include "main.h"
#include "test_baremetal_runner.h"
#include "test_rtos_runner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_UART_TIMEOUT_MS	100

#define TEST_DEBOUNCE_MS		20
#define TEST_RELEASE_MS			20

#ifndef TEST_BAREMETAL_TARGET
#define TEST_BAREMETAL_TARGET RUN_TC_060
#endif

#ifndef TEST_RTOS_TARGET
#define TEST_RTOS_TARGET RUN_TC_063_RTOS
#endif

#ifdef __cplusplus
}
#endif

#endif
