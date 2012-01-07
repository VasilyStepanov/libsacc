#ifndef STRING_EXTRA_H
#define STRING_EXTRA_H

#include <stddef.h>



void strntrim(
  const char *s, size_t n,
  const char **t, size_t *tn,
  const char *match);



#endif
