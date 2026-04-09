/****************************************************************************
 * @file    button_fsm.h
 * @brief   버튼 FSM 상태, 이벤트, 공용 API를 선언한다.
 *
 ****************************************************************************/

#ifndef BUTTON_FSM_H
#define BUTTON_FSM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTN_LEVEL_UP = 0,
    BTN_LEVEL_DOWN = 1
} BtnLevel;

typedef enum {
    BTN_EVT_NONE = 0,
    BTN_EVT_CLICK,
    BTN_EVT_LONG
} BtnEvent;

typedef struct {
    uint32_t debounce_ms;
    uint32_t release_ms;
    uint32_t long_ms;

    enum {
        ST_IDLE = 0,
        ST_DEBOUNCE_DOWN,
        ST_PRESSED,
        ST_DEBOUNCE_UP
    } st;

    uint32_t t_mark;
    uint32_t t_pressed;
    uint8_t  long_fired;

    BtnLevel last_level;
} ButtonFsm;

void ButtonFsm_Init(ButtonFsm* b, uint32_t debounce_ms, uint32_t release_ms, uint32_t long_ms);
BtnLevel ButtonFsm_Level(void);
BtnEvent ButtonFsm_Update(ButtonFsm* b, uint32_t now);

#ifdef __cplusplus
}
#endif

#endif
