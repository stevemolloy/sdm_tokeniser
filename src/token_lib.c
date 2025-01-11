#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "token_lib.h"
#include "sdm_lib.h"

const char *keywords[] = {
  "let",
  "int",
  "float",
  "Drift"
};

char *get_current_parser_string(Parser parser) {
  return parser.contents.data + parser.index;
}

void advance_to_next_line(Parser *parser) {
  while (parser->contents.data[parser->index] != '\n') {
    parser->index++;
  }
  parser->index++;
  if (parser->index < parser->contents.length) {
    parser->line++;
    parser->col = 1;
  }
}

bool starts_with_comment(Parser parser) {
  const char *comment_marker = "//";
  return strncmp(get_current_parser_string(parser), comment_marker, strlen(comment_marker)) == 0;
}

size_t starts_with_float(Parser parser) {
  char *endptr;
  char *input = get_current_parser_string(parser);
  errno = 0;
  strtod(input, &endptr);
  if (errno != 0) return 0;
  size_t retval = endptr - input;
  return retval;
}

Token get_next_token(Parser *parser) {
  parser_trim(parser);

  while (starts_with_comment(*parser)) {
    advance_to_next_line(parser);
    parser_trim(parser);
  };

  Token token = {0};
  memcpy(&token.source, parser, sizeof(*parser));
  size_t len; // Only valid for the float/int part of the code
  //
  if (parser->index >= parser->contents.length) {
    token.token_type = TOKEN_TYPE_EOF;
  } else if ((len = starts_with_float(*parser)) > 0) {
    // Could be an integer or a float
    char *start_ptr = parser->contents.data + parser->index;
    char *end_ptr = start_ptr;
    strtol(start_ptr, &end_ptr, 10);
    if ((end_ptr - start_ptr) == (long)len) {
      token.token_type = TOKEN_TYPE_INT;
    } else {
      token.token_type = TOKEN_TYPE_FLOAT;
    }
  }

  return token;
}

TokenArray tokenise_input_file(Parser *parser) {
  TokenArray token_array = {0};

  sdm_string_view contents = parser->contents;

  while (parser->index < contents.length) {
    get_next_token(parser);
  }
  
  return token_array;
}

void parser_trim(Parser *parser) {
  char *text = get_current_parser_string(*parser);
  while (strlen(text) > 0 && isspace(*text)) {
    if (*text == '\n') {
      parser->line++;
      parser->col = 1;
    } else {
      parser->col++;
    }
    parser->index++;
    text = get_current_parser_string(*parser);
  }
}

void parser_chop(Parser *parser, size_t len) {
  sdm_string_view *SV = &parser->contents;
  for (size_t i=0; SV->length>0 && i<len; i++) {
    if (SV->data[0] == '\n') {
      parser->line++;
      parser->col = 0;
    } else {
      parser->col++;
    }
    SV->data++;
    SV->length--;
  }
}

