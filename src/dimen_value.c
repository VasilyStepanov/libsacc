#include "dimen_value.h"

#include "mpool_extra.h"

#include <stdlib.h>



SAC_DimenValue* SAC_dimen_value_alloc(SAC_MPool mpool, const char *text) {
  char *tail;
  char *unit;
  double value;
  SAC_DimenValue *dimen;

  value = strtod(text, &tail);
  unit = SAC_mpool_strdup(mpool, tail);

  if (unit == NULL) return NULL;
  
  dimen = (SAC_DimenValue*)SAC_mpool_alloc(
    mpool, sizeof(SAC_DimenValue)
  );

  if (dimen == NULL) {
    SAC_mpool_free(mpool, unit);
    return NULL;
  }

  dimen->value = value;
  dimen->unit = unit;
  return dimen;
}



void SAC_dimen_value_free(SAC_DimenValue *dimen, SAC_MPool mpool) {
  SAC_mpool_free(mpool, dimen->unit);
  SAC_mpool_free(mpool, dimen);
}
