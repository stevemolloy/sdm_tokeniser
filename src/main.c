#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <strings.h>

#include <sys/stat.h>
#include <sys/sysmacros.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "sdm_lib.h"

#define EXTERN
#include "token_lib.h"

#define SDM_ARRAY_LENGTH(array) sizeof((array)) / sizeof((array[0]))

char *token_type_strings[] = {
  [TOKEN_TYPE_ID]         = "TOKEN_TYPE_ID",
  [TOKEN_TYPE_FLOAT]      = "TOKEN_TYPE_FLOAT",
  [TOKEN_TYPE_INT]        = "TOKEN_TYPE_INT",
  [TOKEN_TYPE_STRING]     = "TOKEN_TYPE_STRING",
  [TOKEN_TYPE_KEYWORD]    = "TOKEN_TYPE_KEYWORD",
  [TOKEN_TYPE_ASSIGNMENT] = "TOKEN_TYPE_ASSIGNMENT",
  [TOKEN_TYPE_ADD]        = "TOKEN_TYPE_ADD",
  [TOKEN_TYPE_MULT]       = "TOKEN_TYPE_MULT",
  [TOKEN_TYPE_SUB]        = "TOKEN_TYPE_SUB",
  [TOKEN_TYPE_DIV]        = "TOKEN_TYPE_DIV",
  [TOKEN_TYPE_OPAREN]     = "TOKEN_TYPE_OPAREN",
  [TOKEN_TYPE_CPAREN]     = "TOKEN_TYPE_CPAREN",
  [TOKEN_TYPE_SEMICOLON]  = "TOKEN_TYPE_SEMICOLON",
  [TOKEN_TYPE_COLON]      = "TOKEN_TYPE_COLON",
  [TOKEN_TYPE_COMMA]      = "TOKEN_TYPE_COMMA",
  [TOKEN_TYPE_POINT]      = "TOKEN_TYPE_POINT",
  [TOKEN_TYPE_EOF]        = "TOKEN_TYPE_EOF",
  [TOKEN_TYPE_QUOTEMARK]  = "TOKEN_TYPE_QUOTEMARK"
};
static_assert(SDM_ARRAY_LENGTH(token_type_strings) == TOKEN_TYPE_COUNT, 
              "Increasing the number of TokenTypes implies a change here as well");

static sdm_arena_t main_arena = {0};
static sdm_arena_t *active_arena = &main_arena;

void *active_alloc(size_t size)              { return sdm_arena_alloc(active_arena, size); }
void *active_realloc(void *ptr, size_t size) { return sdm_arena_realloc(active_arena, ptr, size); }

int main(void) {
  char *input_filename = "examples/comments.txt";
  char *buffer = sdm_read_entire_file(input_filename);

  Parser parser = {
    .filename = input_filename,
    .contents = sdm_cstr_as_sv(buffer),
    .col = 1,
    .line = 1,
    .index = 0,
  };

  TokenArray token_array = {0};
  tokenise_input_file(&parser, &token_array);
  printf("Found %zu tokens, %zu lines, and %zu characters in %s\n", token_array.length, parser.line, parser.index, parser.filename);

  sdm_arena_free(&main_arena);

  return 0;
}

