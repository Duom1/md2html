#include <stdio.h>
#include <stdlib.h>
#include "exit_stat.h"
#include "buffer.h"

Buffer *get_file_buf(char *file_name) {
  FILE *file_p = fopen(file_name, "r");

  if (file_p == NULL) {
    fprintf(stderr, "could not open file\n");
    exit(EXIT_FILE_ERR);
  }

  fseek(file_p, 0, SEEK_END);
  int file_s = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);

  Buffer *file_b = malloc(sizeof(Buffer));
  file_b->len = file_s + 1;
  file_b->used = file_b->len;
  file_b->data = malloc(file_b->len);

  if (file_b->data == NULL) {
    fprintf(stderr, "could not allocate space for file\n");
    fclose(file_p);
    exit(EXIT_FILE_ERR);
  }

  fread(file_b->data, 1, file_s, file_p);
  file_b->data[file_s] = '\0';

  fclose(file_p);

  return file_b;
}
