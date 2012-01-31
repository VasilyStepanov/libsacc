#include "test_condition.h"

#include "condition.h"
#include "mpool.h"
#include "test_utils.h"

#include <sacc.h>

#include <assert.h>



static void test_condition_factory() {
  SAC_MPool mpool;
  SAC_Condition *condition;
  SAC_Condition *firstCondition;
  SAC_Condition *secondCondition;

  mpool = SAC_mpool_open(256);

  condition = SAC_condition_class(mpool, "foo");
  assert(condition->conditionType == SAC_CLASS_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  ASSERT_EQUAL_STRINGS("class", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_id(mpool, "foo");
  assert(condition->conditionType == SAC_ID_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  ASSERT_EQUAL_STRINGS("id", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  condition = SAC_condition_attribute(mpool, "foo", SAC_TRUE, "bar");
  assert(condition->conditionType == SAC_ATTRIBUTE_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_one_of_attribute(mpool, "foo", "bar");
  assert(condition->conditionType == SAC_ONE_OF_ATTRIBUTE_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_begin_hypen_attribute(mpool, "foo", "bar");
  assert(condition->conditionType == SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.localName);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("bar", condition->desc.attribute.value);

  condition = SAC_condition_pseudo_class(mpool, "foo");
  assert(condition->conditionType == SAC_PSEUDO_CLASS_CONDITION);
  assert(condition->desc.attribute.namespaceURI == NULL);
  assert(condition->desc.attribute.localName == NULL);
  assert(condition->desc.attribute.specified == SAC_TRUE);
  ASSERT_EQUAL_STRINGS("foo", condition->desc.attribute.value);

  firstCondition = SAC_condition_class(mpool, "foo");
  secondCondition = SAC_condition_class(mpool, "bar");
  condition = SAC_condition_and(mpool, firstCondition, secondCondition);
  assert(condition->desc.combinator.firstCondition == firstCondition);
  assert(condition->desc.combinator.secondCondition == secondCondition);
  
  SAC_mpool_close(mpool);
}



void test_condition() {
  test_condition_factory();
}
