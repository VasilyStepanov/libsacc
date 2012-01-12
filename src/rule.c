#include "rule.h"



SAC_Rule* rule_alloc(SAC_MPool mpool,
  SAC_Vector selectors, SAC_List declarations)
{
  SAC_Rule *rule = (SAC_Rule*)mpool_alloc(mpool, sizeof(SAC_Rule));
  rule->selectors = selectors;
  rule->declarations = declarations;
  return rule;
}
