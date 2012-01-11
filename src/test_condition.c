#include "test_condition.h"

#include "condition.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



void test_condition_alloc() {
  mpool_t mpool;
  SAC_Condition *condition;

  mpool = mpool_open(256);
  condition = condition_alloc(mpool, SAC_CLASS_CONDITION);

  assert(condition->conditionType == SAC_CLASS_CONDITION);
  
  mpool_close(mpool);
}



void test_condition() {
  test_condition_alloc();
}
