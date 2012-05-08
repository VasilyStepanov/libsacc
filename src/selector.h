#ifndef SELECTOR_H
#define SELECTOR_H

#include "mpool.h"

#include <sacc.h>



SAC_Selector* SAC_selector_any_node(SAC_MPool mpool);
SAC_Selector* SAC_selector_element_node(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name);
SAC_Selector* SAC_selector_conditional(SAC_MPool mpool,
  SAC_Selector *simpleSelector, SAC_Condition *condition);
SAC_Selector* SAC_selector_descendant(SAC_MPool mpool,
  SAC_Selector *descendantSelector, SAC_Selector *simpleSelector);
SAC_Selector* SAC_selector_child(SAC_MPool mpool,
  SAC_Selector *descendantSelector, SAC_Selector *simpleSelector);
SAC_Selector* SAC_selector_direct_adjacent(SAC_MPool mpool,
  SAC_Selector *firstSelector, SAC_Selector *secondSelector);
SAC_Selector* SAC_selector_general_adjacent(SAC_MPool mpool,
  SAC_Selector *firstSelector, SAC_Selector *secondSelector);



#endif
