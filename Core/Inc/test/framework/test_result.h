/****************************************************************************
 * @file    test_result.h
 * @brief   공통 테스트 결과 상태와 판정 enum을 선언한다.
 *
 ****************************************************************************/

#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_IN_REVIEW = 2
} TestResult;

#ifdef __cplusplus
}
#endif

#endif
