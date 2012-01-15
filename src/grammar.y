/*
 * References:
 *  - http://www.w3.org/TR/CSS21/grammar.html#grammar
 *  - http://www.w3.org/TR/css3-namespace/#syntax
 *
 *  - http://www.w3.org/TR/css3-fonts/
 */
%code requires {
#include "list.h"
#include "vector.h"
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

void yyerror(const char *msg) { printf("ERROR: %s\n", msg); }
extern int yylex();

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
  : sac_style_declarations_start sac_maybe_declarations {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | START_AS_SELECTORS selectors {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
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
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | mediums ',' maybe_spaces medium {
      $$ = $1;
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
    }
  ;
maybe_mediums
  : /* empty */ {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
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
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | selectors ',' maybe_spaces selector {
      $$ = $1;
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
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
    }
  | ',' maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_OPERATOR_COMMA);
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
      SAC_parser_start_style_handler(YY_SCANNER_PARSER(scanner), vector);
      $$ = vector;
    }
  ;
selector
  : simple_selector {
      $$ = $1;
    }
  | selector '+' maybe_spaces simple_selector {
      $$ = SAC_selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_DIRECT_ADJACENT_SELECTOR);
      $$->desc.sibling.nodeType = ANY_NODE;
      $$->desc.sibling.firstSelector = $1;
      $$->desc.sibling.secondSelector = $4;
    }
  | selector '>' maybe_spaces simple_selector {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_CHILD_SELECTOR);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $4;
    }
  | selector simple_selector {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_DESCENDANT_SELECTOR);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $2;
    }
  ;
simple_selector
  : attribute_conditions maybe_spaces {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_CONDITIONAL_SELECTOR);
      $$->desc.conditional.simpleSelector = SAC_selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
      $$->desc.conditional.condition = $1;
    }
  | element_name maybe_attribute_conditions maybe_spaces {
      if ($2 == NULL) {
        $$ = $1;
      } else {
        $$ = SAC_selector_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_CONDITIONAL_SELECTOR);
        $$->desc.conditional.simpleSelector = $1;
        $$->desc.conditional.condition = $2;
      }
    }
  ;
attribute_condition
  : HASH {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_ID_CONDITION);
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
      $$->desc.element.namespaceURI = NULL;
      $$->desc.element.localName = $1;
    }
  | '*' {
      $$ = SAC_selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
    }
  ;
attrib
  : '[' maybe_spaces IDENT maybe_spaces ']' {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_ATTRIBUTE_CONDITION);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $3;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
  | '[' maybe_spaces IDENT maybe_spaces attrib_match
    maybe_spaces attrib_value maybe_spaces ']'
    {
      $$ = SAC_condition_alloc(YY_SCANNER_MPOOL(scanner), $5);
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
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $2;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
  | ':' FUNCTION maybe_spaces maybe_indent ')'
  ;
sac_maybe_declaration
  : property ':' maybe_spaces expr maybe_prio {
      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1,
        SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner)),
        $5);
    }
  | /* empty */
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
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | expr maybe_operator term {
      $$ = $1;
      if ($2 != NULL) SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $2);
      SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $3);
    }
  ;
term
  : unary_operator INT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
      $$->desc.integer = $2;
    }
  | unary_operator REAL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_REAL);
      $$->desc.real = $2;
    }
  | unary_operator PERCENTAGE maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_PERCENTAGE);
      $$->desc.dimension.unit = "%";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PIXEL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PIXEL);
      $$->desc.dimension.unit = "px";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_INCH maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_INCH);
      $$->desc.dimension.unit = "in";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_CENTIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_LENGTH_CENTIMETER);
      $$->desc.dimension.unit = "cm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_MILLIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_LENGTH_MILLIMETER);
      $$->desc.dimension.unit = "mm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_POINT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_POINT);
      $$->desc.dimension.unit = "pt";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PICA maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PICA);
      $$->desc.dimension.unit = "pc";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EM maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EM);
      $$->desc.dimension.unit = "em";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EX maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EX);
      $$->desc.dimension.unit = "ex";
      $$->desc.dimension.value.sreal = $2;
    }
  | ANGLE_DEG maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DEGREE);
      $$->desc.dimension.unit = "deg";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_RAD maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RADIAN);
      $$->desc.dimension.unit = "rad";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_GRAD maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_GRADIAN);
      $$->desc.dimension.unit = "grad";
      $$->desc.dimension.value.ureal = $1;
    }
  | TIME_MS maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_MILLISECOND);
      $$->desc.dimension.unit = "ms";
      $$->desc.dimension.value.ureal = $1;
    }
  | TIME_S maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SECOND);
      $$->desc.dimension.unit = "s";
      $$->desc.dimension.value.ureal = $1;
    }
  | FREQ_HZ maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_HERTZ);
      $$->desc.dimension.unit = "Hz";
      $$->desc.dimension.value.ureal = $1;
    }
  | FREQ_KHZ maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_KILOHERTZ);
      $$->desc.stringValue = "kHz";
      $$->desc.dimension.value.ureal = $1;
    }
  | STRING maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_STRING_VALUE);
      $$->desc.stringValue = $1;
    }
  | IDENT maybe_spaces {
      if (strcmp($1, "inherit") != 0) {
        $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_IDENT);
        $$->desc.ident = $1;
      } else {
        $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INHERIT);
      }
    }
  | URI maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_URI);
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
      $$->desc.unicodeRange = $1;
    }
  ;
function
  : FUNCTION maybe_spaces expr ')' maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_FUNCTION);
      $$->desc.function.name = $1;

      $$->desc.function.parameters = SAC_vector_from_list(
        $3, YY_SCANNER_MPOOL(scanner));
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
        raw = (SAC_LexicalUnit**)$$->desc.function.parameters;

        raw[0] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[0]->desc.integer = r;

        raw[1] = SAC_lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);

        raw[2] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[2]->desc.integer = g;

        raw[3] = SAC_lexical_unit_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);

        raw[4] = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
        raw[4]->desc.integer = b;

      } else {
        $$->desc.function.parameters = SAC_vector_open(
          YY_SCANNER_MPOOL(scanner), 0
        );
      }
    }
  ;
