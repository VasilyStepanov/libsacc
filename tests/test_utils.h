#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdarg.h>



typedef void* stream_t;



stream_t stream_open();

void stream_close(stream_t stream);

void stream_printf(stream_t stream, const char *fmt, ...);

void stream_vprintf(stream_t stream, const char *fmt, va_list ap);

const char* stream_str(stream_t stream);



void assert_equals(const char *lhs, const char *rhs);



#endif
