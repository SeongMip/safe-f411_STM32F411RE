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
