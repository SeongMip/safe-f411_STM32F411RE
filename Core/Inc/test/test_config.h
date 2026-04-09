/****************************************************************************
 * @file    test_config.h
 * @brief   대표 TC 선택과 공통 시험 상수를 정의한다.
 *
 * @details
 * - Bare-metal/RTOS 실행 대상과 debounce 기준을 동일한 설정 파일에서 관리한다.
 *
 ****************************************************************************/

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
