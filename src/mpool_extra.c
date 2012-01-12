#include "mpool_extra.h"

#include <string.h>



char* SAC_mpool_strdup(SAC_MPool mpool, const char *s) {
  size_t n = strlen(s) + 1;
  char *ret = (char*)SAC_mpool_alloc(mpool, n);
  memcpy(ret, s, n);
  return ret;
}



char* SAC_mpool_strndup(SAC_MPool mpool, const char *s, size_t n) {
  char *ret = (char*)SAC_mpool_alloc(mpool, n + 1);
  memcpy(ret, s, n);
  ret[n] = '\0';
  return ret;
}
