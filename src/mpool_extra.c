#include "mpool_extra.h"

#include <string.h>



char* mpool_strdup(mpool_t mpool, const char *s) {
  size_t n = strlen(s);
  char *ret = (char*)mpool_alloc(mpool, n + 1);
  memcpy(ret, s, n);
  return ret;
}



char* mpool_strndup(mpool_t mpool, const char *s, size_t n) {
  char *ret = (char*)mpool_alloc(mpool, n + 1);
  memcpy(ret, s, n);
  return ret;
}
