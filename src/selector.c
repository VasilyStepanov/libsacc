#include "selector.h"

#include "checks.h"



static SAC_Selector* SAC_selector_alloc(SAC_MPool mpool,
  SAC_SelectorType type)
{
  SAC_Selector *selector = (SAC_Selector*)SAC_mpool_alloc(mpool,
    sizeof(SAC_Selector));

  if (selector == NULL) return NULL;

  selector->selectorType = type;
  return selector;
}



SAC_Selector* SAC_selector_any_node(SAC_MPool mpool) {
  SAC_Selector *selector;

  selector = SAC_selector_alloc(mpool, SAC_ANY_NODE_SELECTOR);
  if (selector == NULL) return NULL;

  return selector;
}



SAC_Selector* SAC_selector_element_node(SAC_MPool mpool,
  SAC_STRING namespaceURI, SAC_STRING localName)
{
  SAC_Selector *selector;

  selector = SAC_selector_alloc(mpool, SAC_ELEMENT_NODE_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.element.namespaceURI = namespaceURI;
  selector->desc.element.localName = localName;

  return selector;
}



SAC_Selector* SAC_selector_conditional(SAC_MPool mpool,
  SAC_Selector *simpleSelector, SAC_Condition *condition)
{
  SAC_Selector *selector;

  SAC_CHECK_SIMPLE_FOR_CONDITIONAL_SELECTOR(simpleSelector);

  selector = SAC_selector_alloc(mpool, SAC_CONDITIONAL_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.conditional.simpleSelector = simpleSelector;
  selector->desc.conditional.condition = condition;

  return selector;
}



SAC_Selector* SAC_selector_descendant(SAC_MPool mpool,
  SAC_Selector *descendantSelector, SAC_Selector *simpleSelector)
{
  SAC_Selector *selector;

  SAC_CHECK_SIMPLE_FOR_DESCENDANT_SELECTOR(simpleSelector);

  selector = SAC_selector_alloc(mpool, SAC_DESCENDANT_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.descendant.descendantSelector = descendantSelector;
  selector->desc.descendant.simpleSelector = simpleSelector;

  return selector;
}



SAC_Selector* SAC_selector_child(SAC_MPool mpool,
  SAC_Selector *descendantSelector, SAC_Selector *simpleSelector)
{
  SAC_Selector *selector;

  SAC_CHECK_SIMPLE_FOR_CHILD_SELECTOR(simpleSelector);

  selector = SAC_selector_alloc(mpool, SAC_CHILD_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.descendant.descendantSelector = descendantSelector;
  selector->desc.descendant.simpleSelector = simpleSelector;

  return selector;
}



SAC_Selector* SAC_selector_direct_adjacent(SAC_MPool mpool,
  SAC_NodeType nodeType,
  SAC_Selector *firstSelector, SAC_Selector *secondSelector)
{
  SAC_Selector *selector;

  SAC_CHECK_SIMPLE_FOR_DIRECT_ADJACENT_SELECTOR(secondSelector);

  selector = SAC_selector_alloc(mpool, SAC_DIRECT_ADJACENT_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.sibling.nodeType = nodeType;
  selector->desc.sibling.firstSelector = firstSelector;
  selector->desc.sibling.secondSelector = secondSelector;

  return selector;
}



SAC_Selector* SAC_selector_general_adjacent(SAC_MPool mpool,
  SAC_NodeType nodeType,
  SAC_Selector *firstSelector, SAC_Selector *secondSelector)
{
  SAC_Selector *selector;

  SAC_CHECK_SIMPLE_FOR_GENERAL_ADJACENT_SELECTOR(secondSelector);

  selector = SAC_selector_alloc(mpool, SAC_GENERAL_ADJACENT_SELECTOR);
  if (selector == NULL) return NULL;

  selector->desc.sibling.nodeType = nodeType;
  selector->desc.sibling.firstSelector = firstSelector;
  selector->desc.sibling.secondSelector = secondSelector;

  return selector;
}
