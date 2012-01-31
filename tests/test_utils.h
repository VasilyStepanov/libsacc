#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string.h>
#include <stdio.h>
#include <math.h>



#define ASSERT_EQUAL_STRINGS(lhs, rhs) \
  do { \
    int eq = strcmp(lhs, rhs); \
    if (eq != 0) { \
      printf( \
"Expected: %s\n" \
"Received: %s\n", \
      lhs, rhs); \
    } \
    assert(eq == 0); \
  } while (0)



#define ASSERT_EQUAL_REALS(lhs, rhs) \
  assert(fabs(lhs - rhs) < 1e-7)


#endif
