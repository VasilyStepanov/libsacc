#include "condition.h"



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
  condition->desc.attribute.localName = "class";
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_id(SAC_MPool mpool, SAC_STRING value) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ID_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = "id";
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_Boolean specified, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = specified;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_one_of_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_ONE_OF_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_begin_hypen_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_STRING value)
{
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = localName;
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

  return condition;
}



SAC_Condition* SAC_condition_pseudo_class(SAC_MPool mpool, SAC_STRING value) {
  SAC_Condition *condition;

  condition = SAC_condition_alloc(mpool, SAC_PSEUDO_CLASS_CONDITION);
  if (condition == NULL) return condition;

  condition->desc.attribute.namespaceURI = NULL;
  condition->desc.attribute.localName = NULL;
  condition->desc.attribute.specified = SAC_TRUE;
  condition->desc.attribute.value = value;

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
