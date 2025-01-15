#include <stdio.h>

#include "sdm_lib.h"

#define EXTERN
#include "token_lib.h"

static sdm_arena_t main_arena = {0};
static sdm_arena_t *active_arena = &main_arena;

void *active_alloc(size_t size)              { return sdm_arena_alloc(active_arena, size); }
void *active_realloc(void *ptr, size_t size) { return sdm_arena_realloc(active_arena, ptr, size); }

bool compare_files(const char *testname, const char *filename1, const char *filename2);

typedef bool (*TestFunction)(void);
bool test_comments(void);

int main(void) {
  test_comments();

  sdm_arena_free(active_arena);

  return 0;
}

bool compare_files(const char *testname, const char *filename1, const char *filename2) {
  bool comparison_result = true;
  char *expected_buff = sdm_read_entire_file(filename1);
  char *result_buff = sdm_read_entire_file(filename2);
  
  if (strcmp(expected_buff, result_buff) != 0) {
    comparison_result = false;
  } else {
    comparison_result = true;
  }

  if (!comparison_result) {
    printf("%s FAILED\n", testname);
    printf("    Expected (%s) does not match actual (%s)\n", filename1, filename2);
    comparison_result = false;
  } else {
    printf("%s PASSED\n", testname);
  }

  return comparison_result;
}

bool test_comments(void) {
  const char *test_name = "COMMENTS TEST";
  const char *input_filename = "examples/comments.txt";
  const char *expected_filename = "tests/comments_expected.txt";
  const char *actual_filename = "tests/comments_actual.txt";

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

  FILE *result_file = fopen(actual_filename, "w");
  fprintf(result_file, "Found %zu tokens, %zu lines, and %zu characters in %s\n", 
          token_array.length, parser.line, parser.index, parser.filename);
  fclose(result_file);

  sdm_arena_free(&main_arena);

  bool comparison_result = compare_files(test_name, expected_filename, actual_filename);

  return comparison_result;
}

