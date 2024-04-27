#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void printStrList(List *list);
void readBufferLinesIntoList(List *list, char *buf, int bufLen);
void readFileIntoBuffer(char *name, char **target, int *size);

enum MarkdownElementTypes {
  HEADER1 = 1,
  HEADER2,
  HEADER3,
  HEADER4,
  HEADER5,
  HEADER6,
  BLOCK_QOUTE,
  LIST,
  ULIST,
  CODE_BLOCK,
  RULER,
  IMAGE
};
// for more elements:
// https://www.markdownguide.org/cheat-sheet/

int main(int argc, char **argv) {
  if (argc > 2 || argc < 2) {
    fprintf(stderr, "Too many or too few arguments.\n");
    return EXIT_FAILURE;
  }

  char *fileBuffer = NULL;
  int fileSize = 0;
  readFileIntoBuffer(argv[1], &fileBuffer, &fileSize);

  List *lines = CreateList(20);
  readBufferLinesIntoList(lines, fileBuffer, fileSize);
  free(fileBuffer);

  printStrList(lines);

  FreeList(lines);
  return 0;
}

void readFileIntoBuffer(char *name, char **target, int *size) {
  if (*target != NULL) {
    perror("There maybe a memory leak because target is not NULL in readFileIntoBuffer()");
  }

  FILE *file = fopen(name, "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file: %s", name);
    abort();
  }

  fseek(file, 0, SEEK_END);
  int fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  *target = (char *)malloc(sizeof(char) * fileSize + 1);
  if (target == NULL) {
    perror("Could not allocate space for file buffer.");
    abort();
  }
  fread(*target , 1, fileSize, file);
  (*target)[fileSize] = '\0';
  fclose(file);

  *size = fileSize;
}

void readBufferLinesIntoList(List *list, char *buf, int bufLen) {
  if (buf == NULL) {
    perror("Given buffer in readFileIntoBuffer() is NULL.");
    abort();
  }
  int startIndex = 0;
  int endIndex = 0;
  for (int i=0; i<bufLen; ++i) {
    if (buf[i] == '\n') {
      endIndex = i;
      int difference = endIndex - startIndex + 1;
      char *line = calloc(difference, sizeof(char));
      if (line == NULL) {
        perror("Could not allocate space for line in readBufferLinesIntoList().");
        abort();
      }
      for (int j=startIndex; j<endIndex; ++j) {
        char tmp[2] = {buf[j], '\0'};
        strcat(line, tmp);
      }
      startIndex = endIndex + 1;
      PushList(list, line);
    }
  }
}

void printStrList(List *list) {
  for (int i=0; i<list->size; ++i) {
    printf("Element: %s\n", (char *)list->elements[i]);
  }
}
