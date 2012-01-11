/*
 * Reference: http://www.w3.org/TR/css3-syntax/#grammar0
 * TODO: Replace this with something more relevant.
 */
%code requires {
#include "list.h"
#include "vector.h"
#include "declaration.h"
#include "lexical_unit.h"
#include "mpool.h"
}

%{
#include "parser.h"

#include <sacc.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
declaration_t decl;
list_t list;
SAC_Selector *selector;
}

%locations
%pure_parser

%start start

%token <val> START_AS_STYLE_DECLARATIONS
%token <val> START_AS_SELECTORS
%token <val> START_AS_STYLESHEET

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
%token <str> FUNCTION
%token <str> HASH
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
%type <ch> unary_operator;
%type <value> expr;
%type <value> term;
%type <value> operator;
%type <value> function;
%type <value> hexcolor;
%type <decl> declaration;
%type <list> _declarations1;
%type <list> _selectors1;
%type <selector> selector;

%%

start
  : START_AS_STYLE_DECLARATIONS _declarations1 {
      list_iter_t it;

      parser_start_document(YY_SCANNER_PARSER(scanner));
      for (it = list_head($2); it != NULL; it = list_next(it)) {
        declaration_t decl = *it;

        parser_property_handler(
          YY_SCANNER_PARSER(scanner),
          decl->property, decl->value, decl->important);
      }
      parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | START_AS_SELECTORS _selectors1 {
      size_t size = 0;
      list_iter_t lit;
      vector_t v;
      vector_iter_t vit;

      parser_start_document(YY_SCANNER_PARSER(scanner));
      for (lit = list_head($2); lit != NULL; lit = list_next(lit)) ++size;
      v = vector_open(YY_SCANNER_MPOOL(scanner), size);

      for (lit = list_head($2), vit = v;
           lit != NULL;
           lit = list_next(lit), ++vit)
      {
        *vit = *lit;
      }
      parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = v;
    }
  | START_AS_STYLESHEET stylesheet
  ;

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
  : selector {
      $$ = list_open(YY_SCANNER_MPOOL(scanner));
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | _selectors1 ',' _spaces0 selector {
      $$ = $1;
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
    }
  ;
_declarations1
  : declaration {
      $$ = list_open(YY_SCANNER_MPOOL(scanner));
      if ($1 != NULL) list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | _declarations1 ';' _spaces0 declaration {
      $$ = $1;
      if ($4 != NULL) list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
    }
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
  : '/' _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_SLASH);
    }
  | ',' _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);
    }
  | /* empty */ {
      $$ = NULL;
    }
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
  : simple_selector {
      $$ = NULL;
    }
  | selector combinator simple_selector {
      $$ = NULL;
    }
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
      $$ = declaration_alloc(YY_SCANNER_MPOOL(scanner), $1, $4, SAC_FALSE);
    }
  | property ':' _spaces0 expr prio {
      $$ = declaration_alloc(YY_SCANNER_MPOOL(scanner), $1, $4, SAC_TRUE);
    }
  | /* empty */ {
      $$ = NULL;
    }
  ;
prio
  : IMPORTANT_SYM _spaces0
  ;
expr
  : term { $$ = $1; }
  | expr operator term {
      size_t old_size;
      size_t new_size;
      SAC_LexicalUnit **raw;
      
      if ($1->lexicalUnitType == SAC_SUB_EXPRESSION) {
        old_size = vector_size($1->desc.subValues);
      } else {
        old_size = 1;
      }

      new_size = old_size + 1;
      if ($2 != NULL) ++new_size;

      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SUB_EXPRESSION);
      if ($1->lexicalUnitType == SAC_SUB_EXPRESSION) {
        $$->desc.subValues = vector_open(YY_SCANNER_MPOOL(scanner), new_size);
        vector_cpy(
          (vector_iter_t)$$->desc.subValues, (vector_iter_t)$1->desc.subValues);
        raw = (SAC_LexicalUnit**)$$->desc.subValues;
      } else {
        $$->desc.subValues = vector_open(YY_SCANNER_MPOOL(scanner), new_size);
        raw = (SAC_LexicalUnit**)$$->desc.subValues;
        raw[0] = $1;
      }

      if ($2 != NULL) raw[new_size - 2] = $2;
      raw[new_size - 1] = $3;
    }
  ;
term
  : STRING _spaces0 {
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
  | function { $$ = $1; }
  | hexcolor { $$ = $1; }
  ;
function
  : FUNCTION _spaces0 expr ')' _spaces0 {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_FUNCTION);
      $$->desc.function.name = $1;

      if ($3->lexicalUnitType == SAC_SUB_EXPRESSION) {
        $$->desc.function.parameters = $3->desc.subValues;
      } else {
        $$->desc.function.parameters = vector_open(
          YY_SCANNER_MPOOL(scanner), 1
        );
        $$->desc.function.parameters[0] = $3;
      }
    }
  ;
/*
 * There is a constraint on the color that it must
 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
 */
hexcolor
  : HASH _spaces0 {
      unsigned int r, g, b;
      int ok;
      size_t len;

      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RGBCOLOR);
      $$->desc.function.name = "rgb";

      len = strlen($1);
      
      if (len == 6) {
        ok = sscanf($1, "%2x%2x%2x", &r, &g, &b) == 3;
      } else if (len == 3) {
        ok = sscanf($1, "%1x%1x%1x", &r, &g, &b) == 3;
      } else {
        ok = 0;
      }

      if (ok) {
        SAC_LexicalUnit **raw;
        
        $$->desc.function.parameters = vector_open(
          YY_SCANNER_MPOOL(scanner), 5);
        raw = (SAC_LexicalUnit**)$$->desc.function.parameters;

        raw[0] = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[0]->desc.integer = r;

        raw[1] = lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);

        raw[2] = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[2]->desc.integer = g;

        raw[3] = lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);

        raw[4] = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[4]->desc.integer = b;

      } else {
        $$->desc.function.parameters = vector_open(
          YY_SCANNER_MPOOL(scanner), 0
        );
      }
    }
  ;
