/****************************************************************************
 * @file    config.h
 * @brief   APP/TEST 및 Bare-metal/RTOS 실행 선택 값을 정의한다.
 *
 * @details
 * - main.c와 freertos.c가 동일한 실행 선택 기준을 공유하도록 사용한다.
 *
 ****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#define EXEC_ROLE_APP   0
#define EXEC_ROLE_TEST  1

#define EXEC_MODEL_BAREMETAL  1
#define EXEC_MODEL_RTOS       0

#define EXEC_ROLE   EXEC_ROLE_TEST
#define EXEC_MODEL  EXEC_MODEL_BAREMETAL

#if ((EXEC_ROLE != EXEC_ROLE_APP) && (EXEC_ROLE != EXEC_ROLE_TEST))
#error "EXEC_ROLE must be EXEC_ROLE_APP or EXEC_ROLE_TEST"
#endif

#if ((EXEC_MODEL != EXEC_MODEL_BAREMETAL) && (EXEC_MODEL != EXEC_MODEL_RTOS))
#error "EXEC_MODEL must be EXEC_MODEL_BAREMETAL or EXEC_MODEL_RTOS"
#endif

#endif
