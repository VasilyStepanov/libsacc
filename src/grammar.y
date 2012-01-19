/*
 * References:
 *  - http://www.w3.org/TR/CSS21/grammar.html#grammar
 *  - http://www.w3.org/TR/css3-namespace/#syntax
 *
 *  - http://www.w3.org/TR/css3-fonts/
 */
%code requires {
#include <strings.h>

#include "list.h"
#include "vector.h"
#include "vector_extra.h"
#include "lexical_unit.h"
#include "condition.h"
#include "selector.h"
#include "mpool.h"
#include "pair.h"
}

%{
#include "parser.h"

#include <sacc.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define YYLEX_PARAM scanner
#define YYPARSE_PARAM scanner

int yyerror(const char *data) { return 0; }
extern int yylex();

#define TEST_OBJ(obj, loc) \
  do { \
    if ((obj) == NULL) { \
      SAC_parser_fatal_error_handler(YY_SCANNER_PARSER(scanner), \
        (loc).first_line, (loc).first_column, SAC_FATAL_ERROR_NO_MEMORY); \
      YYABORT; \
    } \
  } while (0)

#define SAC_ERROR(loc, type, data) \
  SAC_parser_error_handler(YY_SCANNER_PARSER(scanner), \
    loc.first_line, loc.first_column, type, data)

#define SAC_SYNTAX_ERROR(loc, data) \
  SAC_ERROR(loc, SAC_ERROR_SYNTAX, data)
  

%}

%union {
int val;
signed long integer;
double real;
char ch;
char *str;
SAC_LexicalUnit *value;
SAC_List list;
SAC_Vector vector;
SAC_Selector *sel;
SAC_Condition *cond;
SAC_ConditionType cond_type;
SAC_Boolean boolean;
SAC_Pair pair;
}

%locations
%pure_parser

%start start

%token <val> START_AS_STYLE_DECLARATIONS
%token <val> START_AS_SELECTORS
%token <val> START_AS_STYLESHEET
%token <val> START_AS_RULE
%token <val> START_AS_PROPERTY_VALUE
%token <val> START_AS_PRIORITY

%token <real> ANGLE_DEG
%token <real> ANGLE_RAD
%token <real> ANGLE_GRAD
%token <val> CDC
%token <val> CDO
%token <val> CHARSET_SYM
%token <val> DASHMATCH
%token <val> DIMEN
%token <val> BAD_STRING
%token <val> BAD_URI
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
%type <str> attrib_value;
%type <str> string_or_uri;
%type <str> medium;
%type <str> maybe_namespace_prefix;
%type <str> maybe_indent;
%type <str> maybe_pseudo_page;
%type <ch> unary_operator;
%type <value> term;
%type <value> maybe_operator;
%type <value> function;
%type <value> hexcolor;
%type <list> selectors;
%type <list> expr;
%type <list> mediums;
%type <list> maybe_mediums;
%type <vector> sac_style_start;
%type <vector> sac_media_start;
%type <sel> selector;
%type <sel> simple_selector;
%type <sel> element_name;
%type <cond> attribute_condition;
%type <cond> attribute_conditions;
%type <cond> maybe_attribute_conditions;
%type <cond> class;
%type <cond> attrib;
%type <cond> pseudo;
%type <cond_type> attrib_match;
%type <boolean> maybe_prio;
%type <pair> sac_page_start;

%%

start
  : START_AS_PRIORITY maybe_prio {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = (void*)$2;
    }
  | START_AS_PROPERTY_VALUE expr {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_lexical_unit_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | sac_style_declarations_start sac_maybe_declarations {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | START_AS_SELECTORS selectors {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | sac_rule_start sac_ruleset maybe_spaces {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | sac_stylesheet_start sac_stylesheet {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  ;

sac_style_declarations_start
  : START_AS_STYLE_DECLARATIONS {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
sac_rule_start
  : START_AS_RULE {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
sac_stylesheet_start
  : START_AS_STYLESHEET {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;

maybe_spaces
  :
  | maybe_spaces S
  ;
maybe_comments
  :
  | maybe_comments S
  | maybe_comments CDO
  | maybe_comments CDC
  ;
sac_maybe_style_units
  : /* empty */
  | sac_maybe_style_units sac_style_unit
  ;
sac_maybe_imports
  :
  | sac_maybe_imports sac_import
  ;
sac_maybe_namespaces
  :
  | sac_maybe_namespaces sac_namespace
  ;
mediums
  : medium {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | mediums ',' maybe_spaces medium {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  ;
maybe_mediums
  : /* empty */ {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | mediums {
      $$ = $1;
    }
  ;
sac_maybe_rulesets
  :
  | sac_maybe_rulesets sac_ruleset maybe_spaces
  ;
selectors
  : selector {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | selectors ',' maybe_spaces selector {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  ;
sac_maybe_declarations
  : sac_maybe_declaration 
  | sac_maybe_declarations ';' maybe_spaces sac_maybe_declaration
  ;
maybe_attribute_conditions
  : /* empty */ {
      $$ = NULL;
    }
  | attribute_conditions {
      $$ = $1;
    }
  ;
attribute_conditions
  : attribute_condition {
      $$ = $1;
    }
  | attribute_conditions attribute_condition {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_AND_CONDITION);
      TEST_OBJ($$, @$);
      $$->desc.combinator.firstCondition = $1;
      $$->desc.combinator.secondCondition = $2;
    }
  ;
sac_stylesheet
  : maybe_charset sac_maybe_imports sac_maybe_namespaces sac_maybe_style_units
  ;
maybe_charset
  :
  | charset
  ;
charset
  : CHARSET_SYM maybe_spaces STRING maybe_spaces ';' maybe_comments
  ;
sac_style_unit
  : sac_ruleset maybe_comments
  | sac_media maybe_comments
  | sac_page maybe_comments
  | sac_font_face maybe_comments
  ;
sac_import
  : IMPORT_SYM maybe_spaces string_or_uri maybe_mediums ';' maybe_comments {
      SAC_Vector mediums;

      mediums = SAC_vector_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(mediums, @4);
      SAC_parser_import_handler(YY_SCANNER_PARSER(scanner), $3, mediums, NULL);
    }
  ;
sac_namespace
  : NAMESPACE_SYM maybe_spaces maybe_namespace_prefix string_or_uri
    ';' maybe_comments
    {
      SAC_parser_namespace_declaration_handler(YY_SCANNER_PARSER(scanner),
        $3, $4);
    }
  ;
string_or_uri
  : STRING maybe_spaces {
      $$ = $1;
    }
  | URI maybe_spaces {
      $$ = $1;
    }
  ;
maybe_namespace_prefix
  : /* empty */ {
      $$ = NULL;
    }
  | IDENT maybe_spaces {
      $$ = $1;
    }
  ;
sac_media
  : sac_media_start '{' maybe_spaces sac_maybe_rulesets '}' {
      SAC_parser_end_media_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  ;
sac_media_start
  : MEDIA_SYM maybe_spaces mediums {
      SAC_Vector mediums;

      mediums = SAC_vector_from_list($3, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(mediums, @3);
      SAC_parser_start_media_handler(YY_SCANNER_PARSER(scanner), mediums);
      $$ = mediums;
    }
  ;
medium
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
sac_page
  : sac_page_start '{' maybe_spaces sac_maybe_declarations '}' {
      SAC_parser_end_page_handler(YY_SCANNER_PARSER(scanner),
        $1.first, $1.second);
    }
  ;
sac_page_start
  : PAGE_SYM maybe_spaces maybe_indent maybe_pseudo_page {
      SAC_parser_start_page_handler(YY_SCANNER_PARSER(scanner), $3, $4);
      $$.first = $3;
      $$.second = $4;
    }
  ;
maybe_indent
  : /* empty */ {
      $$ = NULL;
    }
  | IDENT maybe_spaces {
      $$ = $1;
    }
  ;
maybe_pseudo_page
  : /* empty */ {
      $$ = NULL;
    }
  | ':' IDENT maybe_spaces {
      $$ = $2;
    }
  ;
sac_font_face
  : sac_font_face_start '{' maybe_spaces sac_maybe_declarations '}' {
      SAC_parser_end_font_face_handler(YY_SCANNER_PARSER(scanner));
    }
  ;
sac_font_face_start
  : FONT_FACE_SYM maybe_spaces {
      SAC_parser_start_font_face_handler(YY_SCANNER_PARSER(scanner));
    }
  ;
maybe_operator
  : '/' maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_OPERATOR_SLASH);
      TEST_OBJ($$, @$);
    }
  | ',' maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_OPERATOR_COMMA);
      TEST_OBJ($$, @$);
    }
  | /* empty */ {
      $$ = NULL;
    }
  ;
unary_operator
  : '-' {
      $$ = '-';
    }
  | '+' {
      $$ = '+';
    }
  | /* empty */ {
      $$ = '+';
    }
  ;
property
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
sac_ruleset
  : sac_style_start '{' maybe_spaces sac_maybe_declarations '}' {
      SAC_parser_end_style_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  ;
sac_style_start
  : selectors {
      SAC_Vector vector;
      
      vector = SAC_vector_from_list($1, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(vector, @1);
      SAC_parser_start_style_handler(YY_SCANNER_PARSER(scanner), vector);
      $$ = vector;
    }
  ;
selector
  : simple_selector {
      $$ = $1;
    }
  | selector S {
      $$ = $1;
    }
  | selector S simple_selector {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_DESCENDANT_SELECTOR);
      TEST_OBJ($$, @$);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $3;
    }
  | selector '+' maybe_spaces simple_selector {
      $$ = SAC_selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_DIRECT_ADJACENT_SELECTOR);
      TEST_OBJ($$, @$);
      $$->desc.sibling.nodeType = ANY_NODE;
      $$->desc.sibling.firstSelector = $1;
      $$->desc.sibling.secondSelector = $4;
    }
  | selector '>' maybe_spaces simple_selector {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_CHILD_SELECTOR);
      TEST_OBJ($$, @$);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $4;
    }
  ;
simple_selector
  : attribute_conditions {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_CONDITIONAL_SELECTOR);
      TEST_OBJ($$, @$);
      $$->desc.conditional.simpleSelector = SAC_selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
      TEST_OBJ($$->desc.conditional.simpleSelector, @$);
      $$->desc.conditional.condition = $1;
    }
  | element_name maybe_attribute_conditions {
      if ($2 == NULL) {
        $$ = $1;
      } else {
        $$ = SAC_selector_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_CONDITIONAL_SELECTOR);
        TEST_OBJ($$, @$);
        $$->desc.conditional.simpleSelector = $1;
        $$->desc.conditional.condition = $2;
      }
    }
  ;
attribute_condition
  : HASH {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_ID_CONDITION);
      TEST_OBJ($$, @$);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = "id";
      $$->desc.attribute.specified = SAC_TRUE;
      $$->desc.attribute.value = $1;
    }
  | class {
      $$ = $1;
    }
  | attrib {
      $$ = $1;
    }
  | pseudo {
      $$ = $1;
    }
  ;
class
  : '.' IDENT {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_CLASS_CONDITION);
      TEST_OBJ($$, @$);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = "class";
      $$->desc.attribute.specified = SAC_TRUE;
      $$->desc.attribute.value = $2;
    }
  ;
element_name
  : IDENT {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_ELEMENT_NODE_SELECTOR);
      TEST_OBJ($$, @$);
      $$->desc.element.namespaceURI = NULL;
      $$->desc.element.localName = $1;
    }
  | '*' {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
      TEST_OBJ($$, @$);
    }
  ;
attrib
  : '[' maybe_spaces IDENT maybe_spaces ']' {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_ATTRIBUTE_CONDITION);
      TEST_OBJ($$, @$);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $3;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
  | '[' maybe_spaces IDENT maybe_spaces attrib_match
    maybe_spaces attrib_value maybe_spaces ']'
    {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), $5);
      TEST_OBJ($$, @$);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $3;
      $$->desc.attribute.specified = SAC_TRUE;
      $$->desc.attribute.value = $7;
    }
  ;
attrib_match
  : '=' {
      $$ = SAC_ATTRIBUTE_CONDITION;
    }
  | INCLUDES {
      $$ = SAC_ONE_OF_ATTRIBUTE_CONDITION;
    }
  | DASHMATCH {
      $$ = SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION;
    }
  ;
attrib_value
  : IDENT {
      $$ = $1;
    }
  | STRING {
      $$ = $1;
    }
  ;
pseudo
  : ':' IDENT {
      $$ = SAC_condition_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_PSEUDO_CLASS_CONDITION);
      TEST_OBJ($$, @$);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $2;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
  | ':' FUNCTION maybe_spaces maybe_indent ')'
  ;
sac_maybe_declaration
  : property ':' maybe_spaces expr maybe_prio {
      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5);
    }
  | /* empty */
  | property error {
      SAC_SYNTAX_ERROR(@2,
        "colon expected");
      yyclearin;
    }
  | property ':' maybe_spaces expr maybe_prio error {
      /* p {color: red !important fail;} */

      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5);

      SAC_SYNTAX_ERROR(@6,
        "unexpected token after property expression");
      yyclearin;
    }
  | IMPORTANT_SYM maybe_spaces {
      /* div { text-align: center; !important } */

      SAC_SYNTAX_ERROR(@1,
        "unexpected 'important' token while parsing property expression");
    }
  | property ':' maybe_spaces {
      /* div { font-family: } */

      SAC_SYNTAX_ERROR(@3,
        "property expression expected");
    }
  | property ':' maybe_spaces error {
      /* p { weight: *; } */

      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing property expression");
      yyclearin;
    }
  | property invalid_block {
      /* div { color{;color:maroon} } */

      SAC_SYNTAX_ERROR(@2,
        "expected property expression");
    }
  ;
maybe_prio
  : IMPORTANT_SYM maybe_spaces {
      $$ = SAC_TRUE;
    }
  | /* empty */ {
      $$ = SAC_FALSE;
    }
  ;
expr
  : term {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | expr maybe_operator term {
      $$ = $1;
      if ($2 != NULL)
        TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $2), @2);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $3), @3);
    }
  ;
term
  : unary_operator INT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
      TEST_OBJ($$, @$);
      $$->desc.integer = $2;
    }
  | unary_operator REAL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_REAL);
      TEST_OBJ($$, @$);
      $$->desc.real = $2;
    }
  | unary_operator PERCENTAGE maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_PERCENTAGE);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "%";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PIXEL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PIXEL);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "px";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_INCH maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_INCH);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "in";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_CENTIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_LENGTH_CENTIMETER);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "cm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_MILLIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_LENGTH_MILLIMETER);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "mm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_POINT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_POINT);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "pt";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PICA maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PICA);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "pc";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EM maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EM);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "em";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EX maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EX);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "ex";
      $$->desc.dimension.value.sreal = $2;
    }
  | ANGLE_DEG maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DEGREE);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "deg";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_RAD maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RADIAN);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "rad";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_GRAD maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_GRADIAN);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "grad";
      $$->desc.dimension.value.ureal = $1;
    }
  | unary_operator TIME_MS maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative time not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_MILLISECOND);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "ms";
      $$->desc.dimension.value.ureal = $2;
    }
  | unary_operator TIME_S maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative time not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SECOND);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "s";
      $$->desc.dimension.value.ureal = $2;
    }
  | unary_operator FREQ_HZ maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative frequency not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_HERTZ);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "Hz";
      $$->desc.dimension.value.ureal = $2;
    }
  | unary_operator FREQ_KHZ maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative frequency not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_KILOHERTZ);
      TEST_OBJ($$, @$);
      $$->desc.stringValue = "kHz";
      $$->desc.dimension.value.ureal = $2;
    }
  | STRING maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_STRING_VALUE);
      TEST_OBJ($$, @$);
      $$->desc.stringValue = $1;
    }
  | IDENT maybe_spaces {
      if (strcasecmp($1, "inherit") != 0) {
        $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_IDENT);
        TEST_OBJ($$, @$);
        $$->desc.ident = $1;
      } else {
        $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INHERIT);
        TEST_OBJ($$, @$);
      }
    }
  | URI maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_URI);
      TEST_OBJ($$, @$);
      $$->desc.uri = $1;
    }
  | function {
      $$ = $1;
    }
  | hexcolor {
      $$ = $1;
    }
  | UNICODERANGE maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_UNICODERANGE);
      TEST_OBJ($$, @$);
      $$->desc.unicodeRange = $1;
    }
  ;
function
  : FUNCTION maybe_spaces expr ')' maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_FUNCTION);
      TEST_OBJ($$, @$);
      $$->desc.function.name = $1;

      $$->desc.function.parameters = SAC_vector_from_list(
        $3, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$->desc.function.parameters, @3);
    }
  ;
/*
 * There is a constraint on the color that it must
 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
 */
hexcolor
  : HASH maybe_spaces {
      unsigned int r, g, b;
      int ok;
      size_t len;

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RGBCOLOR);
      TEST_OBJ($$, @$);
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
        
        $$->desc.function.parameters = SAC_vector_open(
          YY_SCANNER_MPOOL(scanner), 5);
        TEST_OBJ($$->desc.function.parameters, @$);
        raw = (SAC_LexicalUnit**)$$->desc.function.parameters;

        raw[0] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        TEST_OBJ(raw[0], @$);
        raw[0]->desc.integer = r;

        raw[1] = SAC_lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);
        TEST_OBJ(raw[1], @$);

        raw[2] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        TEST_OBJ(raw[2], @$);
        raw[2]->desc.integer = g;

        raw[3] = SAC_lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);
        TEST_OBJ(raw[3], @$);

        raw[4] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        TEST_OBJ(raw[4], @$);
        raw[4]->desc.integer = b;

      } else {
        $$->desc.function.parameters = SAC_vector_open(
          YY_SCANNER_MPOOL(scanner), 0
        );
        TEST_OBJ($$->desc.function.parameters, @$);
      }
    }
  ;
invalid_block
  : '{' invalid_blocks error '}'
  | '{' error '}'
  ;
invalid_blocks
  : invalid_block
  | invalid_blocks error invalid_block
  ;
