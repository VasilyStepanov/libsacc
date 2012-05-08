#include "test_selector.h"

#include "src/selector.h"
#include "src/condition.h"
#include "src/mpool.h"
#include "test_utils.h"

#include <sacc.h>

#include <assert.h>



static void test_selector_factory(void) {
  SAC_MPool mpool;
  SAC_Selector *selector;
  SAC_Selector *simpleSelector;
  SAC_Selector *descendantSelector;
  SAC_Selector *firstSelector;
  SAC_Selector *secondSelector;
  SAC_Condition *condition;

  mpool = SAC_mpool_open(256);

  selector = SAC_selector_any_node(mpool);
  assert(selector->selectorType == SAC_ANY_NODE_SELECTOR);

  selector = SAC_selector_element_node(mpool, "foo", "bar");
  assert(selector->selectorType == SAC_ELEMENT_NODE_SELECTOR);
  ASSERT_EQUAL_STRINGS("foo", selector->desc.element.prefix);
  ASSERT_EQUAL_STRINGS("bar", selector->desc.element.name);

  simpleSelector = SAC_selector_any_node(mpool);
  condition = SAC_condition_class(mpool, "foo");
  selector = SAC_selector_conditional(mpool, simpleSelector, condition);
  assert(selector->selectorType == SAC_CONDITIONAL_SELECTOR);
  assert(selector->desc.conditional.simpleSelector == simpleSelector);
  assert(selector->desc.conditional.condition == condition);

  simpleSelector = SAC_selector_any_node(mpool);
  descendantSelector = SAC_selector_any_node(mpool);
  selector = SAC_selector_descendant(mpool, descendantSelector, simpleSelector);
  assert(selector->selectorType == SAC_DESCENDANT_SELECTOR);
  assert(selector->desc.descendant.descendantSelector == descendantSelector);
  assert(selector->desc.descendant.simpleSelector == simpleSelector);

  simpleSelector = SAC_selector_any_node(mpool);
  descendantSelector = SAC_selector_any_node(mpool);
  selector = SAC_selector_child(mpool, descendantSelector, simpleSelector);
  assert(selector->selectorType == SAC_CHILD_SELECTOR);
  assert(selector->desc.descendant.descendantSelector == descendantSelector);
  assert(selector->desc.descendant.simpleSelector == simpleSelector);

  firstSelector = SAC_selector_any_node(mpool);
  secondSelector = SAC_selector_any_node(mpool);
  selector = SAC_selector_direct_adjacent(mpool,
    firstSelector, secondSelector);
  assert(selector->selectorType == SAC_DIRECT_ADJACENT_SELECTOR);
  assert(selector->desc.sibling.firstSelector == firstSelector);
  assert(selector->desc.sibling.secondSelector == secondSelector);

  firstSelector = SAC_selector_any_node(mpool);
  secondSelector = SAC_selector_any_node(mpool);
  selector = SAC_selector_general_adjacent(mpool,
    firstSelector, secondSelector);
  assert(selector->selectorType == SAC_GENERAL_ADJACENT_SELECTOR);
  assert(selector->desc.sibling.firstSelector == firstSelector);
  assert(selector->desc.sibling.secondSelector == secondSelector);
  
  SAC_mpool_close(mpool);
}



void test_selector(void) {
  test_selector_factory();
}
