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
bool test_comments_and_numbers(void);
bool test_general_text(void);

TestFunction tests[] = {
  test_comments,
  test_comments_and_numbers,
  test_general_text,
};

int main(void) {
  size_t num_tests = sizeof(tests) / sizeof(tests[0]);

  size_t passed_tests = 0;
  for (size_t i=0; i<num_tests; i++) {
    if (tests[i]()) passed_tests++;
  }

  fprintf(stdout, "=======================\n");
  fprintf(stdout, "TESTS PASSED: %zu / %zu\n", passed_tests, num_tests);
  fprintf(stdout, "=======================\n");

  sdm_arena_free(active_arena);

  return num_tests - passed_tests;
}

bool test_general_text(void) {
  const char *test_name = "GENERAL TEXT TEST";
  const char *input_filename = "examples/general_text.txt";
  const char *expected_filename = "tests/general_expected.txt";
  const char *actual_filename = "tests/general_actual.txt";

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

  FILE *output = fopen(actual_filename, "w");
  if (output == NULL) {
    fprintf(stderr, "Couldn't open %s\n", actual_filename);
    return false;
  }

  for (size_t i=0; i<token_array.length; i++) {
    Token token = token_array.data[i];
    fprintf(output, "%d :: ", token_array.data[i].token_type);
    if (token.token_type == TOKEN_TYPE_STRING) {
      fprintf(output, "%s", token.as.str_token.value);
    } else if (token.token_type == TOKEN_TYPE_INT) {
      fprintf(output, "%ld", token.as.int_token.value);
    } else if (token.token_type == TOKEN_TYPE_FLOAT) {
      fprintf(output, "%f", token.as.float_token.value);
    } else if (token.token_type == TOKEN_TYPE_ID) {
      fprintf(output, "%s", token.as.id_token.value);
    }
    fprintf(output, "\n");
  }

  fclose(output);

  return compare_files(test_name, expected_filename, actual_filename);
}

bool test_ids(void) {
  const char *test_name = "ID'S TEST";
  const char *input_filename = "examples/ids.txt";
  const char *expected_filename = "tests/ids_expected.txt";
  const char *actual_filename = "tests/ids_actual.txt";

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

  if (token_array.length != 1) {
    fprintf(stderr, "%s FAILED: expected 1 token but found %zu\n", test_name, token_array.length);
    return false;
  }

  if (token_array.data[0].token_type != TOKEN_TYPE_EOF) {
    fprintf(stderr, "%s FAILED: expected EOF token but found something else\n", test_name);
    return false;
  }

  FILE *result_file = fopen(actual_filename, "w");
  fprintf(result_file, "Found %zu tokens, %zu lines, and %zu characters in %s\n", 
          token_array.length, parser.line, parser.index, parser.filename);
  fclose(result_file);

  bool comparison_result = compare_files(test_name, expected_filename, actual_filename);

  return comparison_result;
}

bool test_comments_and_numbers(void) {
  const char *test_name = "COMMENTS_AND_NUMBERS TEST";
  const char *input_filename = "examples/comments_and_numbers.txt";
  const char *expected_filename = "tests/comments_and_numbers_expected.txt";
  const char *actual_filename = "tests/comments_and_numbers_actual.txt";

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

  if (token_array.length != 3) {
    fprintf(stderr, "%s FAILED: expected 3 tokens but found %zu\n", test_name, token_array.length);
    return false;
  }

  if (token_array.data[0].token_type != TOKEN_TYPE_INT) {
    fprintf(stderr, "%s FAILED: expected TOKEN_TYPE_INT token but found something else\n", test_name);
    return false;
  }

  if (token_array.data[1].token_type != TOKEN_TYPE_FLOAT) {
    fprintf(stderr, "%s FAILED: expected TOKEN_TYPE_FLOAT token but found something else\n", test_name);
    return false;
  }

  if (token_array.data[0].as.int_token.value != 42) {
    fprintf(stderr, "%s FAILED: expected TOKEN_TYPE_INT token with value %d but found %ld\n", 
            test_name, 42, token_array.data[0].as.int_token.value);
    return false;
  }

  if (token_array.data[1].as.float_token.value != 42.42) {
    fprintf(stderr, "%s FAILED: expected TOKEN_TYPE_FLOAT token with value %f but found %f\n", 
            test_name, 42.42, token_array.data[1].as.float_token.value);
    return false;
  }

  FILE *result_file = fopen(actual_filename, "w");
  fprintf(result_file, "Found %zu tokens, %zu lines, and %zu characters in %s\n", 
          token_array.length, parser.line, parser.index, parser.filename);
  fclose(result_file);

  bool comparison_result = compare_files(test_name, expected_filename, actual_filename);

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

  if (token_array.length != 1) {
    fprintf(stderr, "%s FAILED: expected 1 token but found %zu\n", test_name, token_array.length);
    return false;
  }

  if (token_array.data[0].token_type != TOKEN_TYPE_EOF) {
    fprintf(stderr, "%s FAILED: expected EOF token but found something else\n", test_name);
    return false;
  }

  FILE *result_file = fopen(actual_filename, "w");
  fprintf(result_file, "Found %zu tokens, %zu lines, and %zu characters in %s\n", 
          token_array.length, parser.line, parser.index, parser.filename);
  fclose(result_file);

  bool comparison_result = compare_files(test_name, expected_filename, actual_filename);

  return comparison_result;
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

