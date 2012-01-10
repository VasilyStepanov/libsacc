/*
 * Reference: http://www.w3.org/TR/css3-syntax/#grammar0
 * TODO: Replace this with something more relevant.
 */
%{
#include "parser.h"
#include "lexical_unit.h"

#include <sacc.h>

#include <string.h>
#include <stdlib.h>

#define YYLEX_PARAM scanner
#define YYPARSE_PARAM scanner

void yyerror() {}
extern int yylex();
%}

%union {
int val;
signed long integer;
double real;
char ch;
char *str;
SAC_LexicalUnit *value;
}

%locations
%pure_parser

%start stylesheet

%token <real> ANGLE_DEG
%token <real> ANGLE_RAD
%token <real> ANGLE_GRAD
%token <val> CDC
%token <val> CDO
%token <val> CHARSET_SYM
%token <val> DASHMATCH
%token <val> DIMEN
%token <real> LENGTH_EM
%token <real> LENGTH_EX
%token <val> FONT_FACE_SYM
%token <real> FREQ_HZ
%token <real> FREQ_KHZ
%token <val> FUNCTION
%token <val> HASH
%token <str> IDENT
%token <val> INCLUDES
%token <val> IMPORT_SYM
%token <val> IMPORTANT_SYM
%token <real> LENGTH_PIXEL
%token <real> LENGTH_CENTIMETER
%token <real> LENGTH_MILLIMETER
%token <real> LENGTH_INCH
%token <real> LENGTH_POINT
%token <real> LENGTH_PICA
%token <val> MEDIA_SYM
%token <val> NAMESPACE_SYM
%token <integer> INT
%token <real> REAL
%token <val> PAGE_SYM
%token <real> PERCENTAGE
%token <val> S
%token <str> STRING
%token <real> TIME_MS
%token <real> TIME_S
%token <str> URI
%token <str> UNICODERANGE

%type <str> property;
%type <value> expr;
%type <value> term;
%type <value> operator;
%type <value> _unary_term;
%type <ch> unary_operator;

%%

_spaces0
  :
  | _spaces0 S
  ;
_comments0
  :
  | _comments0 S
  | _comments0 CDO
  | _comments0 CDC
  ;
_stylesheet_values0
  :
  | _stylesheet_values0 _comments0 _stylesheet_value
  ;
_imports0
  :
  | _imports0 _comments0 import
  ;
_namespaces0
  :
  | _namespaces0 _comments0 namespace
  ;
_mediums1
  : medium
  | _mediums1 ',' _spaces0 medium
  ;
_rulesets0
  :
  | _rulesets0 ruleset
  ;
_selectors1
  : selector
  | _selectors1 ',' _spaces0 selector
  ;
_declarations1
  : declaration
  | _declarations1 ';' _spaces0 declaration
  ;
_simple_selector_values0
  :
  | _simple_selector_values0 _simple_selector_value
  ;
_simple_selector_values1
  : _simple_selector_value
  | _simple_selector_values1 _simple_selector_value
  ;

stylesheet
  : _comments0 _imports0 _namespaces0 _stylesheet_values0
  | CHARSET_SYM _spaces0 STRING _spaces0 ';' _comments0 _imports0 _namespaces0 _stylesheet_values0
  ;
_stylesheet_value
  : ruleset
  | media
  | page
  | font_face
  ;
import
  : IMPORT_SYM _spaces0 _string_or_uri _spaces0 ';' _spaces0
  | IMPORT_SYM _spaces0 _string_or_uri _spaces0 _mediums1 ';' _spaces0
  ;
namespace
  : NAMESPACE_SYM _spaces0 _string_or_uri _spaces0 ';' _spaces0
  | NAMESPACE_SYM _spaces0 namespace_prefix _spaces0 _string_or_uri _spaces0 ';' _spaces0
  ;
_string_or_uri
  : STRING
  | URI
  ;
namespace_prefix
  : IDENT
  ;
media
  : MEDIA_SYM _spaces0 _mediums1 '{' _spaces0 _rulesets0 '}' _spaces0
  ;
medium
  : IDENT _spaces0
  ;
page
  : PAGE_SYM _spaces0 _ident_or_space _pseudo_page_or_space _spaces0 '{' _spaces0 _declarations1 '}' _spaces0
  ;
_ident_or_space
  :
  | IDENT
  ;
_pseudo_page_or_space
  :
  | pseudo_page
  ;
pseudo_page
  : ':' IDENT
  ;
font_face
  : FONT_FACE_SYM _spaces0 '{' _spaces0 _declarations1 '}' _spaces0
  ;
operator
  : '/' _spaces0 { $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_SLASH); }
  | ',' _spaces0 { $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA); }
  | /* empty */ { $$ = NULL; }
  ;
combinator
  : '+' _spaces0
  | '>' _spaces0
  | /* empty */
  ;
unary_operator
  : '-' { $$ = '-'; }
  | '+' { $$ = '+'; }
  | /* empty */ { $$ = '+'; }
  ;
property
  : IDENT _spaces0 { $$ = $1; }
  ;
ruleset
  : _selectors1 '{' _spaces0 _declarations1 '}' _spaces0
  ;
selector
  : simple_selector
  | selector combinator simple_selector
  ;
simple_selector
  : _simple_selector_values1 _spaces0
  | element_name _simple_selector_values0 _spaces0
  ;
_simple_selector_value
  : HASH
  | class
  | attrib
  | pseudo
  ;
class
  : '.' IDENT
  ;
element_name
  : IDENT
  | '*'
  ;
attrib
  : '[' _spaces0 IDENT _spaces0 ']'
  | '[' _spaces0 IDENT _spaces0 _attrib_match _spaces0 _attrib_value _spaces0 ']'
  ;
_attrib_match
  : '='
  | INCLUDES
  | DASHMATCH
  ;
_attrib_value
  : IDENT
  | STRING
  ;
pseudo
  : ':' IDENT
  | ':' FUNCTION _spaces0 IDENT _spaces0 ')'
  ;
declaration
  : property ':' _spaces0 expr {
      parser_property_handler(YY_SCANNER_PARSER(scanner), $1, $4, SAC_FALSE);
    }
  | property ':' _spaces0 expr prio {
      parser_property_handler(YY_SCANNER_PARSER(scanner), $1, $4, SAC_TRUE);
    }
  | /* empty */
  ;
prio
  : IMPORTANT_SYM _spaces0
  ;
expr
  : term { $$ = $1; }
  | expr operator term {
      size_t old_size;
      size_t new_size;
      
      if ($1->lexicalUnitType == SAC_SUB_EXPRESSION) {
        old_size = lexical_unit_array_size($1->desc.subValues);
      } else {
        old_size = 1;
      }

      new_size = old_size + 1;
      if ($2 != NULL) ++new_size;

      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SUB_EXPRESSION);
      if ($1->lexicalUnitType == SAC_SUB_EXPRESSION) {
        $$->desc.subValues = lexical_unit_array_alloc(
          YY_SCANNER_MPOOL(scanner), new_size
        );
        lexical_unit_array_cpy($$->desc.subValues, $1->desc.subValues);
      } else {
        $$->desc.subValues = lexical_unit_array_alloc(
          YY_SCANNER_MPOOL(scanner), new_size
        );
        $$->desc.subValues[0] = $1;
      }

      if ($2 != NULL) $$->desc.subValues[new_size - 2] = $2;
      $$->desc.subValues[new_size - 1] = $3;
    }
  ;
term
  : unary_operator _unary_term
  | STRING _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_STRING_VALUE);
      $$->desc.stringValue = $1;
    }
  | IDENT _spaces0 {
      if (strcmp($1, "inherit") != 0) {
        $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_IDENT);
        $$->desc.ident = $1;
      } else {
        $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INHERIT);
      }
    }
  | URI _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_URI);
      $$->desc.uri = $1;
    }
  | UNICODERANGE _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_UNICODERANGE);
      $$->desc.unicodeRange = $1;
    }
  | hexcolor
  | FREQ_HZ _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_HERTZ);
      $$->desc.dimension.unit = "Hz";
      $$->desc.dimension.value.ureal = $1;
    }
  | FREQ_KHZ _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_KILOHERTZ);
      $$->desc.stringValue = "kHz";
      $$->desc.dimension.value.ureal = $1;
    }
  | unary_operator INT _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
      $$->desc.integer = $2;
    }
  | unary_operator REAL _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_REAL);
      $$->desc.real = $2;
    }
  | unary_operator PERCENTAGE _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_PERCENTAGE);
      $$->desc.dimension.unit = "%";
      $$->desc.dimension.value.sreal = $2;
    }
  | TIME_MS _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_MILLISECOND);
      $$->desc.dimension.unit = "ms";
      $$->desc.dimension.value.ureal = $1;
    }
  | TIME_S _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SECOND);
      $$->desc.dimension.unit = "s";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_DEG _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DEGREE);
      $$->desc.dimension.unit = "deg";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_RAD _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RADIAN);
      $$->desc.dimension.unit = "rad";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_GRAD _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_GRADIAN);
      $$->desc.dimension.unit = "grad";
      $$->desc.dimension.value.ureal = $1;
    }
  | unary_operator LENGTH_EM _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EM);
      $$->desc.dimension.unit = "em";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EX _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EX);
      $$->desc.dimension.unit = "ex";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PIXEL _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PIXEL);
      $$->desc.dimension.unit = "px";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_INCH _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_INCH);
      $$->desc.dimension.unit = "in";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_CENTIMETER _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_CENTIMETER);
      $$->desc.dimension.unit = "cm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_MILLIMETER _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_MILLIMETER);
      $$->desc.dimension.unit = "mm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_POINT _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_POINT);
      $$->desc.dimension.unit = "pt";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PICA _spaces0 {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PICA);
      $$->desc.dimension.unit = "pc";
      $$->desc.dimension.value.sreal = $2;
    }
  ;
_unary_term
  : function
  ;
function
  : FUNCTION _spaces0 expr ')' _spaces0
  ;
/*
 * There is a constraint on the color that it must
 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
 */
hexcolor
  : HASH _spaces0
  ;
