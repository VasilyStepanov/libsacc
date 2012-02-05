#include "test_condition.h"

#include "condition.h"
#include "mpool.h"
#include "selector.h"
#include "test_utils.h"

#include <sacc.h>

#include <assert.h>



static void test_condition_factory() {
  SAC_MPool mpool;
  SAC_Condition *condition;
  SAC_Condition *firstCondition;
  SAC_Condition *secondCondition;
  SAC_Selector *selector;

  mpool = SAC_mpool_open(256);

  condition = SAC_condition_class(mpool, "foo");
  assert(condition->conditionType == SAC_CLASS_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  assert(condition->desc.attribute.localName == NULL);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_id(mpool, "foo");
  assert(condition->conditionType == SAC_ID_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  assert(condition->desc.attribute.localName == NULL);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_prefix_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_PREFIX_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_suffix_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_SUFFIX_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_substring_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_SUBSTRING_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_attribute(mpool, "ns", "foo", NULL);
  assert(condition->conditionType == SAC_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  assert(condition->desc.attribute.value == NULL);

  condition = SAC_condition_one_of_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_ONE_OF_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_begin_hypen_attribute(mpool, "ns", "foo", "bar");
  assert(condition->conditionType == SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION);
  ASSERT_EQUAL_STRINGS("ns", condition->desc.attribute.namespaceURI);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_FALSE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_pseudo_class(mpool, "foo");
  assert(condition->conditionType == SAC_PSEUDO_CLASS_CONDITION);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.pseudo.name);
  assert(condition->desc.pseudo.parameters == NULL);

  condition = SAC_condition_pseudo_element(mpool, "foo");
  assert(condition->conditionType == SAC_PSEUDO_ELEMENT_CONDITION);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.pseudo.name);
  assert(condition->desc.pseudo.parameters == NULL);

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



void test_condition() {
  test_condition_factory();
}
