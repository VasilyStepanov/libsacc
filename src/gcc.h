#ifndef GCC_H
#define GCC_H



#if defined(__GNUC__) && (__GNUC__ >= 3)
#  define SAC_UNUSED  __attribute__ ((unused))
#else
#  define SAC_UNUSED
#endif



#endif
