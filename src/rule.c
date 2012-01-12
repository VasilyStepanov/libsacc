#include "rule.h"



rule_t rule_alloc(mpool_t mpool, vector_t selectors, list_t declarations) {
  rule_t rule = (rule_t)mpool_alloc(mpool, sizeof(struct rule_s));
  rule->selectors = selectors;
  rule->declarations = declarations;
  return rule;
}
