#ifndef CHECKS_H
#define CHECKS_H

#include "config.h"

#ifdef SAC_CHECKS
#  include <assert.h>
#  include <strings.h>
#endif



#ifndef SAC_CHECKS
#define SAC_NOP while(0)
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_UNSIGNED(ureal) assert(ureal >= 0)
#else
#  define SAC_CHECK_UNSIGNED(ureal) SAC_NOP
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_STRING_NOT_EQUALS(lhs, rhs) \
     do { \
       int eq = strcasecmp(lhs, rhs); \
       assert(eq != 0); \
     } while (0)
#else
#  define SAC_CHECK_STRING_NOT_EQUALS(lhs, rhs) SAC_NOP
#endif



#endif
