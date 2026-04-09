/****************************************************************************
 * @file    test_rtos_runtime.h
 * @brief   RTOS 테스트용 object/task 생성 API를 선언한다.
 *
 ****************************************************************************/

#ifndef TEST_RTOS_RUNTIME_H
#define TEST_RTOS_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* RTOS 테스트 경로는 공용 service 초기화 이후 object/task 생성 단계만 별도 래퍼로 둔다. */
void TestRtosRuntime_CreateObjects(void);
void TestRtosRuntime_CreateTasks(void);

#ifdef __cplusplus
}
#endif

#endif
