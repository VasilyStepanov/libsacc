#include "condition.h"

#include "checks.h"



static SAC_Condition* SAC_condition_alloc(SAC_MPool mpool,
  SAC_ConditionType type)
{
  SAC_Condition *condition =
    (SAC_Condition*)SAC_mpool_alloc(mpool, sizeof(SAC_Condition));

  if (condition == NULL) return NULL;

  condition->conditionType = type;
  return condition;
}



SAC_Condition* SAC_condition_class(SAC_MPool mpool, SAC_STRING value) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_CLASS_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = NULL;
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_id(SAC_MPool mpool, SAC_STRING value) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ID_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = NULL;
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_prefix_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_PREFIX_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_suffix_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_SUFFIX_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_substring_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_SUBSTRING_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = value != NULL ? SAC_TRUE : SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_one_of_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ONE_OF_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_begin_hypen_attribute(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = namespaceURI;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_FALSE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_pseudo_class(SAC_MPool mpool, SAC_STRING name) {
  SAC_Condition *condition;

  SAC_CHECK_STRING_NOT_EQUALS(name, "first-line");
  SAC_CHECK_STRING_NOT_EQUALS(name, "first-letter");
  SAC_CHECK_STRING_NOT_EQUALS(name, "before");
  SAC_CHECK_STRING_NOT_EQUALS(name, "after");

  condition = SAC_condition_alloc(mpool, SAC_PSEUDO_CLASS_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.pseudo.name = name;
  condition->desc.pseudo.parameters = NULL;

  return condition;
}



SAC_Condition* SAC_condition_pseudo_element(SAC_MPool mpool, SAC_STRING name) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_PSEUDO_ELEMENT_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.pseudo.name = name;
  condition->desc.pseudo.parameters = NULL;

  return condition;
}



SAC_Condition* SAC_condition_and(SAC_MPool mpool,
  SAC_Condition *firstCondition, SAC_Condition *secondCondition)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_AND_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.combinator.firstCondition = firstCondition;
  condition->desc.combinator.secondCondition = secondCondition;

  return condition;
}



SAC_Condition* SAC_condition_negation(SAC_MPool mpool, SAC_Selector *selector) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_NEGATIVE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.selector = selector;

  return condition;
}
