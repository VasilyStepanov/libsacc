#include "test_parser.h"

#include "test_utils.h"
#include "gcc.h"

#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



#define USERDATA(userData) ((struct userdata_t*)(userData))
#define USERDATA_STREAM(userData) USERDATA(userData)->stream
#define USERDATA_INDENT(userData) USERDATA(userData)->indent

struct userdata_t {
  stream_t stream;
  char *indent;
};



void* userdata_open(stream_t stream) {
  void *ret =
    (struct userdata_t*)malloc(sizeof(struct userdata_t));

  USERDATA_STREAM(ret) = stream;
  USERDATA_INDENT(ret) = (char*)malloc(sizeof(char));
  *USERDATA_INDENT(ret) = '\0';

  return ret;
}



void userdata_close(void *userdata) {
  free(USERDATA_INDENT(userdata));
  free(userdata);
}



void userdata_inc_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 + 2);
  indent[len + 0] = ' ';
  indent[len + 1] = ' ';
  indent[len + 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



void userdata_dec_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 - 2);
  indent[len - 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



void userdata_printf(void *userdata, const char *fmt, ...) {
  va_list args;

  stream_printf(USERDATA_STREAM(userdata), "%s", USERDATA_INDENT(userdata));
  va_start(args, fmt);
  stream_vprintf(USERDATA_STREAM(userdata), fmt, args);
  va_end(args);
}



void start_document(void *userData) {
  userdata_printf(userData, "doc {\n");
  userdata_inc_indent(userData);
}



void end_document(void *userData) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "doc }\n");
}



void start_style(void *userData, const SAC_Selector *selectors[] SAC_UNUSED) {
  userdata_printf(userData, "style {\n");
  userdata_inc_indent(userData);
}



void end_style(void *userData, const SAC_Selector *selectors[] SAC_UNUSED) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "style }\n");
}



void dump_lexical_unit(stream_t out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    stream_printf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_OPERATOR_COMMA:
        stream_printf(out, ",");
        break;
      case SAC_INHERIT:
        stream_printf(out, "inherit");
        break;
      case SAC_INTEGER:
        stream_printf(out, "int(%li)", value->desc.integer);
        break;
      case SAC_REAL:
        stream_printf(out, "real(%g)", value->desc.real);
        break;
      case SAC_LENGTH_EM:
        stream_printf(out, "em(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_EX:
        stream_printf(out, "ex(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PIXEL:
        stream_printf(out, "pixel(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_INCH:
        stream_printf(out, "inch(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_CENTIMETER:
        stream_printf(out, "centimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_MILLIMETER:
        stream_printf(out, "millimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_POINT:
        stream_printf(out, "point(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PICA:
        stream_printf(out, "pica(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_PERCENTAGE:
        stream_printf(out, "percentage(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_DEGREE:
        stream_printf(out, "degree(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_GRADIAN:
        stream_printf(out, "gradian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_RADIAN:
        stream_printf(out, "radian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_MILLISECOND:
        stream_printf(out, "msecs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_SECOND:
        stream_printf(out, "secs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_HERTZ:
        stream_printf(out, "hertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_KILOHERTZ:
        stream_printf(out, "khertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_URI:
        stream_printf(out, "uri('%s')", value->desc.uri);
        break;
      case SAC_RGBCOLOR:
      case SAC_FUNCTION:
        {
          SAC_LexicalUnit **arg;

          stream_printf(out, "func('%s')", value->desc.function.name);
          for (arg = value->desc.function.parameters; *arg != NULL; ++arg) {
            stream_printf(out, " arg(");
            dump_lexical_unit(out, *arg);
            stream_printf(out, ")");
          }
        }
        break;
      case SAC_IDENT:
        stream_printf(out, "ident('%s')", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        stream_printf(out, "str('%s')", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        stream_printf(out, "urange('%s')", value->desc.unicodeRange);
        break;
      case SAC_SUB_EXPRESSION:
        {
          SAC_LexicalUnit **sub;

          for (sub = value->desc.subValues; *sub != NULL; ++sub) {
            if (sub != value->desc.subValues) stream_printf(out, " ");
            stream_printf(out, "sub(");
            dump_lexical_unit(out, *sub);
            stream_printf(out, ")");
          }
        }
        break;
      case SAC_OPERATOR_PLUS:
      case SAC_OPERATOR_MINUS:
      case SAC_OPERATOR_MULTIPLY:
      case SAC_OPERATOR_SLASH:
      case SAC_OPERATOR_MOD:
      case SAC_OPERATOR_EXP:
      case SAC_OPERATOR_LT:
      case SAC_OPERATOR_GT:
      case SAC_OPERATOR_LE:
      case SAC_OPERATOR_GE:
      case SAC_OPERATOR_TILDE:
      case SAC_COUNTER_FUNCTION:
      case SAC_COUNTERS_FUNCTION:
      case SAC_ATTR:
      case SAC_RECT_FUNCTION:
      case SAC_DIMENSION:
        break;
    };
  }
}



void property(
  void *userData,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  userdata_printf(userData, "prop('%s') ", propertyName);
  dump_lexical_unit(USERDATA_STREAM(userData), value);
  if (important == SAC_TRUE)
    stream_printf(USERDATA_STREAM(userData), " important");
  stream_printf(USERDATA_STREAM(userData), "\n");
}



SAC_Parser create_parser(stream_t stream) {
  struct userdata_t *userData;
  SAC_Parser parser;
  
  userData = userdata_open(stream);

  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetStyleHandler(parser, start_style, end_style);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, userData);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  userdata_close(SAC_GetUserData(parser));
  SAC_DisposeParser(parser);
}



void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void parse_styledeclaration(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleDeclaration(parser, buffer, strlen(buffer));
}



const SAC_Selector** parse_selector(SAC_Parser parser, const char *buffer) {
  return SAC_ParseSelectors(parser, buffer, strlen(buffer));
}



void test_parser_styledeclaration() {
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t match_stream = stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  stream_printf(css, "prop-ident : ident;\n");
  stream_printf(css, "prop-inherit : inherit;\n");
  stream_printf(css, "prop-string : 'string' \"string\";\n");
  stream_printf(css, "prop-uri1 : url( \t\r\n\f\"uri\"\f\n\r\t );\n");
  stream_printf(css, "prop-uri2 : "
    "url( \t\r\n\fhttp://example.com/\f\n\r\t );\n");
  stream_printf(css, "prop-unicode : "
    "U+A5, U+0-7F, U+590-5ff, U+4E00-9FFF, U+30??;\n");
  stream_printf(css, "prop-int : 149 -148;\n");
  stream_printf(css, "prop-real : 3.14 -2.71;\n");
  stream_printf(css, "prop-percentage : 149%% -148%%;\n");
  stream_printf(css, "prop-freq : 50.1hz 5.1khz;\n");
  stream_printf(css, "prop-time : 25.1S 50.2MS;\n");
  stream_printf(css, "prop-angle : 90.0DEG 1.57RAD 100.0GRAD;\n");
  stream_printf(css, "prop-em : 1.1EM;\n");
  stream_printf(css, "prop-ex : 1.2EX;\n");
  stream_printf(css, "prop-px : 1.3PX;\n");
  stream_printf(css, "prop-cm : 1.4CM;\n");
  stream_printf(css, "prop-mm : 1.5MM;\n");
  stream_printf(css, "prop-in : 1.6IN;\n");
  stream_printf(css, "prop-pt : 1.7PT;\n");
  stream_printf(css, "prop-pc : 1.8PC;\n");
  stream_printf(css, "prop-func : foo('arg') bar('arg1', 2);\n");
  stream_printf(css, "prop-color : #ab #aBc #abcde #aBCdef;\n");
  parse_styledeclaration(parser, stream_str(css));
  stream_close(css);

  dispose_parser(parser);

  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream, "  prop('prop-ident') ident('ident')\n");
  stream_printf(match_stream, "  prop('prop-inherit') inherit\n");
  stream_printf(match_stream, "  prop('prop-string') "
    "sub(str('string')) sub(str('string'))\n");
  stream_printf(match_stream, "  prop('prop-uri1') uri('uri')\n");
  stream_printf(match_stream, "  prop('prop-uri2') "
    "uri('http://example.com/')\n");
  stream_printf(match_stream, "  prop('prop-unicode') "
    "sub(urange('U+A5')) sub(,) "
    "sub(urange('U+0-7F')) sub(,) "
    "sub(urange('U+590-5ff')) sub(,) "
    "sub(urange('U+4E00-9FFF')) sub(,) "
    "sub(urange('U+30\?\?'))\n");
  stream_printf(match_stream, "  prop('prop-int') "
    "sub(int(149)) sub(int(-148))\n");
  stream_printf(match_stream, "  prop('prop-real') "
    "sub(real(3.14)) sub(real(-2.71))\n");
  stream_printf(match_stream, "  prop('prop-percentage') "
    "sub(percentage(149%%)) sub(percentage(-148%%))\n");
  stream_printf(match_stream, "  prop('prop-freq') "
    "sub(hertz(50.1Hz)) sub(khertz(5.1kHz))\n");
  stream_printf(match_stream, "  prop('prop-time') "
    "sub(secs(25.1s)) sub(msecs(50.2ms))\n");
  stream_printf(match_stream, "  prop('prop-angle') "
    "sub(degree(90deg)) sub(radian(1.57rad)) sub(gradian(100grad))\n");
  stream_printf(match_stream, "  prop('prop-em') em(1.1em)\n");
  stream_printf(match_stream, "  prop('prop-ex') ex(1.2ex)\n");
  stream_printf(match_stream, "  prop('prop-px') pixel(1.3px)\n");
  stream_printf(match_stream, "  prop('prop-cm') centimeter(1.4cm)\n");
  stream_printf(match_stream, "  prop('prop-mm') millimeter(1.5mm)\n");
  stream_printf(match_stream, "  prop('prop-in') inch(1.6in)\n");
  stream_printf(match_stream, "  prop('prop-pt') point(1.7pt)\n");
  stream_printf(match_stream, "  prop('prop-pc') pica(1.8pc)\n");
  stream_printf(match_stream, "  prop('prop-func') "
    "sub(func('foo') arg(str('arg'))) "
    "sub(func('bar') arg(str('arg1')) arg(,) arg(int(2)))\n");
  stream_printf(match_stream, "  prop('prop-color') "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(10)) arg(,) arg(int(11)) arg(,) arg(int(12))) "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(171)) arg(,) "
                    "arg(int(205)) arg(,) "
                    "arg(int(239)))\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



void test_parser_selector() {
  const SAC_Selector **selectors;
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t match_stream = stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  stream_printf(css, "selector");
  selectors = parse_selector(parser, stream_str(css));
  stream_close(css);

  dispose_parser(parser);

  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



void test_parser() {
  test_parser_styledeclaration();
  test_parser_selector();
}
