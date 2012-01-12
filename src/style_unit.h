#ifndef STYLE_UNIT_H
#define STYLE_UNIT_H

#include "mpool.h"
#include "vector.h"
#include "list.h"


typedef enum {
  SAC_RULESET
} SAC_StyleUnitType;

struct _SAC_StyleUnit {
  SAC_StyleUnitType type;

  union {
    /* SAC_RULESET */
    struct {
      SAC_Vector selectors;
      SAC_List declarations;
    } ruleset;
  } desc;
};

typedef struct _SAC_StyleUnit SAC_StyleUnit;



SAC_StyleUnit* SAC_style_unit_alloc(SAC_MPool mpool, SAC_StyleUnitType type);



#endif
