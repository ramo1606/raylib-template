// test.h - Tiny test harness (no dependencies). Output is grep-friendly for agents:
//   PASS name / FAIL name (file:line) expr  and a final "TESTS passed=N failed=M".
#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static int testsPassed = 0, testsFailed = 0, currentFailed = 0;

#define CHECK(expr)                                                                                                                        \
    do                                                                                                                                     \
    {                                                                                                                                      \
        if (!(expr))                                                                                                                       \
        {                                                                                                                                  \
            printf("  FAIL (%s:%d) %s\n", __FILE__, __LINE__, #expr);                                                                      \
            currentFailed = 1;                                                                                                             \
        }                                                                                                                                  \
    } while (0)

#define CHECK_NEAR(a, b, eps) CHECK(((a) - (b) < (eps)) && ((b) - (a) < (eps)))

#define RUN(fn)                                                                                                                            \
    do                                                                                                                                     \
    {                                                                                                                                      \
        currentFailed = 0;                                                                                                                 \
        fn();                                                                                                                              \
        if (currentFailed)                                                                                                                 \
        {                                                                                                                                  \
            testsFailed++;                                                                                                                 \
            printf("FAIL %s\n", #fn);                                                                                                      \
        }                                                                                                                                  \
        else                                                                                                                               \
        {                                                                                                                                  \
            testsPassed++;                                                                                                                 \
            printf("PASS %s\n", #fn);                                                                                                      \
        }                                                                                                                                  \
    } while (0)

#define TEST_SUMMARY() (printf("TESTS passed=%d failed=%d\n", testsPassed, testsFailed), (testsFailed > 0))

#endif // TEST_H
