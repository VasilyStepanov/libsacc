#ifndef CONDITION_H
#define CONDITION_H

#include "mpool.h"

#include <sacc.h>



SAC_Condition* SAC_condition_class(SAC_MPool mpool, SAC_STRING value);
SAC_Condition* SAC_condition_id(SAC_MPool mpool, SAC_STRING value);
SAC_Condition* SAC_condition_prefix_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_suffix_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_substring_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_one_of_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_begin_hyphen_attribute(SAC_MPool mpool,
  SAC_STRING prefix, SAC_STRING name, SAC_STRING value);
SAC_Condition* SAC_condition_pseudo_class(SAC_MPool mpool,
  SAC_LexicalUnit *pseudo);
SAC_Condition* SAC_condition_pseudo_element(SAC_MPool mpool,
  SAC_LexicalUnit *pseudo);
SAC_Condition* SAC_condition_and(SAC_MPool mpool,
  SAC_Condition *firstCondition, SAC_Condition *secondCondition);
SAC_Condition* SAC_condition_negation(SAC_MPool mpool, SAC_Selector *selector);



#endif
