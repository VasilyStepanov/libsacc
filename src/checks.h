#ifndef CHECKS_H
#define CHECKS_H

#include "config.h"

#include <assert.h>



#ifndef SAC_CHECKS
#define SAC_NOP while(0)
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_UNSIGNED(ureal) assert(ureal >= 0)
#else
#  define SAC_CHECK_UNSIGNED(ureal) SAC_NOP
#endif



#endif
