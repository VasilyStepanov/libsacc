#include "condition.h"



SAC_Condition* SAC_condition_alloc(SAC_MPool mpool, SAC_ConditionType type) {
  SAC_Condition *condition =
    (SAC_Condition*)SAC_mpool_alloc(mpool, sizeof(SAC_Condition));

  if (condition == NULL) return NULL;

  condition->conditionType = type;
  return condition;
}
