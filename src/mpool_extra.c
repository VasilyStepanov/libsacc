#include "mpool_extra.h"

#include <string.h>



char* SAC_mpool_strdup(SAC_MPool mpool, const char *s) {
  size_t n = strlen(s) + 1;
  char *dest = (char*)SAC_mpool_alloc(mpool, n);

  if (dest == NULL) return NULL;

  memcpy(dest, s, n);
  return dest;
}



char* SAC_mpool_strndup(SAC_MPool mpool, const char *s, size_t n) {
  char *dest = (char*)SAC_mpool_alloc(mpool, n + 1);

  if (dest == NULL) return NULL;

  memcpy(dest, s, n);
  dest[n] = '\0';
  return dest;
}
