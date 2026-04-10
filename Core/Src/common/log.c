/****************************************************************************
 * @file    log.c
 * @brief   공통 로그 출력 형식과 UART 전송 경로를 제공한다.
 *
 * @details
 * - 시험 로그의 형식 일관성을 유지하기 위한 공통 계층이다.
 * - bare-metal과 RTOS에서 동일한 로그 표현을 유지하는 것이 목적이다.
 * - 출력 자체보다 로그 레벨과 메시지 규약 유지가 중요하다.
 ****************************************************************************/

#include "log.h"
#include "platform_port.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LOG_BUF_SIZE 128
#define LOG_UART_TIMEOUT_MS 100U

static const char* Log_LevelPrefix(LogLevel level)
{
    switch (level)
    {
    case LOG_LEVEL_INFO:  return "[INFO] ";
    case LOG_LEVEL_WARN:  return "[WARN] ";
    case LOG_LEVEL_ERROR: return "[ERR ] ";
    default:              return "[UNKN] ";
    }
}

void Log_Init(void)
{
}

void Log_Raw(const char* str)
{
    UART_HandleTypeDef* uart;

    if (str == NULL)
    {
        return;
    }

    uart = Platform_Uart();
    HAL_UART_Transmit(uart,
                      (uint8_t*)str,
                      (uint16_t)strlen(str),
                      LOG_UART_TIMEOUT_MS);
}

void Log_Write(LogLevel level, const char* str)
{
    if (str == NULL)
    {
        return;
    }

    Log_Raw(Log_LevelPrefix(level));
    Log_Raw(str);
}

void Log_Printf(LogLevel level, const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len >= (int)sizeof(buf))
    {
        buf[sizeof(buf) - 4] = '.';
        buf[sizeof(buf) - 3] = '.';
        buf[sizeof(buf) - 2] = '\n';
        buf[sizeof(buf) - 1] = '\0';
    }

    Log_Write(level, buf);
}


/* 이전 test_logger 심볼을 현재 공용 로그 구현으로 직접 연결한다. */
void TestLogger_Printf(LogLevel level, const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len >= (int)sizeof(buf))
    {
        buf[sizeof(buf) - 4] = '.';
        buf[sizeof(buf) - 3] = '.';
        buf[sizeof(buf) - 2] = '\n';
        buf[sizeof(buf) - 1] = '\0';
    }

    Log_Write(level, buf);
}
