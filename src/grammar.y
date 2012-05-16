/*
 * References:
 *  - http://www.w3.org/TR/CSS21/grammar.html#grammar
 *  - http://www.w3.org/TR/css3-namespace/#syntax
 *
 *  - http://www.w3.org/TR/css3-fonts/
 */
%code requires {
#include <strings.h>
#include <string.h>

#include "list.h"
#include "vector.h"
#include "vector_extra.h"
#include "lexical_unit.h"
#include "condition.h"
#include "selector.h"
#include "media_query.h"
#include "page_margin.h"
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

int yyerror(const char *data) { return 0; }
extern int yylex();

#define PARSER_ASSERT(expr, loc, errno) \
  do { \
    if (!(expr)) { \
      SAC_parser_fatal_error_handler(YY_SCANNER_PARSER(scanner), \
        (loc).first_line, (loc).first_column, (errno)); \
      YYABORT; \
    } \
  } while (0)

#define TEST_OBJ(obj, loc) \
  PARSER_ASSERT((obj) != NULL, loc, SAC_FATAL_ERROR_NO_MEMORY)

#define TEST_RVAL(rval, loc) \
  PARSER_ASSERT(rval == 0, loc, rval)

#define SAC_ERROR(loc, type, data) \
  TEST_RVAL(SAC_parser_error_handler(YY_SCANNER_PARSER(scanner), \
      loc.first_line, loc.first_column, type, data), (loc))

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
SAC_MediaQuery *media;
SAC_PageMargin *margin;
SAC_PageMarginType margin_type;
SAC_Boolean boolean;
}

%locations
%pure_parser
%debug

%nonassoc LOWEST

%start start

%token START_AS_STYLE_DECLARATIONS
%token START_AS_SELECTORS
%token START_AS_PAGESELECTORS
%token START_AS_MEDIAQUERY
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
%token PREFIXMATCH
%token SUFFIXMATCH
%token SUBSTRINGMATCH

%token PLUS
%token GREATER
%token COMMA
%token TILDE
%token NOT

%token <str> STRING
%right <str> IDENT
%token <str> NTH

%nonassoc <str> HASH
%nonassoc error
%nonassoc <str> BAD_STRING
%nonassoc <str> BAD_URI

%token IMPORT_SYM
%token PAGE_SYM
%token TOP_LEFT_CORNER_SYM
%token TOP_LEFT_SYM
%token TOP_CENTER_SYM
%token TOP_RIGHT_SYM
%token TOP_RIGHT_CORNER_SYM
%token BOTTOM_LEFT_CORNER_SYM
%token BOTTOM_LEFT_SYM
%token BOTTOM_CENTER_SYM
%token BOTTOM_RIGHT_SYM
%token BOTTOM_RIGHT_CORNER_SYM
%token LEFT_TOP_SYM
%token LEFT_MIDDLE_SYM
%token LEFT_BOTTOM_SYM
%token RIGHT_TOP_SYM
%token RIGHT_MIDDLE_SYM
%token RIGHT_BOTTOM_SYM
%token MEDIA_SYM
%token FONT_FACE_SYM
%token CHARSET_SYM
%token NAMESPACE_SYM
%token <str> ATKEYWORD_SYM

%token IMPORTANT_SYM
%token MEDIA_AND
%token MEDIA_NOT
%token MEDIA_ONLY

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

%type <vector> page_start;
%type <vector> style_start;
%type <vector> media_start;

%type <margin> margin_start;
%type <margin_type> margin_sym;

%type <str> namespace_prefix;
%type <str> maybe_namespace_prefix;

%type <str> string_or_uri;
%type <str> ident;
%type <sel> maybe_page_ident;

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

%type <sel> simple_selector_sequence;
%type <sel> selector;
%type <list> selectors_group;
%type <vector> page_selectors_group;
%type <cond> maybe_attribute_conditions;
%type <cond> attribute_condition;
%type <cond> attribute_conditions;
%type <cond> class;
%type <cond> hash;
%type <cond> attrib;
%type <cond> negation;
%type <value> hexcolor;
%type <str> attrib_name;
%type <str> attrib_value;
%type <cond> pseudo;
%type <cond> pseudo_page;
%type <cond> maybe_pseudo_page;

%type <boolean> prio;
%type <boolean> maybe_prio;

%type <ch> unary_operator;
%type <ch> maybe_unary_operator;
%type <value> maybe_operator;

%type <list> expr;
%type <value> term;
%type <value> function;
%type <vector> functional_pseudo_expr;
%type <value> functional_pseudo;

%type <sel> type_selector;
%type <sel> universal;
%type <str> selector_namespace_prefix;

%%

start
  : priority_start maybe_prio {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = (void*)$2;
    }
  | priority_start prio error {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing priority");
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = (void*)SAC_FALSE;
    }
  | priority_start error {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing priority");
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = (void*)SAC_FALSE;
    }
  | property_value_start expr {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = SAC_lexical_unit_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | property_value_start bad_expr {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | style_declarations_start maybe_declarations {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  | selectors_start selectors_group {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | selectors_start bad_selectors_group {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | pageselectors_start page_selectors_group {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = $2;
    }
  | pageselectors_start page_selectors_group error {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing page selectors");
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | mediaquery_start maybe_media_query_list {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = SAC_vector_from_list(
        $2, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(YY_SCANNER_OUTPUT(scanner), @2);
    }
  | mediaquery_start bad_media_query {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);

      YY_SCANNER_OUTPUT(scanner) = NULL;
    }
  | rule_start rule_unit {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  | rule_start rule_unit error {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing ruleset");
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  | stylesheet_start stylesheet {
      TEST_RVAL(SAC_parser_end_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  ;

priority_start
  : START_AS_PRIORITY maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  ;
property_value_start
  : START_AS_PROPERTY_VALUE maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  ;
style_declarations_start
  : START_AS_STYLE_DECLARATIONS maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  ;
selectors_start
  : START_AS_SELECTORS maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
pageselectors_start
  : START_AS_PAGESELECTORS maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
mediaquery_start
  : START_AS_MEDIAQUERY maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
rule_start
  : START_AS_RULE maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
    }
  ;
stylesheet_start
  : START_AS_STYLESHEET maybe_spaces {
      TEST_RVAL(SAC_parser_start_document(YY_SCANNER_PARSER(scanner)), @$);
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
      TEST_RVAL(SAC_parser_import_handler(YY_SCANNER_PARSER(scanner),
        $3, mediums, NULL), @$);
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
      TEST_RVAL(SAC_parser_namespace_handler(
        YY_SCANNER_PARSER(scanner), $3, $4), @$);
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
rule_unit
  : style_unit
  | namespace
  | import
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
      TEST_RVAL(SAC_parser_end_media_handler(YY_SCANNER_PARSER(scanner),
        $1), @$);
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
      TEST_RVAL(SAC_parser_start_media_handler(YY_SCANNER_PARSER(scanner),
        mediums), @$);
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
  | some_media_query_list COMMA maybe_spaces media_query {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  | some_media_query_list COMMA maybe_spaces media_query media_query_errors {
      SAC_SYNTAX_ERROR(@5,
        "unexpected token while parsing media query");

      $$ = $1;
    }
  | some_media_query_list COMMA maybe_spaces bad_media_query {
      SAC_SYNTAX_ERROR(@4,
        "unexpected token while parsing media query");

      $$ = $1;
    }
  ;
media_query_list
  : some_media_query_list {
      $$ = $1;
    }
  | media_query media_query_errors COMMA maybe_spaces some_media_query_list {
      SAC_SYNTAX_ERROR(@2,
        "unexpected token while parsing media query");

      $$ = $5;
    }
  | bad_media_query COMMA maybe_spaces some_media_query_list {
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
  | MEDIA_ONLY maybe_spaces media_type_selector {
      $$ = SAC_media_query_only(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ($$, @$);
    }
  | MEDIA_NOT maybe_spaces media_type_selector {
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
  | media_type MEDIA_AND maybe_spaces media_exprs {
      $$ = SAC_media_query_and(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  ;
media_exprs
  : media_expr {
      $$ = $1;
    }
  | media_exprs MEDIA_AND maybe_spaces media_expr {
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
  : page_start maybe_declarations_and_margins closing_brace {
      TEST_RVAL(SAC_parser_end_page_handler(YY_SCANNER_PARSER(scanner),
        $1), @$);
    }
  | PAGE_SYM maybe_spaces maybe_page_ident pseudo_page page_errors
    invalid_block {
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
  | PAGE_SYM maybe_spaces maybe_page_ident pseudo_page page_errors ';' {
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
maybe_declarations_and_margins
  : maybe_declarations
  | maybe_declarations_and_margins margin maybe_declarations
  ;
margin
  : margin_start maybe_declarations closing_brace maybe_spaces
    {
      TEST_RVAL(SAC_parser_end_page_margin_handler(
        YY_SCANNER_PARSER(scanner), $1), @$);
    }
  | margin_sym maybe_spaces margin_errors invalid_block maybe_spaces {
      SAC_SYNTAX_ERROR(@3,
        "unexpected token while parsing 'page margin' rule");
    }
  ;
margin_errors
  : error
  | margin_errors error
  ;
margin_start
  : margin_sym maybe_spaces '{' maybe_spaces {
      $$ = SAC_page_margin(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);

      TEST_RVAL(SAC_parser_start_page_margin_handler(
        YY_SCANNER_PARSER(scanner), $$), @$);
    }
  ;
margin_sym
  : TOP_LEFT_CORNER_SYM {
      $$ = SAC_TOP_LEFT_CORNER_PAGE_MARGIN;
    }
  | TOP_LEFT_SYM {
      $$ = SAC_TOP_LEFT_PAGE_MARGIN;
    }
  | TOP_CENTER_SYM {
      $$ = SAC_TOP_CENTER_PAGE_MARGIN;
    }
  | TOP_RIGHT_SYM {
      $$ = SAC_TOP_RIGHT_PAGE_MARGIN;
    }
  | TOP_RIGHT_CORNER_SYM {
      $$ = SAC_TOP_RIGHT_CORNER_PAGE_MARGIN;
    }
  | BOTTOM_LEFT_CORNER_SYM {
      $$ = SAC_BOTTOM_LEFT_CORNER_PAGE_MARGIN;
    }
  | BOTTOM_LEFT_SYM {
      $$ = SAC_BOTTOM_LEFT_PAGE_MARGIN;
    }
  | BOTTOM_CENTER_SYM {
      $$ = SAC_BOTTOM_CENTER_PAGE_MARGIN;
    }
  | BOTTOM_RIGHT_SYM {
      $$ = SAC_BOTTOM_RIGHT_PAGE_MARGIN;
    }
  | BOTTOM_RIGHT_CORNER_SYM {
      $$ = SAC_BOTTOM_RIGHT_CORNER_PAGE_MARGIN;
    }
  | LEFT_TOP_SYM {
      $$ = SAC_LEFT_TOP_PAGE_MARGIN;
    }
  | LEFT_MIDDLE_SYM {
      $$ = SAC_LEFT_MIDDLE_PAGE_MARGIN;
    }
  | LEFT_BOTTOM_SYM {
      $$ = SAC_LEFT_BOTTOM_PAGE_MARGIN;
    }
  | RIGHT_TOP_SYM {
      $$ = SAC_RIGHT_TOP_PAGE_MARGIN;
    }
  | RIGHT_MIDDLE_SYM {
      $$ = SAC_RIGHT_MIDDLE_PAGE_MARGIN;
    }
  | RIGHT_BOTTOM_SYM {
      $$ = SAC_RIGHT_BOTTOM_PAGE_MARGIN;
    }
  ;
page_errors
  : error
  | page_errors error
  ;
page_selectors_group
  : maybe_page_ident maybe_pseudo_page {
      SAC_Vector vector;
      SAC_Selector **selectors;

      vector = SAC_vector_open(YY_SCANNER_MPOOL(scanner), 1);
      TEST_OBJ(vector, @$);
      selectors = vector;
      
      if ($2 != NULL) {
        selectors[0] = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
          $1, $2);
        TEST_OBJ(selectors[0], @1);
      } else {
        selectors[0] = $1;
      }

      $$ = selectors;
    }
  ;
page_start
  : PAGE_SYM maybe_spaces page_selectors_group '{' maybe_spaces {
      $$ = $3;
      TEST_RVAL(SAC_parser_start_page_handler(YY_SCANNER_PARSER(scanner),
        $$), @$);
    }
  ;
maybe_page_ident
  : /* empty */ {
      $$ = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | ident {
      $$ = SAC_selector_element_node(YY_SCANNER_MPOOL(scanner), NULL, $1);
      TEST_OBJ($$, @$);
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
      SAC_LexicalUnit *pseudo;

      pseudo = SAC_lexical_unit_ident(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ(pseudo, @2);

      $$ = SAC_condition_pseudo_class(YY_SCANNER_MPOOL(scanner), pseudo);
      TEST_OBJ($$, @$);
    }
  ;
font_face
  : font_face_start maybe_declarations closing_brace {
      TEST_RVAL(SAC_parser_end_font_face_handler(YY_SCANNER_PARSER(scanner)),
        @$);
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
      TEST_RVAL(SAC_parser_start_font_face_handler(YY_SCANNER_PARSER(scanner)),
        @$);
    }
  ;
ignorable_at_rule
  : ATKEYWORD_SYM maybe_spaces error invalid_block {
      TEST_RVAL(SAC_parser_ignorable_at_rule_handler(YY_SCANNER_PARSER(scanner),
        $1), @$);
    }
  | ATKEYWORD_SYM maybe_spaces error ';' {
      TEST_RVAL(SAC_parser_ignorable_at_rule_handler(YY_SCANNER_PARSER(scanner),
        $1), @$);
    }
maybe_operator
  : '/' maybe_spaces {
      $$ = SAC_lexical_unit_operator_slash(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | COMMA maybe_spaces {
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
  | PLUS {
      $$ = '+';
    }
  ;
maybe_unary_operator
  : /* empty */ {
      $$ = '\0';
    }
  | unary_operator {
      $$ = $1;
    }
  ;
property
  : IDENT maybe_spaces {
      $$ = $1;
    }
  ;
ruleset
  : style_start maybe_declarations closing_brace {
      TEST_RVAL(SAC_parser_end_style_handler(YY_SCANNER_PARSER(scanner),
        $1), @$);
    }
  | bad_selectors_group invalid_block
  ;
style_start
  : selectors_group '{' maybe_spaces {
      SAC_Vector vector;
      
      vector = SAC_vector_from_list($1, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(vector, @1);
      TEST_RVAL(SAC_parser_start_style_handler(YY_SCANNER_PARSER(scanner),
        vector), @$);
      $$ = vector;
    }
  ;
selectors_group
  : selector {
      $$ = SAC_list_open(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $1), @1);
    }
  | selectors_group COMMA maybe_spaces selector {
      $$ = $1;
      TEST_OBJ(SAC_list_push_back($$, YY_SCANNER_MPOOL(scanner), $4), @4);
    }
  ;
bad_selectors_group
  : selectors_errors {
      SAC_SYNTAX_ERROR(@1,
        "unexpected token while parsing selectors group");
    }
  | selectors_group COMMA maybe_spaces selectors_errors {
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
  : simple_selector_sequence {
      $$ = $1;
    }
  | selector S {
      $$ = $1;
    }
  | selector S simple_selector_sequence {
      $$ = SAC_selector_descendant(YY_SCANNER_MPOOL(scanner), $1, $3);
      TEST_OBJ($$, @$);
    }
  | selector PLUS maybe_spaces simple_selector_sequence {
      $$ = SAC_selector_direct_adjacent(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  | selector GREATER maybe_spaces simple_selector_sequence {
      $$ = SAC_selector_child(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  | selector TILDE maybe_spaces simple_selector_sequence {
      $$ = SAC_selector_general_adjacent(YY_SCANNER_MPOOL(scanner), $1, $4);
      TEST_OBJ($$, @$);
    }
  ;
simple_selector_sequence
  : attribute_conditions {
      SAC_Selector *anyNodeSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @$);

      $$ = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $1);
      TEST_OBJ($$, @$);
    }
  | type_selector maybe_attribute_conditions {
      if ($2 == NULL) {
        $$ = $1;
      } else {
        $$ = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner), $1, $2);
        TEST_OBJ($$, @$);
      }
    }
  | universal maybe_attribute_conditions {
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
type_selector
  : IDENT {
      $$ = SAC_selector_element_node(YY_SCANNER_MPOOL(scanner), NULL, $1);
      TEST_OBJ($$, @$);
    }
  | selector_namespace_prefix IDENT {
      $$ = SAC_selector_element_node(YY_SCANNER_MPOOL(scanner), $1, $2);
      TEST_OBJ($$, @$);
    }
  ;
selector_namespace_prefix
  : IDENT '|' {
      $$ = $1;
    }
  | '*' '|' {
      $$ = NULL;
    }
  ;
universal
  : '*' {
      $$ = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ($$, @$);
    }
  | selector_namespace_prefix '*' {
      if ($1 != NULL) {
        $$ = SAC_selector_element_node(YY_SCANNER_MPOOL(scanner), $1, NULL);
        TEST_OBJ($$, @$);
      } else {
        $$ = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
        TEST_OBJ($$, @$);
      }
    }
  ;
attribute_condition
  : hash {
      $$ = $1;
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
  | negation {
      $$ = $1;
    }
  ;
hash
  : HASH {
      $$ = SAC_condition_id(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  ;
class
  : '.' IDENT {
      $$ = SAC_condition_class(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  ;
attrib
  : '[' maybe_spaces attrib_name ']' {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, NULL);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name ']' {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, NULL);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name PREFIXMATCH maybe_spaces attrib_value ']' {
      $$ = SAC_condition_prefix_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name PREFIXMATCH
    maybe_spaces attrib_value ']' {
      $$ = SAC_condition_prefix_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, $7);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name SUFFIXMATCH maybe_spaces attrib_value ']' {
      $$ = SAC_condition_suffix_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name SUFFIXMATCH
    maybe_spaces attrib_value ']' {
      $$ = SAC_condition_suffix_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, $7);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name SUBSTRINGMATCH maybe_spaces attrib_value ']' {
      $$ = SAC_condition_substring_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name SUBSTRINGMATCH
    maybe_spaces attrib_value ']' {
      $$ = SAC_condition_substring_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, $7);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name '=' maybe_spaces attrib_value ']' {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner), NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name '=' maybe_spaces
    attrib_value ']'
    {
      $$ = SAC_condition_attribute(YY_SCANNER_MPOOL(scanner), $3, $4, $7);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name INCLUDES maybe_spaces attrib_value ']' {
      $$ = SAC_condition_one_of_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name INCLUDES
    maybe_spaces attrib_value ']'
    {
      $$ = SAC_condition_one_of_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, $7);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces attrib_name DASHMATCH maybe_spaces attrib_value ']'
    {
      $$ = SAC_condition_begin_hyphen_attribute(YY_SCANNER_MPOOL(scanner),
        NULL, $3, $6);
      TEST_OBJ($$, @$);
    }
  | '[' maybe_spaces selector_namespace_prefix attrib_name DASHMATCH
    maybe_spaces attrib_value ']'
    {
      $$ = SAC_condition_begin_hyphen_attribute(YY_SCANNER_MPOOL(scanner),
        $3, $4, $7);
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
      SAC_LexicalUnit *pseudo;

      pseudo = SAC_lexical_unit_ident(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ(pseudo, @2);

      if (
        strcasecmp($2, "first-line") == 0 ||
        strcasecmp($2, "first-letter") == 0 ||
        strcasecmp($2, "before") == 0 ||
        strcasecmp($2, "after") == 0)
      {
        $$ = SAC_condition_pseudo_element(YY_SCANNER_MPOOL(scanner), pseudo);
      } else {
        $$ = SAC_condition_pseudo_class(YY_SCANNER_MPOOL(scanner), pseudo);
      }
      TEST_OBJ($$, @$);
    }
  | ':' ':' IDENT {
      SAC_LexicalUnit *pseudo;

      pseudo = SAC_lexical_unit_ident(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ(pseudo, @3);

      $$ = SAC_condition_pseudo_element(YY_SCANNER_MPOOL(scanner), pseudo);
      TEST_OBJ($$, @$);
    }
  | ':' functional_pseudo {
      $$ = SAC_condition_pseudo_element(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  ;
functional_pseudo
  : FUNCTION maybe_spaces functional_pseudo_expr ')' {
      if (strcasecmp($1, "nth-child") == 0) {
        $$ = SAC_lexical_unit_nth_child_function(
          YY_SCANNER_MPOOL(scanner), $3);
      } else if (strcasecmp($1, "nth-last-child") == 0) {
        $$ = SAC_lexical_unit_nth_last_child_function(
          YY_SCANNER_MPOOL(scanner), $3);
      } else if (strcasecmp($1, "nth-of-type") == 0) {
        $$ = SAC_lexical_unit_nth_of_type_function(
          YY_SCANNER_MPOOL(scanner), $3);
      } else if (strcasecmp($1, "nth-last-of-type") == 0) {
        $$ = SAC_lexical_unit_nth_last_of_type_function(
          YY_SCANNER_MPOOL(scanner), $3);
      } else {
        $$ = SAC_lexical_unit_function(YY_SCANNER_MPOOL(scanner), $1, $3);
      }
      TEST_OBJ($$, @$);
    }
  ;
functional_pseudo_expr
  : maybe_unary_operator INT maybe_spaces {
      if ($1 == '-') $2 = -$2;

      $$ = SAC_lexical_unit_nth_expr(YY_SCANNER_MPOOL(scanner), "n", 0, $2);
      TEST_OBJ($$, @$);
    }
  | NTH maybe_spaces {
      $$ = SAC_lexical_unit_parse_nth_expr(YY_SCANNER_MPOOL(scanner), $1);
      TEST_OBJ($$, @$);
    }
  | IDENT maybe_spaces {
      if (strcasecmp($1, "odd") == 0) {
        $$ = SAC_lexical_unit_nth_expr(YY_SCANNER_MPOOL(scanner), "n", 2, 1);
      } else if (strcasecmp($1, "even") == 0) {
        $$ = SAC_lexical_unit_nth_expr(YY_SCANNER_MPOOL(scanner), "n", 2, 0);
      } else {
        $$ = SAC_lexical_unit_nth_ident_expr(YY_SCANNER_MPOOL(scanner), $1);
      }
      TEST_OBJ($$, @$);
    }
  ;
negation
  : NOT maybe_spaces type_selector maybe_spaces ')' {
      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces universal maybe_spaces ')' {
      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner), $3);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces hash maybe_spaces ')' {
      SAC_Selector *anyNodeSelector;
      SAC_Selector *conditionalSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @3);

      conditionalSelector = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $3);
      TEST_OBJ(conditionalSelector, @3);

      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner),
        conditionalSelector);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces class maybe_spaces ')' {
      SAC_Selector *anyNodeSelector;
      SAC_Selector *conditionalSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @3);

      conditionalSelector = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $3);
      TEST_OBJ(conditionalSelector, @3);

      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner),
        conditionalSelector);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces attrib maybe_spaces ')' {
      SAC_Selector *anyNodeSelector;
      SAC_Selector *conditionalSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @3);

      conditionalSelector = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $3);
      TEST_OBJ(conditionalSelector, @3);

      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner),
        conditionalSelector);
      TEST_OBJ($$, @$);
    }
  | NOT maybe_spaces pseudo maybe_spaces ')' {
      SAC_Selector *anyNodeSelector;
      SAC_Selector *conditionalSelector;

      anyNodeSelector = SAC_selector_any_node(YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(anyNodeSelector, @3);

      conditionalSelector = SAC_selector_conditional(YY_SCANNER_MPOOL(scanner),
        anyNodeSelector, $3);
      TEST_OBJ(conditionalSelector, @3);

      $$ = SAC_condition_negation(YY_SCANNER_MPOOL(scanner),
        conditionalSelector);
      TEST_OBJ($$, @$);
    }
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

      TEST_RVAL(SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5), @$);
    }
  | property ':' maybe_spaces expr prio declaration_errors {
      /* p {color: red !important fail;} */

      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      TEST_RVAL(SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, $5), @$);

      SAC_SYNTAX_ERROR(@6,
        "unexpected token after property expression");
    }
  | property ':' maybe_spaces expr declaration_errors {
      /* p {color: red !important fail;} */

      SAC_LexicalUnit *expr;
      
      expr = SAC_lexical_unit_from_list($4, YY_SCANNER_MPOOL(scanner));
      TEST_OBJ(expr, @4);

      TEST_RVAL(SAC_parser_property_handler(YY_SCANNER_PARSER(scanner),
        $1, expr, SAC_FALSE), @$);

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
  : maybe_unary_operator INT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_int(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator REAL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_real(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator PERCENTAGE maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_percentage(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_PIXEL maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_pixel(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_INCH maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_inch(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_CENTIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_centimeter(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_MILLIMETER maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_millimeter(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_POINT maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_point(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_PICA maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_pica(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_EM maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_em(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator LENGTH_EX maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_ex(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator ANGLE_DEG maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_degree(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator ANGLE_RAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_radian(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator ANGLE_GRAD maybe_spaces {
      if ($1 == '-') $2 = -$2;
      $$ = SAC_lexical_unit_gradian(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator DIMEN maybe_spaces {
      double value;
      char *unit;

      value = strtod($2, &unit);
      if ($1 == '-') value = -value;

      $$ = SAC_lexical_unit_dimension(YY_SCANNER_MPOOL(scanner), unit, value);
    }
  | maybe_unary_operator TIME_MS maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative time not allowed");

      $$ = SAC_lexical_unit_millisecond(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator TIME_S maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative time not allowed");

      $$ = SAC_lexical_unit_second(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator FREQ_HZ maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative frequency not allowed");

      $$ = SAC_lexical_unit_hertz(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator FREQ_KHZ maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative frequency not allowed");

      $$ = SAC_lexical_unit_kilohertz(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator RESOLUTION_DPI maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative resolution not allowed");

      $$ = SAC_lexical_unit_dots_per_inch(YY_SCANNER_MPOOL(scanner), $2);
      TEST_OBJ($$, @$);
    }
  | maybe_unary_operator RESOLUTION_DPCM maybe_spaces {
      if ($1 == '-') SAC_SYNTAX_ERROR(@1,
        "negative resolution not allowed");

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
        r *= 17;
        g *= 17;
        b *= 17;
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
