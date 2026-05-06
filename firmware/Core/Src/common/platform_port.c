/****************************************************************************
 * @file    platform_port.c
 * @brief   시간, delay, UART 등 플랫폼 포팅 인터페이스를 제공한다.
 *
 * @details
 * - 테스트 코드와 런타임 코드가 보드 의존 API를 직접 호출하지 않도록 중간 계층으로 둔다.
 * - 타이밍 관찰과 로그 출력에서 공통 기준을 제공한다.
 ****************************************************************************/

#include "platform_port.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

static uint8_t Platform_IsSchedulerRunning(void)
{
#if (INCLUDE_xTaskGetSchedulerState == 1)
    return (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) ? 1U : 0U;
#else
    return 0U;
#endif
}

UART_HandleTypeDef* Platform_Uart(void)
{
    return &huart2;
}

uint32_t Platform_NowMs(void)
{
    return HAL_GetTick();
}

void Platform_DelayMs(uint32_t ms)
{
    if (Platform_IsSchedulerRunning())
    {
        vTaskDelay(pdMS_TO_TICKS(ms));
        return;
    }

    HAL_Delay(ms);
}

uint32_t Platform_TaskNowMs(void)
{
    if (Platform_IsSchedulerRunning())
    {
        return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    return HAL_GetTick();
}

void Platform_TaskDelayMs(uint32_t ms)
{
    if (Platform_IsSchedulerRunning())
    {
        vTaskDelay(pdMS_TO_TICKS(ms));
        return;
    }

    HAL_Delay(ms);
}
