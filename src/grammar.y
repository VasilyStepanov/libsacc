/*
 * Reference: http://www.w3.org/TR/css3-syntax/#grammar0
 * TODO: Replace this with something more relevant.
 */
%{
#include "parser.h"

#include <sacc.h>

#include <string.h>

#define YYLEX_PARAM scanner
#define YYPARSE_PARAM scanner

void yyerror() {}
extern int yylex();
%}

%union {
int val;
char *str;
SAC_LexicalUnit *lunit;
}

%locations
%pure_parser

%start stylesheet

%token <val> ANGLE
%token <val> CDC
%token <val> CDO
%token <val> CHARSET_SYM
%token <val> DASHMATCH
%token <val> DIMEN
%token <val> EMS
%token <val> EXS
%token <val> FONT_FACE_SYM
%token <val> FREQ
%token <val> FUNCTION
%token <val> HASH
%token <str> IDENT
%token <val> INCLUDES
%token <val> IMPORT_SYM
%token <val> IMPORTANT_SYM
%token <val> LENGTH
%token <val> MEDIA_SYM
%token <val> NAMESPACE_SYM
%token <val> NUMBER
%token <val> PAGE_SYM
%token <val> PERCENTAGE
%token <val> S
%token <str> STRING
%token <val> TIME
%token <str> URI
%token <str> UNICODERANGE

%type <str> property;
%type <lunit> expr;
%type <lunit> term;

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
  : '/' _spaces0
  | ',' _spaces0
  |
  ;
combinator
  : '+' _spaces0
  | '>' _spaces0
  | /* empty */
  ;
unary_operator
  : '-'
  | '+'
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
  : property ':' _spaces0 expr      { parser_property_handler(YY_SCANNER_PARSER(scanner), $1, $4, SAC_FALSE); }
  | property ':' _spaces0 expr prio { parser_property_handler(YY_SCANNER_PARSER(scanner), $1, $4, SAC_TRUE); }
  | /* empty */
  ;
prio
  : IMPORTANT_SYM _spaces0
  ;
expr
  : term                { $$ = $1; }
  | expr operator term  { $$ = NULL; }
  ;
term
  : _unary_term
  | unary_operator _unary_term
  | STRING _spaces0             {
                                  $$ = mpool_alloc(YY_SCANNER_MPOOL(scanner), sizeof(SAC_LexicalUnit));
                                  $$->lexicalUnitType = SAC_STRING_VALUE;
                                  $$->desc.stringValue = $1;
                                }
  | IDENT _spaces0              {
                                  $$ = mpool_alloc(YY_SCANNER_MPOOL(scanner), sizeof(SAC_LexicalUnit));
                                  if (strcmp($1, "inherit") != 0) {
                                    $$->lexicalUnitType = SAC_IDENT;
                                    $$->desc.ident = $1;
                                  } else {
                                    $$->lexicalUnitType = SAC_INHERIT;
                                  }
                                }
  | URI _spaces0                {
                                  $$ = mpool_alloc(YY_SCANNER_MPOOL(scanner), sizeof(SAC_LexicalUnit));
                                  $$->lexicalUnitType = SAC_URI;
                                  $$->desc.uri = $1;
                                }
  | UNICODERANGE _spaces0       {
                                  $$ = mpool_alloc(YY_SCANNER_MPOOL(scanner), sizeof(SAC_LexicalUnit));
                                  $$->lexicalUnitType = SAC_UNICODERANGE;
                                  $$->desc.unicodeRange = $1;
                                }
  | hexcolor
  ;
_unary_term
  : NUMBER _spaces0
  | PERCENTAGE _spaces0
  | LENGTH _spaces0
  | EMS _spaces0
  | EXS _spaces0
  | ANGLE _spaces0
  | TIME _spaces0
  | FREQ _spaces0
  | function
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
