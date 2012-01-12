#include "condition.h"



SAC_Condition* SAC_condition_alloc(SAC_MPool mpool, SAC_ConditionType type) {
  SAC_Condition *ret =
    (SAC_Condition*)SAC_mpool_alloc(mpool, sizeof(SAC_Condition));
  ret->conditionType = type;
  return ret;
}
