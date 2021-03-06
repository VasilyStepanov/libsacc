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
  yylloc->first_line = yylloc->last_line = yylineno; \
  yylloc->first_column = yycolumn; \
  yylloc->last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;


#define YY_USER_INIT \
  do { \
    yylineno = 1; \
    yycolumn = 1; \
  } while (0)

%}

h            [0-9a-f]
nonascii     [^\0-\177]
unicode      \\{h}{1,6}(\r\n|[ \n\r\t\f])?
escape       {unicode}|\\[^\n\r\f0-9a-f]
nmstart      [_a-z]|{nonascii}|{escape}
nmchar       [_a-z0-9-]|{nonascii}|{escape}
string1      \"([^\n\r\f\\"]|\\{nl}|{nonascii}|{escape})*\"
string2      \'([^\n\r\f\\']|\\{nl}|{nonascii}|{escape})*\'
badstring1   \"([^\n\r\f\\"]|\\{nl}|{nonascii}|{escape})*
badstring2   \'([^\n\r\f\\']|\\{nl}|{nonascii}|{escape})*
badcomment1  \/\*[^*]*\*+([^/*][^*]*\*+)*
badcomment2  \/\*[^*]*(\*+[^/*][^*]*)*
baduri1      url\({w}([!#$%&*-@A-Z\[\]-`a-z{-~]|{nonascii}|{escape})*{w}
baduri2      url\({w}{string}{w}
baduri3      url\({w}{badstring}
comment      \/\*[^*]*\*+([^/*][^*]*\*+)*\/
ident        [-]?{nmstart}{nmchar}*
name         {nmchar}+
int          [0-9]+
real         [0-9]*\.[0-9]+
num          {int}|{real}
string       {string1}|{string2}
badstring    {badstring1}|{badstring2}
badcomment   {badcomment1}|{badcomment2}
baduri       {baduri1}|{baduri2}|{baduri3}
url          ([!#$%&*-~]|{nonascii}|{escape})*
s            [ \t\r\n\f]+
w            {s}?
nl           \n|\r\n|\r|\f

A            a|\\0{0,4}(41|61)(\r\n|[ \t\r\n\f])?|\\a
C            c|\\0{0,4}(43|63)(\r\n|[ \t\r\n\f])?|\\c
D            d|\\0{0,4}(44|64)(\r\n|[ \t\r\n\f])?|\\d
E            e|\\0{0,4}(45|65)(\r\n|[ \t\r\n\f])?|\\e
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
V            v|\\0{0,4}(58|78)(\r\n|[ \t\r\n\f])?|\\v
X            x|\\0{0,4}(58|78)(\r\n|[ \t\r\n\f])?|\\x
Y            y|\\0{0,4}(59|79)(\r\n|[ \t\r\n\f])?|\\y
Z            z|\\0{0,4}(5a|7a)(\r\n|[ \t\r\n\f])?|\\z

range        \?{1,6}|{h}(\?{0,5}|{h}(\?{0,4}|{h}(\?{0,3}|{h}(\?{0,2}|{h}(\??|{h})))))
nth          [\+-]?{int}?{N}({w}[\+-]{w}{int})?

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
"^="                    {return PREFIXMATCH;}
"$="                    {return SUFFIXMATCH;}
"*="                    {return SUBSTRINGMATCH;}

{O}{N}{L}{Y}            {return MEDIA_ONLY;}
{N}{O}{T}               {return MEDIA_NOT;}
{A}{N}{D}               {return MEDIA_AND;}

{nth}                   {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext, yyleng);
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return NTH;
                        }

{string}                {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 2
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return STRING;
                        }
{badstring}             {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 1
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return BAD_STRING;
                        }

{ident}                 {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return IDENT;
                        }

"#"{name}               {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1,
                            yyleng - 1
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return HASH;
                        }

"+"                     {return PLUS;}
{w}">"                  {return GREATER;}
{w}","                  {return COMMA;}
{w}"~"                  {return TILDE;}

":"{N}{O}{T}"("         {return NOT;}

@{I}{M}{P}{O}{R}{T}     {return IMPORT_SYM;}
@{P}{A}{G}{E}           {return PAGE_SYM;}
"@top-left-corner"      {return TOP_LEFT_CORNER_SYM;}
"@top-left"             {return TOP_LEFT_SYM;}
"@top-center"           {return TOP_CENTER_SYM;}
"@top-right"            {return TOP_RIGHT_SYM;}
"@top-right-corner"     {return TOP_RIGHT_CORNER_SYM;}
"@bottom-left-corner"   {return BOTTOM_LEFT_CORNER_SYM;}
"@bottom-left"          {return BOTTOM_LEFT_SYM;}
"@bottom-center"        {return BOTTOM_CENTER_SYM;}
"@bottom-right"         {return BOTTOM_RIGHT_SYM;}
"@bottom-right-corner"  {return BOTTOM_RIGHT_CORNER_SYM;}
"@left-top"             {return LEFT_TOP_SYM;}
"@left-middle"          {return LEFT_MIDDLE_SYM;}
"@left-bottom"          {return LEFT_BOTTOM_SYM;}
"@right-top"            {return RIGHT_TOP_SYM;}
"@right-middle"         {return RIGHT_MIDDLE_SYM;}
"@right-bottom"         {return RIGHT_BOTTOM_SYM;}
@{M}{E}{D}{I}{A}        {return MEDIA_SYM;}
"@font-face"            {return FONT_FACE_SYM;}
"@charset "             {return CHARSET_SYM;}
@{N}{A}{M}{E}{S}{P}{A}{C}{E}  {return NAMESPACE_SYM;}
@{ident}                {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner),
                            yytext + 1, yyleng - 1);
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return ATKEYWORD_SYM;
                        }

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
{num}{D}{P}{I}          {
                          yylval->real = strtod(yytext, NULL);
                          return RESOLUTION_DPI;
                        }
{num}{D}{P}{C}{M}       {
                          yylval->real = strtod(yytext, NULL);
                          return RESOLUTION_DPCM;
                        }
{num}{ident}            {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng);
                          return DIMEN;
                        }
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
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
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
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return URI;
                        }
{baduri}                {
                          const char *text;
                          size_t len;
                          SAC_strntrim(
                            yytext + 4, yyleng - 4,
                            &text, &len,
                            " \t\r\n\f");
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), text, len
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return BAD_URI;
                        }
{ident}"("              {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng - 1
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return FUNCTION;
                        }

U\+{range}              {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return UNICODERANGE;
                        }
U\+{h}{1,6}-{h}{1,6}    {
                          yylval->str = SAC_mpool_strndup(
                            YY_SCANNER_MPOOL(yyscanner), yytext, yyleng
                          );
                          if (yylval->str == NULL)
                            YY_FATAL_ERROR("out of dynamic memory");
                          return UNICODERANGE;
                        }

.                       {return *yytext;}
