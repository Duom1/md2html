#include "buffer.h"
#include "exit_stat.h"
#include "process_input.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define EXTRA_SPACE 100

Buffer *process_buf(Buffer *file_buf) {
  int exit_code = 0;
  Buffer *processed = malloc(sizeof(Buffer));
  if (processed == NULL) {
    fprintf(stderr, "Failed to allocate momery in process_buf() for buffer");
    exit_code = EXIT_MAL_FAIL;
    goto processed_fail;
  }
  processed->data = malloc(file_buf->used + EXTRA_SPACE);
  if (processed->data == NULL) {
    fprintf(stderr, "Failed to allocate momery in process_buf() for data");
    exit_code = EXIT_MAL_FAIL;
    goto processed_data_fail;
  }
  processed->len = file_buf->used + EXTRA_SPACE;
  processed->used = 0;

  int start_i = 0;
  int end_i = 0;
  int dif_i = 0;
  bool expecting = false;
  for (int i = 0; i < file_buf->used; ++i) {
    char c = file_buf->data[i];
    if (c == '#') {
      start_i = i;
      expecting = true;
    } else if (c == '>') {
      start_i = i;
      expecting = true;
    } else if (c == '\n' && expecting) {
      end_i = i;
      dif_i = end_i - start_i;
      if (dif_i == 0) {
        fprintf(stderr, "index difference is too small: %i, %i", start_i,
                end_i);
        exit_code = EXIT_PARSE_FAIL;
        goto fail_parse;
      }
      int elem_s = dif_i + 1;
      char *element = malloc(elem_s);
      memset(element, 0, elem_s);
      for (int j = start_i; j <= end_i; ++j) {
        char tmp[2] = {file_buf->data[j], '\0'};
        strcat(element, tmp);
      }
      printf("%s", element);
      free(element);
      expecting = false;
    }
  }
  goto normal_return;

fail_parse:
  free(processed->data);

processed_data_fail:
  free(processed);

processed_fail:
  free(file_buf->data);
  free(file_buf);
  exit(exit_code);

normal_return:
  return processed;
}
