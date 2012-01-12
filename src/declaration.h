#ifndef DECLARATION_H
#define DECLARATION_H

#include "mpool.h"

#include <sacc.h>



struct _SAC_Declaration {
  SAC_STRING property;
  SAC_LexicalUnit *value;
  SAC_Boolean important;
};

typedef struct _SAC_Declaration SAC_Declaration;



SAC_Declaration* SAC_declaration_alloc(SAC_MPool mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important);



#endif
