/*
 * Reference: http://www.w3.org/TR/css3-syntax/#grammar0
 * TODO: Replace this with something more relevant.
 */
%code requires {
#include "list.h"
#include "vector.h"
#include "declaration.h"
#include "rule.h"
#include "lexical_unit.h"
#include "condition.h"
#include "selector.h"
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
SAC_Declaration *decl;
SAC_Rule *rule;
list_t list;
SAC_Selector *sel;
SAC_Condition *cond;
SAC_ConditionType cond_type;
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
%type <ch> unary_operator;
%type <value> term;
%type <value> operator;
%type <value> function;
%type <value> hexcolor;
%type <decl> declaration;
%type <rule> ruleset;
%type <list> declarations;
%type <list> selectors;
%type <list> expr;
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

%%

start
  : START_AS_STYLE_DECLARATIONS declarations {
      list_iter_t it;

      parser_start_document(YY_SCANNER_PARSER(scanner));
      for (it = list_head($2); it != NULL; it = list_next(it)) {
        SAC_Declaration *decl = *it;

        parser_property_handler(
          YY_SCANNER_PARSER(scanner),
          decl->property, decl->value, decl->important);
      }
      parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | START_AS_SELECTORS selectors {
      SAC_Vector v;
      list_iter_t lit;
      SAC_VectorIter vit;

      parser_start_document(YY_SCANNER_PARSER(scanner));
      v = vector_open(YY_SCANNER_MPOOL(scanner), list_size($2));

      for (lit = list_head($2), vit = vector_head(v);
           lit != NULL;
           lit = list_next(lit), ++vit)
      {
        *vit = *lit;
      }
      parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = v;
    }
  | START_AS_RULE ruleset maybe_spaces {
      list_iter_t lit;

      parser_start_document(YY_SCANNER_PARSER(scanner));
      parser_start_style_handler(YY_SCANNER_PARSER(scanner), $2->selectors);
      for (lit = list_head($2->declarations);
           lit != NULL;
           lit = list_next(lit))
      {
        SAC_Declaration *declaration = *lit;
        parser_property_handler(YY_SCANNER_PARSER(scanner),
          declaration->property, declaration->value, declaration->important);
      }
      parser_end_style_handler(YY_SCANNER_PARSER(scanner), $2->selectors);
      parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | START_AS_STYLESHEET stylesheet
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
maybe_stylesheet_values
  :
  | maybe_stylesheet_values stylesheet_value
  ;
maybe_imports
  :
  | maybe_imports import
  ;
maybe_namespaces
  :
  | maybe_namespaces namespace
  ;
mediums
  : medium
  | mediums ',' maybe_spaces medium
  ;
maybe_rulesets
  :
  | maybe_rulesets ruleset maybe_spaces
  ;
selectors
  : selector {
      $$ = list_open(YY_SCANNER_MPOOL(scanner));
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | selectors ',' maybe_spaces selector {
      $$ = $1;
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
    }
  ;
declarations
  : declaration {
      $$ = list_open(YY_SCANNER_MPOOL(scanner));
      if ($1 != NULL) list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | declarations ';' maybe_spaces declaration {
      $$ = $1;
      if ($4 != NULL) list_push_back($$, YY_SCANNER_MPOOL(scanner), $4);
    }
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
      $$ = condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_AND_CONDITION);
      $$->desc.combinator.firstCondition = $1;
      $$->desc.combinator.secondCondition = $2;
    }
  ;
stylesheet
  : maybe_charset maybe_imports maybe_namespaces maybe_stylesheet_values 
  ;
maybe_charset
  :
  | charset
  ;
charset
  : CHARSET_SYM maybe_spaces STRING maybe_spaces ';' maybe_comments
  ;
stylesheet_value
  : ruleset maybe_comments
  | media maybe_comments
  | page maybe_comments
  | font_face maybe_comments
  ;
import
  : IMPORT_SYM maybe_spaces string_or_uri maybe_spaces ';' maybe_comments
  | IMPORT_SYM maybe_spaces string_or_uri maybe_spaces mediums ';' maybe_comments
  ;
namespace
  : NAMESPACE_SYM maybe_spaces string_or_uri maybe_spaces ';' maybe_comments
  | NAMESPACE_SYM maybe_spaces namespace_prefix maybe_spaces string_or_uri maybe_spaces ';' maybe_comments
  ;
string_or_uri
  : STRING
  | URI
  ;
namespace_prefix
  : IDENT
  ;
media
  : MEDIA_SYM maybe_spaces mediums '{' maybe_spaces maybe_rulesets '}'
  ;
medium
  : IDENT maybe_spaces
  ;
page
  : PAGE_SYM maybe_spaces maybe_indent maybe_pseudo_page maybe_spaces '{' maybe_spaces declarations '}'
  ;
maybe_indent
  :
  | IDENT
  ;
maybe_pseudo_page
  :
  | pseudo_page
  ;
pseudo_page
  : ':' IDENT
  ;
font_face
  : FONT_FACE_SYM maybe_spaces '{' maybe_spaces declarations '}'
  ;
operator
  : '/' maybe_spaces {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_SLASH);
    }
  | ',' maybe_spaces {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_OPERATOR_COMMA);
    }
  | /* empty */ {
      $$ = NULL;
    }
  ;
unary_operator
  : '-' { $$ = '-'; }
  | '+' { $$ = '+'; }
  | /* empty */ { $$ = '+'; }
  ;
property
  : IDENT maybe_spaces { $$ = $1; }
  ;
ruleset
  : selectors '{' maybe_spaces declarations '}' {
      list_iter_t lit;
      SAC_Vector vector;
      SAC_VectorIter vit;

      vector = vector_open(YY_SCANNER_MPOOL(scanner), list_size($1));
      for (lit = list_head($1),
           vit = vector_head(vector);
           lit != NULL;
           lit = list_next(lit),
           ++vit)
      {
        *vit = *lit;
      }

      
      $$ = rule_alloc(YY_SCANNER_MPOOL(scanner), vector, $4);
    }
  ;
selector
  : simple_selector {
      $$ = $1;
    }
  | selector '+' maybe_spaces simple_selector {
      $$ = selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_DIRECT_ADJACENT_SELECTOR);
      $$->desc.sibling.nodeType = ANY_NODE;
      $$->desc.sibling.firstSelector = $1;
      $$->desc.sibling.secondSelector = $4;
    }
  | selector '>' maybe_spaces simple_selector {
      $$ = selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_CHILD_SELECTOR);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $4;
    }
  | selector maybe_spaces simple_selector {
      $$ = selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_DESCENDANT_SELECTOR);
      $$->desc.descendant.descendantSelector = $1;
      $$->desc.descendant.simpleSelector = $3;
    }
  ;
simple_selector
  : attribute_conditions maybe_spaces {
      $$ = selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_CONDITIONAL_SELECTOR);
      $$->desc.conditional.simpleSelector = selector_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
      $$->desc.conditional.condition = $1;
    }
  | element_name maybe_attribute_conditions maybe_spaces {
      if ($2 == NULL) {
        $$ = $1;
      } else {
        $$ = selector_alloc(
          YY_SCANNER_MPOOL(scanner), SAC_CONDITIONAL_SELECTOR);
        $$->desc.conditional.simpleSelector = $1;
        $$->desc.conditional.condition = $2;
      }
    }
  ;
attribute_condition
  : HASH {
      $$ = condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_ID_CONDITION);
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
      $$ = condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_CLASS_CONDITION);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = "class";
      $$->desc.attribute.specified = SAC_TRUE;
      $$->desc.attribute.value = $2;
    }
  ;
element_name
  : IDENT {
      $$ = selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_ELEMENT_NODE_SELECTOR);
      $$->desc.element.namespaceURI = NULL;
      $$->desc.element.localName = $1;
    }
  | '*' {
      $$ = selector_alloc(YY_SCANNER_MPOOL(scanner), SAC_ANY_NODE_SELECTOR);
    }
  ;
attrib
  : '[' maybe_spaces IDENT maybe_spaces ']' {
      $$ = condition_alloc(YY_SCANNER_MPOOL(scanner), SAC_ATTRIBUTE_CONDITION);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $3;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
  | '[' maybe_spaces IDENT maybe_spaces attrib_match
    maybe_spaces attrib_value maybe_spaces ']'
    {
      $$ = condition_alloc(YY_SCANNER_MPOOL(scanner), $5);
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
      $$ = condition_alloc(
        YY_SCANNER_MPOOL(scanner), SAC_PSEUDO_CLASS_CONDITION);
      $$->desc.attribute.namespaceURI = NULL;
      $$->desc.attribute.localName = $2;
      $$->desc.attribute.specified = SAC_FALSE;
      $$->desc.attribute.value = NULL;
    }
/*
  | ':' FUNCTION maybe_spaces IDENT maybe_spaces ')'
*/
  ;
declaration
  : property ':' maybe_spaces expr {
      $$ = declaration_alloc(
        YY_SCANNER_MPOOL(scanner), $1,
        lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner)), SAC_FALSE);
    }
  | property ':' maybe_spaces expr prio {
      $$ = declaration_alloc(
        YY_SCANNER_MPOOL(scanner), $1,
        lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner)), SAC_TRUE);
    }
  | /* empty */ {
      $$ = NULL;
    }
  ;
prio
  : IMPORTANT_SYM maybe_spaces
  ;
expr
  : term maybe_spaces {
      $$ = list_open(YY_SCANNER_MPOOL(scanner));
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $1);
    }
  | expr operator term maybe_spaces {
      $$ = $1;
      if ($2 != NULL) list_push_back($$, YY_SCANNER_MPOOL(scanner), $2);
      list_push_back($$, YY_SCANNER_MPOOL(scanner), $3);
    }
  ;
term
  : STRING {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_STRING_VALUE);
      $$->desc.stringValue = $1;
    }
  | IDENT {
      if (strcmp($1, "inherit") != 0) {
        $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_IDENT);
        $$->desc.ident = $1;
      } else {
        $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INHERIT);
      }
    }
  | URI {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_URI);
      $$->desc.uri = $1;
    }
  | UNICODERANGE {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_UNICODERANGE);
      $$->desc.unicodeRange = $1;
    }
  | FREQ_HZ {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_HERTZ);
      $$->desc.dimension.unit = "Hz";
      $$->desc.dimension.value.ureal = $1;
    }
  | FREQ_KHZ {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_KILOHERTZ);
      $$->desc.stringValue = "kHz";
      $$->desc.dimension.value.ureal = $1;
    }
  | unary_operator INT {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_INTEGER);
      $$->desc.integer = $2;
    }
  | unary_operator REAL {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_REAL);
      $$->desc.real = $2;
    }
  | unary_operator PERCENTAGE {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_PERCENTAGE);
      $$->desc.dimension.unit = "%";
      $$->desc.dimension.value.sreal = $2;
    }
  | TIME_MS {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_MILLISECOND);
      $$->desc.dimension.unit = "ms";
      $$->desc.dimension.value.ureal = $1;
    }
  | TIME_S {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_SECOND);
      $$->desc.dimension.unit = "s";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_DEG {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DEGREE);
      $$->desc.dimension.unit = "deg";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_RAD {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RADIAN);
      $$->desc.dimension.unit = "rad";
      $$->desc.dimension.value.ureal = $1;
    }
  | ANGLE_GRAD {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_GRADIAN);
      $$->desc.dimension.unit = "grad";
      $$->desc.dimension.value.ureal = $1;
    }
  | unary_operator LENGTH_EM {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EM);
      $$->desc.dimension.unit = "em";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_EX {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_EX);
      $$->desc.dimension.unit = "ex";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PIXEL {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PIXEL);
      $$->desc.dimension.unit = "px";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_INCH {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_INCH);
      $$->desc.dimension.unit = "in";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_CENTIMETER {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_CENTIMETER);
      $$->desc.dimension.unit = "cm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_MILLIMETER {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_MILLIMETER);
      $$->desc.dimension.unit = "mm";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_POINT {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_POINT);
      $$->desc.dimension.unit = "pt";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator LENGTH_PICA {
      if ($1 == '-') $2 = -$2;
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_LENGTH_PICA);
      $$->desc.dimension.unit = "pc";
      $$->desc.dimension.value.sreal = $2;
    }
  | function { $$ = $1; }
  | hexcolor { $$ = $1; }
  ;
function
  : FUNCTION maybe_spaces expr ')' {
      $$ = lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_FUNCTION);
      $$->desc.function.name = $1;

      $$->desc.function.parameters = lexical_unit_vector_from_list(
        $3, YY_SCANNER_MPOOL(scanner));
    }
  ;
/*
 * There is a constraint on the color that it must
 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
 */
hexcolor
  : HASH {
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
