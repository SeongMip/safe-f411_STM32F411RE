/****************************************************************************
 * @file    platform_gpio.c
 * @brief   LED와 버튼에 대한 보드 종속 GPIO 접근을 캡슐화한다.
 *
 * @details
 * - 상위 로직이 STM32 HAL 세부사항에 직접 의존하지 않도록 분리한다.
 * - NUCLEO-F411RE 보드의 LED/버튼 해석 규칙을 이 계층에 모은다.
 ****************************************************************************/

#include "platform_gpio.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "main.h"

#define PLATFORM_BUTTON_GPIO_PORT GPIOC
#define PLATFORM_BUTTON_PIN       GPIO_PIN_13

void Platform_LedToggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

PlatformLedState Platform_LedRead(void)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin);
    return (state == GPIO_PIN_SET) ? PLATFORM_LED_ON : PLATFORM_LED_OFF;
}

PlatformButtonRawState Platform_ButtonReadRaw(void)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(PLATFORM_BUTTON_GPIO_PORT, PLATFORM_BUTTON_PIN);
    return (state == GPIO_PIN_RESET) ? PLATFORM_BUTTON_DOWN : PLATFORM_BUTTON_UP;
}
