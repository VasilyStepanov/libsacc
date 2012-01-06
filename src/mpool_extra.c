#include "mpool_extra.h"

#include <string.h>



char* mpool_strdup(mpool_t mpool, const char *s) {
  size_t len = strlen(s) + 1;
  char *ret = (char*)mpool_alloc(mpool, len);
  memcpy(ret, s, len);
  return ret;
}
