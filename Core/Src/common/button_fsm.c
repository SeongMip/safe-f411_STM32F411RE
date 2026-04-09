/****************************************************************************
 * @file    button_fsm.c
 * @brief   버튼 raw 입력을 debounce/click/long-press 이벤트로 변환한다.
 *
 * @details
 * - active-low 버튼 입력을 polling 기반 상태 전이로 판정한다.
 * - 제품 기능 완성보다 재현 가능한 시험 입력 판정을 우선한다.
 *
 * @note
 * - 호출 주기가 크게 흔들리면 debounce와 long-press 판정 결과에도 영향이 있을 수 있다.
 *
 ****************************************************************************/

#include "button_fsm.h"
#include "platform_gpio.h"

static inline BtnLevel raw_to_level(PlatformButtonRawState raw)
{
    return (raw == PLATFORM_BUTTON_DOWN) ? BTN_LEVEL_DOWN : BTN_LEVEL_UP;
}

static inline int ButtonFsm_Elapsed(uint32_t now, uint32_t since, uint32_t threshold_ms)
{
    return (int32_t)(now - since) >= (int32_t)threshold_ms;
}

/**
 * @brief   버튼 상태 머신을 초기화한다.
 *
 * @param   b           버튼 FSM 인스턴스
 * @param   debounce_ms press 확정 전 debounce 시간
 * @param   release_ms  release 확정 전 release debounce 시간
 * @param   long_ms     long-press 판정 시간
 *
 * @details
 * - 초기 level은 현재 GPIO raw 상태를 기준으로 맞춘다.
 * - 시작 직후 오검출을 줄이기 위해 내부 상태와 기준 시간을 초기화한다.
 */
void ButtonFsm_Init(ButtonFsm* b, uint32_t debounce_ms, uint32_t release_ms, uint32_t long_ms)
{
    b->debounce_ms = debounce_ms;
    b->release_ms  = release_ms;
    b->long_ms     = long_ms;

    b->st = ST_IDLE;
    b->t_mark = 0;
    b->t_pressed = 0;
    b->long_fired = 0;

    b->last_level = raw_to_level(Platform_ButtonReadRaw());
}

BtnLevel ButtonFsm_Level(void)
{
    return raw_to_level(Platform_ButtonReadRaw());
}

/**
 * @brief   현재 버튼 상태를 반영하여 click/long 이벤트를 판정한다.
 *
 * @param   b    버튼 FSM 인스턴스
 * @param   now  현재 tick(ms)
 *
 * @return  BTN_EVT_NONE / BTN_EVT_CLICK / BTN_EVT_LONG
 *
 * @details
 * - press는 debounce 이후에만 확정한다.
 * - long-press는 지정 시간 이상 유지 시 1회만 발생한다.
 * - release 이후 안정 상태가 확인되면 click 여부를 최종 판정한다.
 */
BtnEvent ButtonFsm_Update(ButtonFsm* b, uint32_t now)
{
    BtnLevel level = ButtonFsm_Level();

    switch (b->st)
    {
    case ST_IDLE:
        if (b->last_level == BTN_LEVEL_UP && level == BTN_LEVEL_DOWN)
        {
            b->st = ST_DEBOUNCE_DOWN;
            b->t_mark = now;
        }
        break;

    case ST_DEBOUNCE_DOWN:
        if (ButtonFsm_Elapsed(now, b->t_mark, b->debounce_ms))
        {
            if (level == BTN_LEVEL_DOWN)
            {
                b->st = ST_PRESSED;
                b->t_pressed = now;
                b->long_fired = 0;
            }
            else
            {
                b->st = ST_IDLE;
            }
        }
        break;

    case ST_PRESSED:
        if (!b->long_fired && ButtonFsm_Elapsed(now, b->t_pressed, b->long_ms))
        {
            b->long_fired = 1;
            return BTN_EVT_LONG;
        }

        if (level == BTN_LEVEL_UP)
        {
            b->st = ST_DEBOUNCE_UP;
            b->t_mark = now;
        }
        break;

    case ST_DEBOUNCE_UP:
        if (ButtonFsm_Elapsed(now, b->t_mark, b->release_ms))
        {
            /* release 이후 click 확정: press 중 bounce 오검출 방지 */
            if (raw_to_level(Platform_ButtonReadRaw()) == BTN_LEVEL_UP)
            {
                BtnEvent evt = b->long_fired ? BTN_EVT_NONE : BTN_EVT_CLICK;
                b->st = ST_IDLE;
                b->last_level = raw_to_level(Platform_ButtonReadRaw());
                return evt;
            }
            else
            {
                b->st = ST_PRESSED;
            }
        }
        break;
    }

    b->last_level = level;
    return BTN_EVT_NONE;
}
