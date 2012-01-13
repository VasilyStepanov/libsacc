#ifndef STYLE_UNIT_H
#define STYLE_UNIT_H

#include "mpool.h"
#include "vector.h"
#include "list.h"


typedef enum {
  SAC_STYLE_RULESET,
  SAC_STYLE_IMPORT
} SAC_StyleUnitType;

struct _SAC_StyleUnit {
  SAC_StyleUnitType type;

  union {
    /* SAC_STYLE_RULESET */
    struct {
      SAC_Vector selectors;
      SAC_List declarations;
    } ruleset;

    /* SAC_STYLE_IMPORT */
    struct {
      SAC_STRING uri;
      const SAC_STRING *media;
    } import;
  } desc;
};

typedef struct _SAC_StyleUnit SAC_StyleUnit;



SAC_StyleUnit* SAC_style_unit_alloc(SAC_MPool mpool, SAC_StyleUnitType type);



#endif
