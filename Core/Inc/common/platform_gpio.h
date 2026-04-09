/****************************************************************************
 * @file    platform_gpio.h
 * @brief   보드 종속 LED/버튼 GPIO 접근 API를 선언한다.
 *
 ****************************************************************************/

#ifndef PLATFORM_GPIO_H
#define PLATFORM_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PLATFORM_LED_OFF = 0,
    PLATFORM_LED_ON  = 1
} PlatformLedState;

typedef enum
{
    PLATFORM_BUTTON_UP = 0,
    PLATFORM_BUTTON_DOWN = 1
} PlatformButtonRawState;

void Platform_LedToggle(void);
PlatformLedState Platform_LedRead(void);
PlatformButtonRawState Platform_ButtonReadRaw(void);

#ifdef __cplusplus
}
#endif

#endif
