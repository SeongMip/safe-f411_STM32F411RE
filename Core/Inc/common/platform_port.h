/****************************************************************************
 * @file    platform_port.h
 * @brief   시간/지연/UART 포팅 계층 API를 선언한다.
 *
 ****************************************************************************/

#ifndef PLATFORM_PORT_H
#define PLATFORM_PORT_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

UART_HandleTypeDef* Platform_Uart(void);
uint32_t Platform_NowMs(void);
void Platform_DelayMs(uint32_t ms);
uint32_t Platform_TaskNowMs(void);
void Platform_TaskDelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
