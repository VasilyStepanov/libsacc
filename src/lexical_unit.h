#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"
#include "list.h"

#include <sacc.h>



SAC_LexicalUnit* SAC_lexical_unit_from_list(SAC_List list, SAC_MPool mpool);

SAC_LexicalUnit* SAC_lexical_unit_int(SAC_MPool mpool, int integer);
SAC_LexicalUnit* SAC_lexical_unit_real(SAC_MPool mpool, double real);
SAC_LexicalUnit* SAC_lexical_unit_percentage(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_pixel(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_inch(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_centimeter(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_millimeter(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_point(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_pica(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_em(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_ex(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_degree(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_radian(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_gradian(SAC_MPool mpool, double sreal);
SAC_LexicalUnit* SAC_lexical_unit_millisecond(SAC_MPool mpool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_second(SAC_MPool mpool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_hertz(SAC_MPool mpool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_kilohertz(SAC_MPool mpool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_dots_per_inch(SAC_MPool mpool, double ureal);
SAC_LexicalUnit* SAC_lexical_unit_dots_per_centimeter(SAC_MPool mpool,
  double ureal);
SAC_LexicalUnit* SAC_lexical_unit_string(SAC_MPool mpool, SAC_STRING string);
SAC_LexicalUnit* SAC_lexical_unit_ident(SAC_MPool mpool, SAC_STRING ident);
SAC_LexicalUnit* SAC_lexical_unit_inherit(SAC_MPool mpool);
SAC_LexicalUnit* SAC_lexical_unit_uri(SAC_MPool mpool, SAC_STRING uri);
SAC_LexicalUnit* SAC_lexical_unit_unicode_range(SAC_MPool mpool,
  SAC_STRING unicode_range);
SAC_LexicalUnit* SAC_lexical_unit_operator_slash(SAC_MPool mpool);
SAC_LexicalUnit* SAC_lexical_unit_operator_comma(SAC_MPool mpool);
SAC_LexicalUnit* SAC_lexical_unit_function(SAC_MPool mpool,
  SAC_STRING name, SAC_LexicalUnit **parameters);
SAC_LexicalUnit* SAC_lexical_unit_attr(SAC_MPool mpool,
  SAC_LexicalUnit **parameters);
SAC_LexicalUnit* SAC_lexical_unit_rgbcolor(SAC_MPool mpool,
  SAC_LexicalUnit **parameters);



#endif
