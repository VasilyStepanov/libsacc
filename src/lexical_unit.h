#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"
#include "list.h"

#include <sacc.h>



SAC_LexicalUnit* SAC_lexical_unit_alloc(SAC_MPool mpool,
  SAC_LexicalUnitCode type);

SAC_LexicalUnit* SAC_lexical_unit_from_list(SAC_List list, SAC_MPool mpool);

SAC_LexicalUnit* SAC_lexical_unit_int(SAC_MPool, int integer);
SAC_LexicalUnit* SAC_lexical_unit_real(SAC_MPool, double real);
SAC_LexicalUnit* SAC_lexical_unit_percentage(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_pixel(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_inch(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_centimeter(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_millimeter(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_point(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_pica(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_em(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_ex(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_degree(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_radian(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_gradian(SAC_MPool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_millisecond(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_second(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_hertz(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_kilohertz(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_dots_per_inch(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_dots_per_centimeter(SAC_MPool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_string(SAC_MPool, SAC_STRING string);



#endif
