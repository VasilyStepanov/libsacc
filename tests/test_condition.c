#include "test_condition.h"

#include "src/condition.h"
#include "src/mpool.h"
#include "src/selector.h"
#include "src/lexical_unit.h"
#include "test_utils.h"

#include <sacc.h>

#include <assert.h>



static void test_condition_factory(void) {
  SAC_MPool mpool;
  SAC_Condition *condition;
  SAC_Condition *firstCondition;
  SAC_Condition *secondCondition;
  SAC_Selector *selector;
  SAC_LexicalUnit *ident;

  mpool = SAC_mpool_open(256);

  condition = SAC_condition_class(mpool, "foo");
  assert(condition->conditionType == SAC_CLASS_CONDITION);
  assert(condition->desc.attribute.prefix == NULL);
  assert(condition->desc.attribute.name == NULL);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_id(mpool, "foo");
  assert(condition->conditionType == SAC_ID_CONDITION);
  assert(condition->desc.attribute.prefix == NULL);
  assert(condition->desc.attribute.name == NULL);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_prefix_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_PREFIX_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_suffix_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_SUFFIX_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_substring_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_SUBSTRING_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_attribute(mpool, "ns", "foo", NULL);
  assert(condition->conditionType == SAC_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  assert(condition->desc.attribute.value == NULL);

  condition = SAC_condition_one_of_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_ONE_OF_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_begin_hyphen_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.prefix);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.name);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  ident = SAC_lexical_unit_ident(mpool, "foo");
  condition = SAC_condition_pseudo_class(mpool, ident);
  assert(condition->conditionType == SAC_PSEUDO_CLASS_CONDITION);
  assert(condition->desc.pseudo == ident);

  ident = SAC_lexical_unit_ident(mpool, "foo");
  condition = SAC_condition_pseudo_element(mpool, ident);
  assert(condition->conditionType == SAC_PSEUDO_ELEMENT_CONDITION);
  assert(condition->desc.pseudo == ident);

  firstCondition = SAC_condition_class(mpool, "foo");
  secondCondition = SAC_condition_class(mpool, "bar");
  condition = SAC_condition_and(mpool, firstCondition, secondCondition);
  assert(condition->conditionType == SAC_AND_CONDITION);
  assert(condition->desc.combinator.firstCondition == firstCondition);
  assert(condition->desc.combinator.secondCondition == secondCondition);

  selector = SAC_selector_any_node(mpool);
  condition = SAC_condition_negation(mpool, selector);
  assert(condition->conditionType == SAC_NEGATIVE_CONDITION);
  assert(condition->desc.selector == selector);
  
  SAC_mpool_close(mpool);
}



void test_condition(void) {
  test_condition_factory();
}
