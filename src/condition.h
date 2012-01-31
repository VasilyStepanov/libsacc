#ifndef CONDITION_H
#define CONDITION_H

#include "mpool.h"

#include <sacc.h>



SAC_Condition* SAC_condition_class(SAC_MPool mpool, SAC_STRING value);
SAC_Condition* SAC_condition_id(SAC_MPool mpool, SAC_STRING value);
SAC_Condition* SAC_condition_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_Boolean specified, SAC_STRING value);
SAC_Condition* SAC_condition_one_of_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_STRING value);
SAC_Condition* SAC_condition_begin_hypen_attribute(SAC_MPool mpool,
  SAC_STRING localName, SAC_STRING value);
SAC_Condition* SAC_condition_pseudo_class(SAC_MPool mpool, SAC_STRING value);
SAC_Condition* SAC_condition_and(SAC_MPool mpool,
  SAC_Condition *firstCondition, SAC_Condition *secondCondition);



#endif
