#include "buffer.h"
#include "exit_stat.h"
#include "file_util.h"
#include "process_input.h"
#include <stdio.h>
#include <stdlib.h>

#define ARGMAX 2
#define ARGMIN 2

int main(int argc, char **argv) {
  if (argc > ARGMAX || argc < ARGMIN) {
    fprintf(stderr, "%s: incorrect amount of arguments\n", argv[0]);
    return EXIT_INV_ARG;
  }

  Buffer *file_buf = get_file_buf(argv[1]);
  Buffer *output = process_buf(file_buf);

  free(output->data);
  free(file_buf->data);
  free(output);
  free(file_buf);
  return EXIT_SUCCESS;
}
