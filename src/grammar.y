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
#include "media_query.h"
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

#define SAC_LOG_LOCATION(loc, data) \
  fprintf(stderr, "%d:%d:%d:%d: %s\n", \
    loc.first_line, loc.first_column, \
    loc.last_line, loc.last_column, \
    data);
  
int yydebug = 0;

%}

%union {
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
SAC_MediaQuery *media;
SAC_Boolean boolean;
SAC_Pair pair;
}

%locations
%pure_parser
%debug

%nonassoc LOWEST

%start start

%token START_AS_STYLE_DECLARATIONS
%token START_AS_SELECTORS
%token START_AS_STYLESHEET
%token START_AS_RULE
%token START_AS_PROPERTY_VALUE
%token START_AS_PRIORITY

%token S
%token CDC
%token CDO
%token EOF_TOKEN 0

%token INCLUDES
%token DASHMATCH

%token <str> STRING
%right <str> IDENT

%nonassoc <str> HASH
%nonassoc error
%nonassoc <str> BAD_STRING
%nonassoc <str> BAD_URI

%token IMPORT_SYM
%token PAGE_SYM
%token MEDIA_SYM
%token FONT_FACE_SYM
%token CHARSET_SYM
%token NAMESPACE_SYM

%token IMPORTANT_SYM
%token AND
%token NOT
%token ONLY

%token <real> LENGTH_EM
%token <real> LENGTH_EX
%token <real> LENGTH_PIXEL
%token <real> LENGTH_CENTIMETER
%token <real> LENGTH_MILLIMETER
%token <real> LENGTH_INCH
%token <real> LENGTH_POINT
%token <real> LENGTH_PICA
%token <real> ANGLE_DEG
%token <real> ANGLE_RAD
%token <real> ANGLE_GRAD
%token <real> TIME_MS
%token <real> TIME_S
%token <real> FREQ_HZ
%token <real> FREQ_KHZ
%token <real> RESOLUTION_DPI
%token <real> RESOLUTION_DPCM
%token DIMEN
%token <real> PERCENTAGE
%token <real> REAL
%token <integer> INT

%token <str> URI
%token <str> FUNCTION

%token <str> UNICODERANGE

%type <pair> page_start;
%type <vector> style_start;
%type <vector> media_start;

%type <str> namespace_prefix;
%type <str> maybe_namespace_prefix;

%type <str> string_or_uri;
%type <str> ident;
%type <str> maybe_ident;

%type <list> media_query_list;
%type <list> maybe_media_query_list;
%type <media> media_query;
%type <media> media_type;
%type <media> media_type_selector;
%type <media> media_expr;
%type <media> media_exprs;
%type <str> media_feature;

%type <str> property;

%type <sel> simple_selector;
%type <sel> selector;
%type <list> selectors;
%type <cond> maybe_attribute_conditions;
%type <cond> attribute_condition;
%type <cond> attribute_conditions;
%type <cond> class;
%type <cond> attrib;
%type <value> hexcolor;
%type <str> attrib_value;
%type <cond> pseudo;
%type <str> pseudo_page;
%type <str> maybe_pseudo_page;

%type <boolean> prio;
%type <boolean> maybe_prio;

%type <cond_type> attrib_match;
%type <ch> unary_operator;
%type <value> maybe_operator;

%type <list> expr;
%type <value> term;
%type <value> function;

%type <sel> element_name;

%%

start
  : priority_start maybe_prio {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = (void*)$2;
    }
  | priority_start prio error {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing priority");
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = (void*)SAC_FALSE;
    }
  | priority_start error {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing priority");
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = (void*)SAC_FALSE;
    }
  | property_value_start expr {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_lexical_unit_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | property_value_start bad_expr {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | style_declarations_start maybe_declarations {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | selectors_start selectors {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | selectors_start bad_selectors {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | rule_start ruleset maybe_spaces {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | rule_start ruleset maybe_spaces error {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing ruleset");
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  | stylesheet_start stylesheet {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));
    }
  ;

priority_start
  : START_AS_PRIORITY maybe_spaces {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
property_value_start
  : START_AS_PROPERTY_VALUE maybe_spaces {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
style_declarations_start
  : START_AS_STYLE_DECLARATIONS maybe_spaces {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
selectors_start
  : START_AS_SELECTORS maybe_spaces {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
rule_start
  : START_AS_RULE maybe_spaces {
      SAC_parser_start_document(YY_SCANNER_PARSER(scanner));
    }
  ;
stylesheet_start
  : START_AS_STYLESHEET maybe_spaces {
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
closing_brace
  : '}'
  | EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing brace not found");
    }
  ;
maybe_rulesets
  :
  | maybe_rulesets ruleset maybe_spaces
  ;
stylesheet
  : maybe_charset maybe_imports maybe_namespaces maybe_style_units
  ;
maybe_charset
  :
  | charset
  ;
charset
  : CHARSET_SYM maybe_spaces STRING maybe_spaces ';' maybe_comments
  | bad_charset
  ;
ignore_charset
  : CHARSET_SYM maybe_spaces STRING maybe_spaces ';' maybe_comments {
      SAC_SYNTAX_ERROR(@1,
        "unexpected 'charset' rule");
    }
  | bad_charset
  ;
bad_charset
  : CHARSET_SYM maybe_spaces STRING maybe_spaces charset_errors ';'
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing 'charset' rule");
    }
  | CHARSET_SYM maybe_spaces charset_errors ';' maybe_comments {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'charset' rule");
    }
  | CHARSET_SYM maybe_spaces STRING maybe_spaces charset_errors invalid_block
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@6,
        "unexpected 'opening brace' while parsing 'charset' rule");
    }
  | CHARSET_SYM maybe_spaces charset_errors invalid_block maybe_comments {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'opening brace' while parsing 'charset' rule");
    }
  ;
charset_errors
  : error
  | charset_errors error
  ;
maybe_imports
  : /* empty */
  | imports
  ;
imports
  : import
  | imports import
  | imports ignore_charset
  ;
import
  : IMPORT_SYM maybe_spaces string_or_uri maybe_media_query_list ';'
    maybe_comments
    {
      SAC_Vector mediums;

      mediums = SAC_vector_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(mediums, @4);
      SAC_parser_import_handler(YY_SCANNER_PARSER(scanner), $3, mediums, NULL);
    }
  | bad_import
  ;
ignore_import
  : IMPORT_SYM maybe_spaces string_or_uri maybe_media_query_list ';'
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@1,
        "unexpected 'import' rule");
    }
  | bad_import
  ;
bad_import
  : IMPORT_SYM maybe_spaces string_or_uri media_query_list import_errors
    ';' maybe_comments
    {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces import_errors ';' maybe_comments {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces string_or_uri media_query_list import_errors
    invalid_block maybe_spaces
    {
      SAC_SYNTAX_ERROR(@6,
        "unexpected 'opening brace' token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces import_errors invalid_block maybe_spaces {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'opening brace' token while parsing 'import' rule");
    }
  ;
import_errors
  : error
  | import_errors error
maybe_namespaces
  : /* empty */
  | namespaces
  ;
namespaces
  : namespace
  | namespaces namespace
  | namespaces ignore_charset
  | namespaces ignore_import
  ;
namespace
  : NAMESPACE_SYM maybe_spaces maybe_namespace_prefix string_or_uri
    ';' maybe_comments
    {
      SAC_parser_namespace_declaration_handler(YY_SCANNER_PARSER(scanner),
        $3, $4);
    }
  | bad_namespace
  ;
ignore_namespace
  : NAMESPACE_SYM maybe_spaces maybe_namespace_prefix string_or_uri
    ';' maybe_comments
    {
      SAC_SYNTAX_ERROR(@1,
        "unexpected 'namespace' rule");
    }
  | bad_namespace
  ;
bad_namespace
  : NAMESPACE_SYM maybe_spaces maybe_namespace_prefix string_or_uri
    namespace_errors ';' maybe_comments
    {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing 'namespace' rule");
    }
  | NAMESPACE_SYM maybe_spaces namespace_prefix namespace_errors ';'
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing 'namespace' rule");
    }
  | NAMESPACE_SYM maybe_spaces namespace_errors ';' maybe_comments {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'namespace' rule");
    }
  | NAMESPACE_SYM maybe_spaces namespace_errors invalid_block maybe_comments
    {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'opening brace' token while parsing 'namespace' rule");
    }
  ;
namespace_errors
  : error
  | namespace_errors error
  ;
maybe_style_units
  : /* empty */
  | style_units
  ;
style_units
  : style_unit
  | style_units style_unit
  | style_units ignore_charset
  | style_units ignore_import
  | style_units ignore_namespace
  ;
style_unit
  : ruleset maybe_comments
  | media maybe_comments
  | page maybe_comments
  | font_face maybe_comments
  | ignorable_at_rule maybe_comments
  ;
string_or_uri
  : STRING maybe_spaces {
      $$ = $1;
    }
  | BAD_STRING EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing quote not found");

      $$ = $1;
    }
  | URI maybe_spaces {
      $$ = $1;
    }
  | BAD_URI EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing bracket not found");

      $$ = $1;
    }
  ;
maybe_namespace_prefix
  : /* empty */ {
      $$ = NULL;
    }
  | namespace_prefix {
      $$ = $1;
    }
  ;
namespace_prefix
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
media
  : media_start '{' maybe_spaces maybe_rulesets closing_brace {
      SAC_parser_end_media_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  | MEDIA_SYM maybe_spaces media_query_list media_errors ';' {
      SAC_SYNTAX_ERROR(@5,
        "unexpected 'semicolon' while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces media_errors ';' {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'semicolon' while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces media_query_list media_errors invalid_block {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces media_errors invalid_block {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'media' rule");
    }
  ;
media_errors
  : error
  | media_errors error
  ;
media_start
  : MEDIA_SYM maybe_spaces maybe_media_query_list {
      SAC_Vector mediums;

      mediums = SAC_vector_from_list($3, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(mediums, @3);
      SAC_parser_start_media_handler(YY_SCANNER_PARSER(scanner), mediums);
      $$ = mediums;
    }
  ;
maybe_media_query_list
  : /* empty */ {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | media_query_list {
      $$ = $1;
    }
  ;
media_query_list
  : media_query {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @1);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | media_query_list ',' maybe_spaces media_query {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  ;
media_query
  : media_type_selector {
      $$ = $1;
    }
  | ONLY maybe_spaces media_type_selector {
      $$ = NULL;
    }
  | NOT maybe_spaces media_type_selector {
      $$ = NULL;
    }
  | media_exprs {
      $$ = $1;
    }
  ;
media_type_selector
  : media_type {
      $$ = $1;
    }
  | media_type AND maybe_spaces media_exprs {
      $$ = SAC_media_query_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_AND_MEDIA_QUERY);
      TEST_OBJ($$, @$);
      $$->desc.combinator.firstMediaQuery = $1;
      $$->desc.combinator.secondMediaQuery = $4;
    }
  ;
media_exprs
  : media_expr {
      $$ = $1;
    }
  | media_exprs AND maybe_spaces media_expr {
      $$ = SAC_media_query_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_AND_MEDIA_QUERY);
      TEST_OBJ($$, @$);
      $$->desc.combinator.firstMediaQuery = $1;
      $$->desc.combinator.secondMediaQuery = $4;
    }
  ;
media_type
  : IDENT maybe_spaces {
      $$ = SAC_media_query_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_TYPE_MEDIA_QUERY);
      TEST_OBJ($$, @$);
      $$->desc.type = $1;
    }
  ;
media_expr
  : '(' maybe_spaces media_feature ')' maybe_spaces {
      $$ = SAC_media_query_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_FEATURE_MEDIA_QUERY);
      TEST_OBJ($$, @$);
      $$->desc.feature.name = $3;
      $$->desc.feature.value = NULL;
    }
  | '(' maybe_spaces media_feature ':' maybe_spaces expr ')' maybe_spaces {
      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($6, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @6);

      $$ = SAC_media_query_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_FEATURE_MEDIA_QUERY);
      TEST_OBJ($$, @$);
      $$->desc.feature.name = $3;
      $$->desc.feature.value = expr;
    }
  ;
media_feature
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
page
  : page_start '{' maybe_spaces maybe_declarations closing_brace {
      SAC_parser_end_page_handler(YY_SCANNER_PARSER(scanner),
        $1.first, $1.second);
    }
  | PAGE_SYM maybe_spaces maybe_ident pseudo_page page_errors invalid_block {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing 'page' rule");
    }
  | PAGE_SYM maybe_spaces ident page_errors invalid_block {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing 'page' rule");
    }
  | PAGE_SYM maybe_spaces page_errors invalid_block {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'page' rule");
    }
  | PAGE_SYM maybe_spaces maybe_ident pseudo_page page_errors ';' {
      SAC_SYNTAX_ERROR(@6,
        "unexpected 'semicolon' while parsing 'page' rule");
    }
  | PAGE_SYM maybe_spaces ident page_errors ';' {
      SAC_SYNTAX_ERROR(@5,
        "unexpected 'semicolon' while parsing 'page' rule");
    }
  | PAGE_SYM maybe_spaces page_errors ';' {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'semicolon' while parsing 'page' rule");
    }
  ;
page_errors
  : error
  | page_errors error
  ;
page_start
  : PAGE_SYM maybe_spaces maybe_ident maybe_pseudo_page {
      SAC_parser_start_page_handler(YY_SCANNER_PARSER(scanner), $3, $4);
      $$.first = $3;
      $$.second = $4;
    }
  ;
maybe_ident
  : /* empty */ {
      $$ = NULL;
    }
  | ident {
      $$ = $1;
    }
  ;
ident
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
maybe_pseudo_page
  : /* empty */ {
      $$ = NULL;
    }
  | pseudo_page {
      $$ = $1;
    }
  ;
pseudo_page
  : ':' IDENT maybe_spaces {
      $$ = $2;
    }
  ;
font_face
  : font_face_start '{' maybe_spaces maybe_declarations closing_brace {
      SAC_parser_end_font_face_handler(YY_SCANNER_PARSER(scanner));
    }
  | FONT_FACE_SYM maybe_spaces font_face_errors invalid_block {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'font-face' rule");
    }
  | FONT_FACE_SYM maybe_spaces font_face_errors ';' {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'semicolon' while parsing 'font-face' rule");
    }
  ;
font_face_errors
  : error
  | font_face_errors error
  ;
font_face_start
  : FONT_FACE_SYM maybe_spaces {
      SAC_parser_start_font_face_handler(YY_SCANNER_PARSER(scanner));
    }
  ;
ignorable_at_rule
  : '@' IDENT maybe_spaces error invalid_block {
      SAC_parser_ignorable_at_rule_handler(YY_SCANNER_PARSER(scanner), $2);
    }
  | '@' IDENT maybe_spaces error ';' {
      SAC_parser_ignorable_at_rule_handler(YY_SCANNER_PARSER(scanner), $2);
    }
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
ruleset
  : style_start '{' maybe_spaces maybe_declarations closing_brace {
      SAC_parser_end_style_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  | bad_selectors invalid_block
  ;
style_start
  : selectors {
      SAC_Vector vector;
      
      vector = SAC_vector_from_list($1, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(vector, @1);
      SAC_parser_start_style_handler(YY_SCANNER_PARSER(scanner), vector);
      $$ = vector;
    }
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
bad_selectors
  : selectors_errors {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing selectors");
    }
  | selectors ',' maybe_spaces selectors_errors {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing selectors");
    }
  | selectors selectors_errors {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing selectors");
    }
selectors_errors
  : error
  | selectors_errors error
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
  | BAD_STRING EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing quote not found");

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
/*
  | ':' FUNCTION maybe_spaces maybe_ident ')'
*/
  ;
maybe_declarations
  : /* empty */
  | declaration
  | strict_declarations
  | strict_declarations declaration
  ;
strict_declarations
  : declaration ';' maybe_spaces
  | strict_declarations declaration ';' maybe_spaces
  ;
declaration
  : property ':' maybe_spaces expr maybe_prio {
      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5);
    }
  | property ':' maybe_spaces expr prio declaration_errors {
      /* p {color: red !important fail;} */

      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5);

      SAC_SYNTAX_ERROR(@6,
        "unexpected token after property expression");
    }
  | property ':' maybe_spaces expr declaration_errors {
      /* p {color: red !important fail;} */

      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, SAC_FALSE);

      SAC_SYNTAX_ERROR(@5,
        "unexpected token after property expression");
    }
  | property ':' maybe_spaces declaration_errors {
      /* p { weight: *; } */

      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing property expression");
    }
  | property declaration_errors {
      /* p { weight: *; } */

      SAC_SYNTAX_ERROR(@2,
        "colon expected while parsing style declaration");
    }
  | declaration_errors {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing style declaration");
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
  ;
declaration_errors
  : error
  | declaration_errors error
  | invalid_block
  | declaration_errors invalid_block
  ;
prio
  : IMPORTANT_SYM maybe_spaces {
      $$ = SAC_TRUE;
    }
  ;
maybe_prio
  : prio {
      $$ = $1;
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
bad_expr
  : expr_errors {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing property expression");
    }
  | expr expr_errors {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing property expression");
    }
  ;
expr_errors
  : error
  | expr_errors error
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
  | unary_operator ANGLE_DEG maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DEGREE);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "deg";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator ANGLE_RAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_RADIAN);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "rad";
      $$->desc.dimension.value.sreal = $2;
    }
  | unary_operator ANGLE_GRAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_GRADIAN);
      TEST_OBJ($$, @$);
      $$->desc.dimension.unit = "grad";
      $$->desc.dimension.value.sreal = $2;
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
  | unary_operator RESOLUTION_DPI maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative resolution not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_DOTS_PER_INCH);
      TEST_OBJ($$, @$);
      $$->desc.stringValue = "dpi";
      $$->desc.dimension.value.ureal = $2;
    }
  | unary_operator RESOLUTION_DPCM maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative resolution not allowed");
      }

      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner),
        SAC_DOTS_PER_CENTIMETER);
      TEST_OBJ($$, @$);
      $$->desc.stringValue = "dpcm";
      $$->desc.dimension.value.ureal = $2;
    }
  | STRING maybe_spaces {
      $$ = SAC_lexical_unit_alloc(YY_SCANNER_MPOOL(scanner), SAC_STRING_VALUE);
      TEST_OBJ($$, @$);
      $$->desc.stringValue = $1;
    }
  | BAD_STRING EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing quote not found");

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
  | BAD_URI EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing bracket not found");

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
  : '{' error invalid_blocks error closing_brace
  | '{' error closing_brace
  ;
invalid_blocks
  : invalid_block
  | invalid_blocks error invalid_block
  ;
