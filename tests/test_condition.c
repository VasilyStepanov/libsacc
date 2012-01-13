#include "test_condition.h"

#include "condition.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_condition_alloc() {
  SAC_MPool mpool;
  SAC_Condition *condition;

  mpool = SAC_mpool_open(256);
  condition = SAC_condition_alloc(mpool, SAC_CLASS_CONDITION);

  assert(condition->conditionType == SAC_CLASS_CONDITION);
  
  SAC_mpool_close(mpool);
}



void test_condition() {
  test_condition_alloc();
}
