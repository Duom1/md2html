#include <stdio.h>
#include <stdlib.h>
#include "exit_stat.h"
#include "buffer.h"

Buffer *get_file_buf(char *file_name) {
  int exit_code = 0;
  FILE *file_p = fopen(file_name, "r");

  if (file_p == NULL) {
    fprintf(stderr, "could not open file\n");
    exit_code = EXIT_FILE_ERR;
    goto just_exit;
  }

  fseek(file_p, 0, SEEK_END);
  int file_s = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);

  Buffer *file_b = malloc(sizeof(Buffer));
  if (file_b == NULL) {
    fprintf(stderr, "failed to allocate memory for file_b in get_file_buf()");
    exit_code = EXIT_MAL_FAIL;
    goto close_file_exit;
  }
  file_b->len = file_s + 1;
  file_b->used = file_b->len;
  file_b->data = malloc(file_b->len);
  if (file_b->data == NULL) {
    fprintf(stderr, "failed to allocate memory for file_b->data in get_file_buf()");
    exit_code = EXIT_MAL_FAIL;
    goto free_file_exit;
  }

  fread(file_b->data, 1, file_s, file_p);
  file_b->data[file_s] = '\0';
  fclose(file_p);
  goto normal_return;

free_file_exit:
  free(file_b);
close_file_exit:
  fclose(file_p);
just_exit:
  exit(exit_code);

normal_return:
  return file_b;
}
