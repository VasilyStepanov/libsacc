#include "mpool.h"
#include "parser.h"

#include <stdlib.h>
#include <stddef.h>



void* yyalloc(size_t size, void *scanner) {
	return SAC_mpool_alloc(YY_SCANNER_MPOOL(scanner), size);
}



void* yyrealloc(void *ptr, size_t size, void *scanner) {
	return SAC_mpool_realloc(YY_SCANNER_MPOOL(scanner), ptr, size);
}



void yyfree(void *ptr, void *scanner) {
	SAC_mpool_free(YY_SCANNER_MPOOL(scanner), ptr);
}
