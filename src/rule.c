#include "rule.h"



SAC_Rule* rule_alloc(mpool_t mpool, vector_t selectors, list_t declarations) {
  SAC_Rule *rule = (SAC_Rule*)mpool_alloc(mpool, sizeof(SAC_Rule));
  rule->selectors = selectors;
  rule->declarations = declarations;
  return rule;
}
