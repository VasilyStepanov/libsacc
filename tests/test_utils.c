#include "test_utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void assert_equals(const char *lhs, const char *rhs) {
  int eq = strcmp(lhs, rhs);
  if (eq != 0) {
    printf(
"Expected: %s\n"
"Received: %s\n",
    lhs, rhs);
  }
  assert(eq == 0);
}
