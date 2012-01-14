#include "stream.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define STREAM(stream) ((struct _SAC_Stream*)(stream))

struct _SAC_Stream {
  FILE *file;
  size_t size;
  char *buffer;
};



SAC_Stream SAC_stream_open() {
  struct _SAC_Stream *stream = (struct _SAC_Stream*)malloc(
    sizeof(struct _SAC_Stream));
  stream->file = open_memstream(&stream->buffer, &stream->size);
  return stream;
}



void SAC_stream_close(SAC_Stream stream) {
  fclose(STREAM(stream)->file);
  free(STREAM(stream)->buffer);
  free(stream);
}



void SAC_stream_printf(SAC_Stream stream, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(STREAM(stream)->file, fmt, args);
  va_end(args);
}



void SAC_stream_vprintf(SAC_Stream stream, const char *fmt, va_list ap) {
  vfprintf(STREAM(stream)->file, fmt, ap);
}



const char* SAC_stream_str(SAC_Stream stream) {
  fflush(STREAM(stream)->file);
  return STREAM(stream)->buffer;
}
