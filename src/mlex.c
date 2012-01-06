#include "mpool.h"
#include "parser.h"

#include <stdlib.h>
#include <stddef.h>

extern void* yyget_extra();



void* yyalloc(size_t size, void *scanner) {
	return mpool_alloc(parser_mpool(yyget_extra(scanner)), size);
}



void* yyrealloc(void *ptr, size_t size, void *scanner) {
	return mpool_realloc(parser_mpool(yyget_extra(scanner)), ptr, size);
}



void yyfree(void *ptr, void *scanner) {
	mpool_free(parser_mpool(yyget_extra(scanner)), ptr);
}
