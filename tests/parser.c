#include "parser.h"

#include <string.h>

#define USERDATA_FILE(userData) (FILE*)(userData)



static void start_document(void *userData) {
  fprintf(USERDATA_FILE(userData), "<document>");
}



static void end_document(void *userData) {
  fprintf(USERDATA_FILE(userData), "</document>");
}



static void namespace_declaration(void *userData,
  const SAC_STRING prefix, const SAC_STRING uri)
{
  fprintf(USERDATA_FILE(userData), "<namespace");
  if (prefix != NULL)
    fprintf(USERDATA_FILE(userData), " prefix=\"%s\"", prefix);
  if (uri != NULL)
    fprintf(USERDATA_FILE(userData), " uri=\"%s\"", uri);
  fprintf(USERDATA_FILE(userData), "/>");
}



static void dump_media(FILE *out, const SAC_STRING media[]) {
  const SAC_STRING *m;

  for (m = media; *m != NULL; ++m)
    fprintf(out, "<medium>%s</medium>", *m);
}



static void import(void *userData,
  const SAC_STRING base,
  const SAC_STRING uri,
  const SAC_STRING media[],
  const SAC_STRING defaultNamepaceURI)
{
  fprintf(USERDATA_FILE(userData), "<import");
  if (base != NULL)
    fprintf(USERDATA_FILE(userData), " base=\"%s\"", base);
  if (uri != NULL)
    fprintf(USERDATA_FILE(userData), " uri=\"%s\"", uri);
  if (defaultNamepaceURI != NULL)
    fprintf(USERDATA_FILE(userData),
      " defaultNamepaceURI=\"%s\"", defaultNamepaceURI);
  fprintf(USERDATA_FILE(userData), ">");

  dump_media(USERDATA_FILE(userData), media);

  fprintf(USERDATA_FILE(userData), "</import>");
}



static void start_media(void *userData, const SAC_STRING media[]) {
  fprintf(USERDATA_FILE(userData), "<media>");
  fprintf(USERDATA_FILE(userData), "<start_mediums>");
  dump_media(USERDATA_FILE(userData), media);
  fprintf(USERDATA_FILE(userData), "</start_mediums>");
}



static void end_media(void *userData, const SAC_STRING media[]) {
  fprintf(USERDATA_FILE(userData), "<end_mediums>");
  dump_media(USERDATA_FILE(userData), media);
  fprintf(USERDATA_FILE(userData), "</end_mediums>");
  fprintf(USERDATA_FILE(userData), "</media>");
}



static void dump_condition(FILE *out, const SAC_Condition *condition) {
  switch (condition->conditionType) {
    case SAC_ONE_OF_ATTRIBUTE_CONDITION:
    case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
    case SAC_ATTRIBUTE_CONDITION:
    case SAC_CLASS_CONDITION:
    case SAC_PSEUDO_CLASS_CONDITION:
    case SAC_ID_CONDITION:
      fprintf(out, "<condition type=");
      switch (condition->conditionType) {
        case SAC_ID_CONDITION:
          fprintf(out, "\"id\"");
          break;
        case SAC_CLASS_CONDITION:
          fprintf(out, "\"class\"");
          break;
        case SAC_ATTRIBUTE_CONDITION:
          fprintf(out, "\"attr\"");
          break;
        case SAC_ONE_OF_ATTRIBUTE_CONDITION:
          fprintf(out, "\"one_of\"");
          break;
        case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
          fprintf(out, "\"begin_hypen\"");
          break;
        case SAC_PSEUDO_CLASS_CONDITION:
          fprintf(out, "\"pseudo_class\"");
          break;
        default:
          fprintf(out, "\"unknown_%d\"", condition->conditionType);
          break;
      }

      if (condition->desc.attribute.namespaceURI != NULL)
        fprintf(out,
          " namespaceURI=\"%s\"", condition->desc.attribute.namespaceURI);

      if (condition->desc.attribute.localName != NULL)
        fprintf(out,
          " localName=\"%s\"", condition->desc.attribute.localName);

      fprintf(out, " specified=\"%s\"",
        condition->desc.attribute.specified == SAC_TRUE ? "true" : "false");

      if (condition->desc.attribute.value != NULL)
        fprintf(out,
          " value=\"%s\"", condition->desc.attribute.value);

      fprintf(out, "/>");
      break;
    case SAC_AND_CONDITION:
      fprintf(out, "<condition type=\"and\">");
      dump_condition(out, condition->desc.combinator.firstCondition);
      dump_condition(out, condition->desc.combinator.secondCondition);
      fprintf(out, "</condition>");
      break;
    case SAC_OR_CONDITION:
    case SAC_NEGATIVE_CONDITION:
    case SAC_POSITIONAL_CONDITION:
    case SAC_LANG_CONDITION:
    case SAC_ONLY_CHILD_CONDITION:
    case SAC_ONLY_TYPE_CONDITION:
    case SAC_CONTENT_CONDITION:
      fprintf(out,
        "<condition type=\"unknown_%d\"/>", condition->conditionType);
      break;
  }
}



static const char* node_type(SAC_nodeType type) {
  switch (type) {
    case ELEMENT_NODE: return "ELEMENT";
    case ATTRIBUTE_NODE: return "ATTRIBUTE";
    case TEXT_NODE: return "TEXT";
    case CDATA_NODE: return "CDATA";
    case ENTITY_REFERENCE_NODE: return "ENTITY_REFERENCE";
    case PROCESSING_INSTRUCTION_NODE: return "PROCESSING_INSTRUCTION";
    case COMMENT_NODE: return "COMMENT";
    case ANY_NODE: return "ANY";
  }
  return "UNKNOWN";
}



static void dump_selector(FILE *out, const SAC_Selector *selector) {
  switch (selector->selectorType) {
    case SAC_CONDITIONAL_SELECTOR:
      fprintf(out, "<selector type=\"conditional\">");
      dump_selector(out, selector->desc.conditional.simpleSelector);
      dump_condition(out, selector->desc.conditional.condition);
      fprintf(out, "</selector>");
      break;
    case SAC_ANY_NODE_SELECTOR:
      fprintf(out, "<selector type=\"any_node\"/>");
      break;
    case SAC_ELEMENT_NODE_SELECTOR:
      fprintf(out, "<selector type=\"element_node\"");

      if (selector->desc.element.namespaceURI != NULL)
        fprintf(out,
          " namespaceURI=\"%s\"", selector->desc.element.namespaceURI);

      if (selector->desc.element.localName != NULL)
        fprintf(out,
          " localName=\"%s\"", selector->desc.element.localName);

      fprintf(out, "/>");
      break;
    case SAC_DESCENDANT_SELECTOR:
    case SAC_CHILD_SELECTOR:
      fprintf(out, "<selector type=");
      switch (selector->selectorType) {
        case SAC_DESCENDANT_SELECTOR:
          fprintf(out, "\"descendant\"");
          break;
        case SAC_CHILD_SELECTOR:
          fprintf(out, "\"child\"");
          break;
        default:
          fprintf(out, "\"unknown_%d\"", selector->selectorType);
          break;
      }
      fprintf(out, ">");
      dump_selector(out, selector->desc.descendant.descendantSelector);
      dump_selector(out, selector->desc.descendant.simpleSelector);
      fprintf(out, "</selector>");
      break;
    case SAC_DIRECT_ADJACENT_SELECTOR:
      fprintf(out, "<selector type=\"sibling\" nodeType=\"%s\">",
        node_type(selector->desc.sibling.nodeType));
      dump_selector(out, selector->desc.sibling.firstSelector);
      dump_selector(out, selector->desc.sibling.secondSelector);
      fprintf(out, "</selector>");
      break;
    case SAC_PSEUDO_ELEMENT_SELECTOR:
    case SAC_ROOT_NODE_SELECTOR:
    case SAC_NEGATIVE_SELECTOR:
    case SAC_TEXT_NODE_SELECTOR:
    case SAC_CDATA_SECTION_NODE_SELECTOR:
    case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR:
    case SAC_COMMENT_NODE_SELECTOR:
      fprintf(out,
        "<selector type=\"unknown_%d\"/>", selector->selectorType);
      break;
  }
}



static void dump_selectors(FILE *out, const SAC_Selector **value) {
  const SAC_Selector **it;

  if (value == NULL) {
    fprintf(out, "NULL");
    return;
  }

  for (it = value; *it != NULL; ++it)
    dump_selector(out, *it);
}



static void start_style(void *userData, const SAC_Selector *selectors[]) {
  fprintf(USERDATA_FILE(userData), "<style>");
  fprintf(USERDATA_FILE(userData), "<start_selectors>");
  dump_selectors(USERDATA_FILE(userData), selectors);
  fprintf(USERDATA_FILE(userData), "</start_selectors>");
}



static void end_style(
  void *userData,
  const SAC_Selector *selectors[])
{
  fprintf(USERDATA_FILE(userData), "<end_selectors>");
  dump_selectors(USERDATA_FILE(userData), selectors);
  fprintf(USERDATA_FILE(userData), "</end_selectors>");
  fprintf(USERDATA_FILE(userData), "</style>");
}



static void dump_lexical_unit(FILE *out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    fprintf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_OPERATOR_COMMA:
        fprintf(out, "<comma/>");
        break;
      case SAC_INHERIT:
        fprintf(out, "<inherit/>");
        break;
      case SAC_INTEGER:
        fprintf(out, "<int>%li</int>", value->desc.integer);
        break;
      case SAC_REAL:
        fprintf(out, "<real>%g</real>", value->desc.real);
        break;
      case SAC_LENGTH_EM:
        fprintf(out, "<em dimension=\"%s\">%g</em>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_EX:
        fprintf(out, "<ex dimension=\"%s\">%g</ex>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_PIXEL:
        fprintf(out, "<pixel dimension=\"%s\">%g</pixel>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_INCH:
        fprintf(out, "<inch dimension=\"%s\">%g</inch>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_CENTIMETER:
        fprintf(out, "<centimeter dimension=\"%s\">%g</centimeter>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_MILLIMETER:
        fprintf(out, "<millimeter dimension=\"%s\">%g</millimeter>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_POINT:
        fprintf(out, "<point dimension=\"%s\">%g</point>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_PICA:
        fprintf(out, "<pica dimension=\"%s\">%g</pica>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_PERCENTAGE:
        fprintf(out, "<percentage dimension=\"%s\">%g</percentage>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_DEGREE:
        fprintf(out, "<degree dimension=\"%s\">%g</degree>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_GRADIAN:
        fprintf(out, "<gradian dimension=\"%s\">%g</gradian>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_RADIAN:
        fprintf(out, "<radian dimension=\"%s\">%g</radian>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_MILLISECOND:
        fprintf(out, "<msecs dimension=\"%s\">%g</msecs>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_SECOND:
        fprintf(out, "<secs dimension=\"%s\">%g</secs>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_HERTZ:
        fprintf(out, "<hertz dimension=\"%s\">%g</hertz>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_KILOHERTZ:
        fprintf(out, "<khertz dimension=\"%s\">%g</khertz>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_URI:
        fprintf(out, "<uri>%s</uri>", value->desc.uri);
        break;
      case SAC_RGBCOLOR:
      case SAC_FUNCTION:
        {
          SAC_LexicalUnit **arg;

          fprintf(out,
            "<func name=\"%s\">", value->desc.function.name);
          for (arg = value->desc.function.parameters; *arg != NULL; ++arg) {
            fprintf(out, "<arg>");
            dump_lexical_unit(out, *arg);
            fprintf(out, "</arg>");
          }
          fprintf(out, "</func>");
        }
        break;
      case SAC_IDENT:
        fprintf(out, "<ident>%s</ident>", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        fprintf(out, "<str>%s</str>", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        fprintf(out, "<urange>%s</urange>", value->desc.unicodeRange);
        break;
      case SAC_SUB_EXPRESSION:
        {
          SAC_LexicalUnit **sub;

          for (sub = value->desc.subValues; *sub != NULL; ++sub) {
            fprintf(out, "<sub>");
            dump_lexical_unit(out, *sub);
            fprintf(out, "</sub>");
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
        fprintf(out,
          "<value type=\"unknown_%d\"/>", value->lexicalUnitType);
        break;
    };
  }
}



static void property(
  void *userData,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  fprintf(USERDATA_FILE(userData),
    "<property name=\"%s\"", propertyName);
  if (important == SAC_TRUE)
    fprintf(USERDATA_FILE(userData), " important=\"important\"");
  fprintf(USERDATA_FILE(userData), ">");
  dump_lexical_unit(USERDATA_FILE(userData), value);
  fprintf(USERDATA_FILE(userData), "</property>");
}



SAC_Parser create_parser(FILE *out) {
  SAC_Parser parser;
  
  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetNamespaceDeclarationHandler(parser, namespace_declaration);
  SAC_SetImportHandler(parser, import);
  SAC_SetMediaHandler(parser, start_media, end_media);
  SAC_SetStyleHandler(parser, start_style, end_style);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, out);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  SAC_DisposeParser(parser);
}



void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void parse_styledeclaration(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleDeclaration(parser, buffer, strlen(buffer));
}



void parse_selectors(SAC_Parser parser, const char *buffer) {
  FILE *out;

  out = USERDATA_FILE(SAC_GetUserData(parser));
  fprintf(out, "<selectors>\n");
  dump_selectors(out, SAC_ParseSelectors(parser, buffer, strlen(buffer)));
  fprintf(out, "</selectors>\n");
}



void parse_rule(SAC_Parser parser, const char *buffer) {
  SAC_ParseRule(parser, buffer, strlen(buffer));
}