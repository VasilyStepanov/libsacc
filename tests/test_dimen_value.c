#include "test_dimen_value.h"

#include "dimen_value.h"
#include "mpool.h"

#include "test_utils.h"

#include <assert.h>



static void test_dimen_value_alloc() {
  SAC_MPool mpool;
  SAC_DimenValue *dimen;

  mpool = SAC_mpool_open(256);
  dimen = SAC_dimen_value_alloc(mpool, "123.5cm");

  assert(dimen->value == 123.5);
  assert_equals("cm", dimen->unit);

  SAC_dimen_value_free(dimen, mpool);
  SAC_mpool_close(mpool);
}



void test_dimen_value() {
  test_dimen_value_alloc();
}
