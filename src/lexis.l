/*
 * Reference: http://www.w3.org/TR/css3-syntax/#lexical
 * TODO: Replace this with something more relevant.
 */
%option case-insensitive
%option bison-bridge bison-locations
%option noyywrap
%option noinput nounput
%option reentrant
%option noyyalloc noyyrealloc noyyfree
%{
#include <sacc.h>

#include "grammar.h"
#include "mpool.h"
#include "mpool_extra.h"
#include "parser.h"
#include "string_extra.h"
%}

h         [0-9a-f]
nonascii  [\200-\377]
unicode   \\{h}{1,6}[ \t\r\n\f]?
escape    {unicode}|\\[ -~\200-\377]
nmstart   [a-z]|{nonascii}|{escape}
nmchar    [a-z0-9-]|{nonascii}|{escape}
string1   \"([\t !#$%&(-~]|\\{nl}|\'|{nonascii}|{escape})*\"
string2   \'([\t !#$%&(-~]|\\{nl}|\"|{nonascii}|{escape})*\'

ident     [-]?{nmstart}{nmchar}*
name      {nmchar}+
num       [0-9]+|[0-9]*"."[0-9]+
string    {string1}|{string2}
url       ([!#$%&*-~]|{nonascii}|{escape})*
w         [ \t\r\n\f]*
nl        \n|\r\n|\r|\f
range     \?{1,6}|{h}(\?{0,5}|{h}(\?{0,4}|{h}(\?{0,3}|{h}(\?{0,2}|{h}(\??|{h})))))

%%

[ \t\r\n\f]+            {return S;}

\/\*[^*]*\*+([^/][^*]*\*+)*\/ /* ignore comments */

"<!--"                  {return CDO;}
"-->"                   {return CDC;}
"~="                    {return INCLUDES;}
"|="                    {return DASHMATCH;}

{string}                {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 2
                          );
                          return STRING;
                        }

{ident}                 {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return IDENT;
                        }

"#"{name}               {return HASH;}

"@import"               {return IMPORT_SYM;}
"@page"                 {return PAGE_SYM;}
"@media"                {return MEDIA_SYM;}
"@font-face"            {return FONT_FACE_SYM;}
"@charset"              {return CHARSET_SYM;}
"@namespace"            {return NAMESPACE_SYM;}

"!{w}important"         {return IMPORTANT_SYM;}

{num}em                 {return EMS;}
{num}ex                 {return EXS;}
{num}px                 {return LENGTH;}
{num}cm                 {return LENGTH;}
{num}mm                 {return LENGTH;}
{num}in                 {return LENGTH;}
{num}pt                 {return LENGTH;}
{num}pc                 {return LENGTH;}
{num}deg                {return ANGLE;}
{num}rad                {return ANGLE;}
{num}grad               {return ANGLE;}
{num}ms                 {return TIME;}
{num}s                  {return TIME;}
{num}Hz                 {return FREQ;}
{num}kHz                {return FREQ;}
{num}{ident}            {return DIMEN;}
{num}%                  {return PERCENTAGE;}
{num}                   {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return NUMBER;
                        }

"url("{w}{string}{w}")" {
                          const char *text;
                          size_t len;
                          strntrim(yytext + 4, yyleng - 5, &text, &len, " \t\r\n\f");
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), text + 1, len - 2
                          );
                          return URI;
                        }
"url("{w}{url}{w}")"    {
                          const char *text;
                          size_t len;
                          strntrim(yytext + 4, yyleng - 5, &text, &len, " \t\r\n\f");
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), text, len
                          );
                          return URI;
                        }
{ident}"("              {return FUNCTION;}

U\+{range}              {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return UNICODERANGE;
                        }
U\+{h}{1,6}-{h}{1,6}    {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return UNICODERANGE;
                        }

.                       {return *yytext;}
