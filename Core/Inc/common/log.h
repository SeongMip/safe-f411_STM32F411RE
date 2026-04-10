#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

void Log_Init(void);
void Log_Raw(const char* str);
void Log_Write(LogLevel level, const char* str);
void Log_Printf(LogLevel level, const char* fmt, ...);

void TestLogger_Printf(LogLevel level, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
