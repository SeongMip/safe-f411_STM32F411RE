/****************************************************************************
 * @file    rtos_app.h
 * @brief   RTOS 공용 서비스 초기화와 object/task 생성 API를 선언한다.
 *
 ****************************************************************************/

#ifndef RTOS_APP_H
#define RTOS_APP_H

void RtosApp_Init(void);
void RtosApp_CreateObjects(void);
void RtosApp_CreateTasks(void);
void RtosApp_Start(void);

#endif
