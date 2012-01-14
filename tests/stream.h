#ifndef STREAM_H
#define STREAM_H

#include <stdarg.h>



typedef void* SAC_Stream;



SAC_Stream SAC_stream_open();

void SAC_stream_close(SAC_Stream stream);

void SAC_stream_printf(SAC_Stream stream, const char *fmt, ...);

void SAC_stream_vprintf(SAC_Stream stream, const char *fmt, va_list ap);

const char* SAC_stream_str(SAC_Stream stream);



#endif
