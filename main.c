#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "exit_stat.h"

void printStrList(List *list);

int main(int argc, char **argv) {
  if (argc > 2 || argc < 2) {
    fprintf(stderr, "Too many or too few arguments.\n");
    return EXIT_INV_ARG;
  }
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file %s", argv[1]);
    return EXIT_FILE_ERR;
  }
  fseek(file, 0, SEEK_END);
  int fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *fileBuffer = malloc(sizeof(char) * fileSize + 1);
  if (fileBuffer == NULL) {
    perror("Could not allocate space for file buffer.");
    fclose(file);
    return EXIT_MAL_FAIL;
  }

  fread(fileBuffer, 1, fileSize, file);
  fileBuffer[fileSize] = '\0';
  fclose(file);

  List *lines = CreateList(20);
  printf("%i, %i\n", lines->size, lines->capcity);
  int startIndex = 0;
  int endIndex = 0;
  for (int i=0; i<fileSize; ++i) {
    if (fileBuffer[i] == '\n') {
      endIndex = i;
      int difference = endIndex - startIndex + 1;
      char *line = calloc(difference, sizeof(char));
      if (line == NULL) {
        perror("Could not allocate space for line.");
        FreeList(lines);
        free(fileBuffer);
        return EXIT_MAL_FAIL;
      }
      for (int j=startIndex; j<endIndex; ++j) {
        char tmp[2] = {fileBuffer[j], '\0'};
        strcat(line, tmp);
      }
      startIndex = endIndex + 1;
      PushList(lines, line);
    }
  }
  free(fileBuffer);

  printf("%i, %i\n", lines->size, lines->capcity);

  //printStrList(lines);

  FreeList(lines);
  return 0;
}

void printStrList(List *list) {
  for (int i=0; i<list->size; ++i) {
    printf("Line: %s\n", (char *)list->elements[i]);
  }
}
