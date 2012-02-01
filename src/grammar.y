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
%token <str> DIMEN
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
%type <list> some_media_query_list;
%type <media> media_query;
%type <media> media_type;
%type <media> media_type_selector;
%type <media> media_expr;
%type <media> media_exprs;
%type <str> media_feature;

%type <str> property;

%type <sel> simple_selector;
%type <sel> selector;
%type <list> selectors_group;
%type <cond> maybe_attribute_conditions;
%type <cond> attribute_condition;
%type <cond> attribute_conditions;
%type <cond> class;
%type <cond> attrib;
%type <value> hexcolor;
%type <str> attrib_name;
%type <str> attrib_value;
%type <cond> pseudo;
%type <str> pseudo_page;
%type <str> maybe_pseudo_page;

%type <boolean> prio;
%type <boolean> maybe_prio;

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
  | selectors_start selectors_group {
      SAC_parser_end_document(YY_SCANNER_PARSER(scanner));

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | selectors_start bad_selectors_group {
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
closing_bracket
  : ')'
  | EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing bracket not found");
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
  : IMPORT_SYM maybe_spaces string_or_uri bad_media_query ';'
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces import_errors ';' maybe_comments {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces string_or_uri bad_media_query invalid_block
    maybe_comments
    {
      SAC_SYNTAX_ERROR(@5,
        "unexpected 'opening brace' token while parsing 'import' rule");
    }
  | IMPORT_SYM maybe_spaces import_errors invalid_block maybe_comments {
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
  : media_start maybe_rulesets closing_brace {
      SAC_parser_end_media_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  | MEDIA_SYM maybe_spaces media_query media_query_errors invalid_block {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces bad_media_query invalid_block {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces media_query media_query_errors ';' {
      SAC_SYNTAX_ERROR(@5,
        "unexpected 'semicolon' while parsing 'media' rule");
    }
  | MEDIA_SYM maybe_spaces bad_media_query ';' {
      SAC_SYNTAX_ERROR(@4,
        "unexpected 'semicolon' while parsing 'media' rule");
    }
  ;
media_start
  : MEDIA_SYM maybe_spaces maybe_media_query_list '{' maybe_spaces {
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
some_media_query_list
  : media_query {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @1);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | some_media_query_list ',' maybe_spaces media_query {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  | some_media_query_list ',' maybe_spaces media_query media_query_errors {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing media query");

      $$ = $1;
    }
  | some_media_query_list ',' maybe_spaces bad_media_query {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing media query");

      $$ = $1;
    }
  ;
media_query_list
  : some_media_query_list {
      $$ = $1;
    }
  | media_query media_query_errors ',' maybe_spaces some_media_query_list {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing media query");

      $$ = $5;
    }
  | bad_media_query ',' maybe_spaces some_media_query_list {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing media query");

      $$ = $4;
    }
  ;
bad_media_query
  : media_query_errors
  | invalid_bracket_block
  ;
media_query_errors
  : error
  | media_query_errors error
  ;
media_query
  : media_type_selector {
      $$ = $1;
    }
  | ONLY maybe_spaces media_type_selector {
      $$ = SAC_media_query_only(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces media_type_selector {
      $$ = SAC_media_query_not(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ($$, @$);
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
      $$ = SAC_media_query_and(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  ;
media_exprs
  : media_expr {
      $$ = $1;
    }
  | media_exprs AND maybe_spaces media_expr {
      $$ = SAC_media_query_and(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  ;
media_type
  : IDENT maybe_spaces {
      $$ = SAC_media_query_type(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  ;
media_expr
  : '(' maybe_spaces media_feature ')' maybe_spaces {
      $$ = SAC_media_query_feature(YY_SCANNER_MPOOL(scanner), $3, NULL);
      TEST_OBJ($$, @$);
    }
  | '(' maybe_spaces media_feature ':' maybe_spaces expr ')' maybe_spaces {
      SAC_LexicalUnit *value;
      
      value = SAC_lexical_unit_from_list($6, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(value, @6);

      $$ = SAC_media_query_feature(YY_SCANNER_MPOOL(scanner), $3, value);
      TEST_OBJ($$, @$);
    }
  ;
media_feature
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
page
  : page_start maybe_declarations closing_brace {
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
  : PAGE_SYM maybe_spaces maybe_ident maybe_pseudo_page '{' maybe_spaces {
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
  : font_face_start maybe_declarations closing_brace {
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
  : FONT_FACE_SYM maybe_spaces '{' maybe_spaces {
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
      $$ = SAC_lexical_unit_operator_slash(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | ',' maybe_spaces {
      $$ = SAC_lexical_unit_operator_comma(YY_SCANNER_MPOOL(scanner));
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
  : style_start maybe_declarations closing_brace {
      SAC_parser_end_style_handler(YY_SCANNER_PARSER(scanner), $1);
    }
  | bad_selectors_group invalid_block
  ;
style_start
  : selectors_group '{' maybe_spaces {
      SAC_Vector vector;
      
      vector = SAC_vector_from_list($1, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(vector, @1);
      SAC_parser_start_style_handler(YY_SCANNER_PARSER(scanner), vector);
      $$ = vector;
    }
  ;
selectors_group
  : selector {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | selectors_group ',' maybe_spaces selector {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  ;
bad_selectors_group
  : selectors_errors {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing selectors group");
    }
  | selectors_group ',' maybe_spaces selectors_errors {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing selectors group");
    }
  | selectors_group selectors_errors {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing selectors group");
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
      $$ = SAC_selector_descendant(YY_SCANNER_MPOOL(scanner), $1, $3);
      TEST_OBJ($$, @$);
    }
  | selector '+' maybe_spaces simple_selector {
      $$ = SAC_selector_direct_adjacent(YY_SCANNER_MPOOL(scanner),
        SAC_ANY_NODE, $1, $4);
      TEST_OBJ($$, @$);
    }
  | selector '>' maybe_spaces simple_selector {
      $$ = SAC_selector_child(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  ;
simple_selector
  : attribute_conditions {
      SAC_Selector *anyNodeSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @$);

      $$ = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $1);
      TEST_OBJ($$, @$);
    }
  | element_name maybe_attribute_conditions {
      if ($2 == NULL) {
        $$ = $1;
      } else {
        $$ = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner), $1, $2);
        TEST_OBJ($$, @$);
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
      $$ = SAC_condition_and(YY_SCANNER_MPOOL(scanner), $1, $2);
      TEST_OBJ($$, @$);
    }
  ;
attribute_condition
  : HASH {
      $$ = SAC_condition_id(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
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
      $$ = SAC_condition_class(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  ;
element_name
  : IDENT {
      $$ = SAC_selector_element_node(YY_SCANNER_MPOOL(scanner), NULL, $1);
      TEST_OBJ($$, @$);
    }
  | '*' {
      $$ = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  ;
attrib
  : '[' maybe_spaces attrib_name ']' {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner),
        $3, NULL);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name '=' maybe_spaces attrib_value ']' {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner), $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name INCLUDES maybe_spaces attrib_value ']' {
      $$ = SAC_condition_one_of_attribute(YY_SCANNER_MPOOL(scanner), $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name DASHMATCH maybe_spaces attrib_value ']' {
      $$ = SAC_condition_begin_hypen_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $6);
      TEST_OBJ($$, @$);
    }
  ;
attrib_name
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
attrib_value
  : IDENT maybe_spaces {
      $$ = $1;
    }
  | STRING maybe_spaces {
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
      $$ = SAC_condition_pseudo_class(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
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
      $$ = SAC_lexical_unit_int(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator REAL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_real(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator PERCENTAGE maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_percentage(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_PIXEL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_pixel(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_INCH maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_inch(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_CENTIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_centimeter(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_MILLIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_millimeter(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_POINT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_point(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_PICA maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_pica(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_EM maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_em(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator LENGTH_EX maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_ex(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator ANGLE_DEG maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_degree(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator ANGLE_RAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_radian(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator ANGLE_GRAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_gradian(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator DIMEN maybe_spaces {
      double value;
      char *unit;

      value = strtod($2, &unit);
      if ($1 == '-') value = -value;

      $$ = SAC_lexical_unit_dimension(YY_SCANNER_MPOOL(scanner), unit, value);
    }
  | unary_operator TIME_MS maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative time not allowed");
      }

      $$ = SAC_lexical_unit_millisecond(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator TIME_S maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative time not allowed");
      }

      $$ = SAC_lexical_unit_second(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator FREQ_HZ maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative frequency not allowed");
      }

      $$ = SAC_lexical_unit_hertz(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator FREQ_KHZ maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative frequency not allowed");
      }

      $$ = SAC_lexical_unit_kilohertz(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator RESOLUTION_DPI maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative resolution not allowed");
      }

      $$ = SAC_lexical_unit_dots_per_inch(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | unary_operator RESOLUTION_DPCM maybe_spaces {
      if ($1 == '-') {
        SAC_SYNTAX_ERROR(@1,
          "negative resolution not allowed");
      }

      $$ = SAC_lexical_unit_dots_per_centimeter(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | STRING maybe_spaces {
      $$ = SAC_lexical_unit_string(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  | BAD_STRING EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing quote not found");

      $$ = SAC_lexical_unit_string(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  | IDENT maybe_spaces {
      if (strcasecmp($1, "inherit") != 0) {
        $$ = SAC_lexical_unit_ident(YY_SCANNER_MPOOL(scanner), $1);
        TEST_OBJ($$, @$);
      } else {
        $$ = SAC_lexical_unit_inherit(YY_SCANNER_MPOOL(scanner));
        TEST_OBJ($$, @$);
      }
    }
  | URI maybe_spaces {
      $$ = SAC_lexical_unit_uri(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  | BAD_URI EOF_TOKEN {
      SAC_SYNTAX_ERROR(@1,
        "closing bracket not found");

      $$ = SAC_lexical_unit_uri(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  | function {
      $$ = $1;
    }
  | hexcolor {
      $$ = $1;
    }
  | UNICODERANGE maybe_spaces {
      $$ = SAC_lexical_unit_unicode_range(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  ;
function
  : FUNCTION maybe_spaces expr ')' maybe_spaces {
      SAC_LexicalUnit **parameters;

      parameters = SAC_vector_from_list($3, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(parameters, @3);

      if (strcasecmp($1, "attr") == 0) {
        $$ = SAC_lexical_unit_attr(YY_SCANNER_MPOOL(scanner), parameters);
      } else if (strcasecmp($1, "rgb") == 0) {
        $$ = SAC_lexical_unit_rgbcolor(YY_SCANNER_MPOOL(scanner), parameters);
      } else if (strcasecmp($1, "rect") == 0) {
        $$ = SAC_lexical_unit_rect(YY_SCANNER_MPOOL(scanner), parameters);
      } else if (strcasecmp($1, "counter") == 0) {
        $$ = SAC_lexical_unit_counter(YY_SCANNER_MPOOL(scanner), parameters);
      } else if (strcasecmp($1, "counters") == 0) {
        $$ = SAC_lexical_unit_counters(YY_SCANNER_MPOOL(scanner), parameters);
      } else {
        $$ = SAC_lexical_unit_function(YY_SCANNER_MPOOL(scanner),
          $1, parameters);
      }
      TEST_OBJ($$, @$);
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
      SAC_LexicalUnit **parameters;

      len = strlen($1);
      
      if (len == 6) {
        ok = sscanf($1, "%2x%2x%2x", &r, &g, &b) == 3;
      } else if (len == 3) {
        ok = sscanf($1, "%1x%1x%1x", &r, &g, &b) == 3;
      } else {
        ok = 0;
      }

      if (ok) {
        parameters = SAC_vector_open(YY_SCANNER_MPOOL(scanner), 5);
        TEST_OBJ(parameters, @$);

        parameters[0] = SAC_lexical_unit_int(YY_SCANNER_MPOOL(scanner), r);
        TEST_OBJ(parameters[0], @$);

        parameters[1] = SAC_lexical_unit_operator_comma(
          YY_SCANNER_MPOOL(scanner));
        TEST_OBJ(parameters[1], @$);

        parameters[2] = SAC_lexical_unit_int(YY_SCANNER_MPOOL(scanner), g);
        TEST_OBJ(parameters[2], @$);

        parameters[3] = SAC_lexical_unit_operator_comma(
          YY_SCANNER_MPOOL(scanner));
        TEST_OBJ(parameters[3], @$);

        parameters[4] = SAC_lexical_unit_int(YY_SCANNER_MPOOL(scanner), b);
        TEST_OBJ(parameters[4], @$);

      } else {
        parameters = SAC_vector_open(YY_SCANNER_MPOOL(scanner), 0);
        TEST_OBJ(parameters, @$);
      }

      $$ = SAC_lexical_unit_rgbcolor(YY_SCANNER_MPOOL(scanner), parameters);
      TEST_OBJ($$, @$);
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
invalid_bracket_block
  : '(' error invalid_blocks error closing_bracket
  | '(' error closing_bracket
  ;
