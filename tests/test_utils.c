#include "test_utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define STREAM(stream) ((struct stream_s*)(stream))

struct stream_s {
  FILE *file;
  size_t size;
  char *buffer;
};



stream_t stream_open() {
  struct stream_s *stream = (struct stream_s*)malloc(sizeof(struct stream_s));
  stream->file = open_memstream(&stream->buffer, &stream->size);
  return stream;
}



void stream_close(stream_t stream) {
  fclose(STREAM(stream)->file);
  free(STREAM(stream)->buffer);
  free(stream);
}



void stream_printf(stream_t stream, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(STREAM(stream)->file, fmt, args);
  va_end(args);
}



void stream_vprintf(stream_t stream, const char *fmt, va_list ap) {
  vfprintf(STREAM(stream)->file, fmt, ap);
}



const char* stream_str(stream_t stream) {
  fflush(STREAM(stream)->file);
  return STREAM(stream)->buffer;
}



void assert_equals(const char *lhs, const char *rhs) {
  int eq = strcmp(lhs, rhs);
  if (eq != 0) {
    printf(
"Expected: %s\n"
"Received: %s\n",
    lhs, rhs);
  }
  assert(eq == 0);
}
