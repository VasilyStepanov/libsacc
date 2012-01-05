#include "mpool.h"

#include <stdlib.h>
#include <stddef.h>

extern void* yyget_extra();



void* yyalloc(size_t size, void *scanner) {
	return mpool_alloc(yyget_extra(scanner), size);
}



void* yyrealloc(void *ptr, size_t size, void *scanner) {
	return mpool_realloc(yyget_extra(scanner), ptr, size);
}



void yyfree(void *ptr, void *scanner) {
	mpool_free(yyget_extra(scanner), ptr);
}
