#include "condition.h"



SAC_Condition* condition_alloc(mpool_t mpool, SAC_ConditionType type) {
  SAC_Condition *ret =
    (SAC_Condition*)mpool_alloc(mpool, sizeof(SAC_Condition));
  ret->conditionType = type;
  return ret;
}
