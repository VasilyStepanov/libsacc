#include "string_extra.h"

#include <string.h>



void SAC_strntrim(
  const char *s, size_t n,
  const char **t, size_t *tn,
  const char *match)
{
  for (; n != 0; ++s, --n) {
    if (strchr(match, *s) == NULL) break;
  }
  for (; n != 0; --n) {
    if (strchr(match, s[n - 1]) == NULL) break;
  }
  *t = s;
  *tn = n;
}
