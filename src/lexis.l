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
#include "string_extra.h"
#include "parser.h"

#include <stdlib.h>
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
int       [0-9]+
real      [0-9]*"."[0-9]+
num       {int}|{real}
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

"#"{name}               {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 1
                          );
                          return HASH;
                        }

"@import"               {return IMPORT_SYM;}
"@page"                 {return PAGE_SYM;}
"@media"                {return MEDIA_SYM;}
"@font-face"            {return FONT_FACE_SYM;}
"@charset"              {return CHARSET_SYM;}
"@namespace"            {return NAMESPACE_SYM;}

"!{w}important"         {return IMPORTANT_SYM;}

{num}em                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_EM;
                        }
{num}ex                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_EX;
                        }
{num}px                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_PIXEL;
                        }
{num}cm                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_CENTIMETER;
                        }
{num}mm                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_MILLIMETER;
                        }
{num}in                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_INCH;
                        }
{num}pt                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_POINT;
                        }
{num}pc                 {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_PICA;
                        }
{num}deg                {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_DEG;
                        }
{num}rad                {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_RAD;
                        }
{num}grad               {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_GRAD;
                        }
{num}ms                 {
                          yylval->real = strtod(yytext, NULL);
                          return TIME_MS;
                        }
{num}s                  {
                          yylval->real = strtod(yytext, NULL);
                          return TIME_S;
                        }
{num}Hz                 {
                          yylval->real = strtod(yytext, NULL);
                          return FREQ_HZ;
                        }
{num}kHz                {
                          yylval->real = strtod(yytext, NULL);
                          return FREQ_KHZ;
                        }
{num}{ident}            {return DIMEN;}
{num}%                  {
                          yylval->real = strtod(yytext, NULL);
                          return PERCENTAGE;
                        }
{int}                   {
                          yylval->integer = strtol(yytext, NULL, 10);
                          return INT;
                        }
{real}                  {
                          yylval->real = strtod(yytext, NULL);
                          return REAL;
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
{ident}"("              {
                          yylval->str = mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng - 1
                          );
                          return FUNCTION;
                        }

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
