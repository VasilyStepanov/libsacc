#include <stdlib.h>
#include <stddef.h>

void* yyalloc(size_t size, void *scanner __attribute__((unused))) {
	return malloc(size);
}



void* yyrealloc(void *ptr, size_t size, void *scanner __attribute__((unused))) {
	return realloc(ptr, size);
}



void yyfree(void *ptr, void *scanner __attribute__((unused))) {
	free(ptr);
}
