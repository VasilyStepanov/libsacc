/*
 * References:
 *  - http://www.w3.org/TR/CSS21/grammar.html#scanner
 *  - http://www.w3.org/TR/CSS21/grammar.html#q4
 *  - http://www.w3.org/TR/css3-mediaqueries/#syntax
 *  - http://www.w3.org/TR/css3-namespace/#syntax
 *  - http://www.w3.org/TR/css3-selectors/#lex
 *
 *  - http://www.w3.org/TR/css3-fonts/
 */
%option case-insensitive
%option bison-bridge bison-locations
%option noyywrap
%option noinput nounput
%option reentrant
%option noyyalloc noyyrealloc noyyfree
%option yylineno
%{
#include <sacc.h>

#include "grammar.h"
#include "mpool.h"
#include "mpool_extra.h"
#include "string_extra.h"
#include "parser.h"

#include <stdlib.h>


#define YY_USER_ACTION \
  yylloc->first_line = yylloc->last_line = yylineno + 1; \
  yylloc->first_column = yycolumn; \
  yylloc->last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;  



%}

h            [0-9a-f]
nonascii     [\240-\377]
unicode      \\{h}{1,6}(\r\n|[ \t\r\n\f])?
escape       {unicode}|\\[^\r\n\f0-9a-f]
nmstart      [_a-z]|{nonascii}|{escape}
nmchar       [_a-z0-9-]|{nonascii}|{escape}
string1      \"([^\n\r\f\\"]|\\{nl}|{escape})*\"
string2      \'([^\n\r\f\\']|\\{nl}|{escape})*\'
badstring1   \"([^\n\r\f\\"]|\\{nl}|{escape})*\\?
badstring2   \'([^\n\r\f\\']|\\{nl}|{escape})*\\?
badcomment1  \/\*[^*]*\*+([^/*][^*]*\*+)*
badcomment2  \/\*[^*]*(\*+[^/*][^*]*)*
baduri1      url\({w}([!#$%&*-\[\]-~]|{nonascii}|{escape})*{w}
baduri2      url\({w}{string}{w}
baduri3      url\({w}{badstring}
comment      \/\*[^*]*\*+([^/*][^*]*\*+)*\/
ident        -?{nmstart}{nmchar}*
name         {nmchar}+
int          [0-9]+
real         [0-9]*"."[0-9]+
num          {int}|{real}
string       {string1}|{string2}
badstring    {badstring1}|{badstring2}
badcomment   {badcomment1}|{badcomment2}
baduri       {baduri1}|{baduri2}|{baduri3}
url          ([!#$%&*-~]|{nonascii}|{escape})*
s            [ \t\r\n\f]+
w            {s}?
nl           \n|\r\n|\r|\f

A            a|\\0{0,4}(41|61)(\r\n|[ \t\r\n\f])?
C            c|\\0{0,4}(43|63)(\r\n|[ \t\r\n\f])?
D            d|\\0{0,4}(44|64)(\r\n|[ \t\r\n\f])?
E            e|\\0{0,4}(45|65)(\r\n|[ \t\r\n\f])?
G            g|\\0{0,4}(47|67)(\r\n|[ \t\r\n\f])?|\\g
H            h|\\0{0,4}(48|68)(\r\n|[ \t\r\n\f])?|\\h
I            i|\\0{0,4}(49|69)(\r\n|[ \t\r\n\f])?|\\i
K            k|\\0{0,4}(4b|6b)(\r\n|[ \t\r\n\f])?|\\k
L            l|\\0{0,4}(4c|6c)(\r\n|[ \t\r\n\f])?|\\l
M            m|\\0{0,4}(4d|6d)(\r\n|[ \t\r\n\f])?|\\m
N            n|\\0{0,4}(4e|6e)(\r\n|[ \t\r\n\f])?|\\n
O            o|\\0{0,4}(4f|6f)(\r\n|[ \t\r\n\f])?|\\o
P            p|\\0{0,4}(50|70)(\r\n|[ \t\r\n\f])?|\\p
R            r|\\0{0,4}(52|72)(\r\n|[ \t\r\n\f])?|\\r
S            s|\\0{0,4}(53|73)(\r\n|[ \t\r\n\f])?|\\s
T            t|\\0{0,4}(54|74)(\r\n|[ \t\r\n\f])?|\\t
U            u|\\0{0,4}(55|75)(\r\n|[ \t\r\n\f])?|\\u
X            x|\\0{0,4}(58|78)(\r\n|[ \t\r\n\f])?|\\x
Z            z|\\0{0,4}(5a|7a)(\r\n|[ \t\r\n\f])?|\\z

range        \?{1,6}|{h}(\?{0,5}|{h}(\?{0,4}|{h}(\?{0,3}|{h}(\?{0,2}|{h}(\??|{h})))))

%%

%{
  if (YY_SCANNER_START_TOKEN(yyscanner) != 0) {
    int ret = YY_SCANNER_START_TOKEN(yyscanner);

    YY_SCANNER_START_TOKEN(yyscanner) = 0;
    return ret;
  }
%}

{s}                     {return S;}

\/\*[^*]*\*+([^/*][^*]*\*+)*\/ /* ignore comments */
{badcomment}                   /* unclosed comment at EOF */

"<!--"                  {return CDO;}
"-->"                   {return CDC;}
"~="                    {return INCLUDES;}
"|="                    {return DASHMATCH;}

{string}                {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 2
                          );
                          return STRING;
                        }
{badstring}             {return BAD_STRING;}

{ident}                 {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return IDENT;
                        }

"#"{name}               {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 1
                          );
                          return HASH;
                        }

@{I}{M}{P}{O}{R}{T}     {return IMPORT_SYM;}
@{P}{A}{G}{E}           {return PAGE_SYM;}
@{M}{E}{D}{I}{A}        {return MEDIA_SYM;}
"@font-face"            {return FONT_FACE_SYM;}
"@charset "             {return CHARSET_SYM;}
@{N}{A}{M}{E}{S}{P}{A}{C}{E}  {return NAMESPACE_SYM;}

"!"({w}|{comment})*{I}{M}{P}{O}{R}{T}{A}{N}{T}  {return IMPORTANT_SYM;}


{num}{E}{M}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_EM;
                        }
{num}{E}{X}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_EX;
                        }
{num}{P}{X}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_PIXEL;
                        }
{num}{C}{M}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_CENTIMETER;
                        }
{num}{M}{M}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_MILLIMETER;
                        }
{num}{I}{N}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_INCH;
                        }
{num}{P}{T}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_POINT;
                        }
{num}{P}{C}             {
                          yylval->real = strtod(yytext, NULL);
                          return LENGTH_PICA;
                        }
{num}{D}{E}{G}          {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_DEG;
                        }
{num}{R}{A}{D}          {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_RAD;
                        }
{num}{G}{R}{A}{D}       {
                          yylval->real = strtod(yytext, NULL);
                          return ANGLE_GRAD;
                        }
{num}{M}{S}             {
                          yylval->real = strtod(yytext, NULL);
                          return TIME_MS;
                        }
{num}{S}                {
                          yylval->real = strtod(yytext, NULL);
                          return TIME_S;
                        }
{num}{H}{Z}             {
                          yylval->real = strtod(yytext, NULL);
                          return FREQ_HZ;
                        }
{num}{K}{H}{Z}          {
                          yylval->real = strtod(yytext, NULL);
                          return FREQ_KHZ;
                        }
{num}{ident}            {return DIMEN;}
{num}%                  {
                          yylval->real = strtod(yytext, NULL);
                          return PERCENTAGE;
                        }
{int}                 {
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
                          SAC_strntrim(
                            yytext + 4, yyleng - 5,
                            &text, &len,
                            " \t\r\n\f");
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), text + 1, len - 2
                          );
                          return URI;
                        }
"url("{w}{url}{w}")"    {
                          const char *text;
                          size_t len;
                          SAC_strntrim(
                            yytext + 4, yyleng - 5,
                            &text, &len,
                            " \t\r\n\f");
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), text, len
                          );
                          return URI;
                        }
{baduri}                {return BAD_URI;}
{ident}"("              {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng - 1
                          );
                          return FUNCTION;
                        }

U\+{range}              {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return UNICODERANGE;
                        }
U\+{h}{1,6}-{h}{1,6}    {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          return UNICODERANGE;
                        }

.                       {return *yytext;}
