#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "process_input.h"

#define EXTRA_SPACE 100

Buffer *process_buf(Buffer *file_buf) {
  Buffer *processed = malloc(sizeof(Buffer));
  processed->data = malloc(file_buf->used + EXTRA_SPACE);
  processed->len = file_buf->used + EXTRA_SPACE;
  processed->used = 0;

  printf("Line: ");
  for (int i = 0; i < file_buf->used; ++i) {
    char c = file_buf->data[i];
    if (c == '\n') {
      printf("\nLine: ");
    } else {
      printf("%c", c);
    }
  }
  printf("\n");

  return processed;
}


