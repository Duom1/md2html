#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for more elements:
// https://www.markdownguide.org/cheat-sheet/
enum MarkdownElementTypes {
  UNDEF = 0,
  HEADER1,
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
  IMAGE,
};

// clang-format off
char MDElementTypeNames[13][15] = {
  "undefined",
  "header 1",
  "header 2",
  "header 3",
  "header 4",
  "header 5",
  "header 6",
  "block qoute",
  "list",
  "unordered list",
  "code clock",
  "ruler",
  "image",
};
// clang-format on

typedef int MDLineType;
typedef struct {
  MDLineType type;
  char *data;
} MDLine_t;

void printList(List *list);
void readBufferLinesIntoList(List *list, char *buf, int bufLen);
void readFileIntoBuffer(char *name, char **target, int *size);
List *appendMDType(List *list);
MDLineType getMDLineType(char *line);

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

  List *linesType = appendMDType(lines);
  FreeList(lines, false);
  printList(linesType);

  for (int i=0; i<linesType->size; ++i) {
    free(((MDLine_t *)linesType->elements[i])->data);
  }
  FreeList(linesType, true);

  return 0;
}

MDLineType getMDLineType(char *line) {
  MDLineType type = UNDEF;
  int len = strlen(line);
  switch (line[0]) {
  case '#':
    type = HEADER1;
    break;
  case '>':
    type = BLOCK_QOUTE;
    break;
  }
  return type;
}

List *appendMDType(List *list) {
  List *ans = CreateList(list->size);
  for (int i = 0; i < list->size; ++i) {
    MDLine_t *elem = malloc(sizeof(MDLine_t));
    elem->type = getMDLineType((char *)list->elements[i]);
    elem->data = (char *)list->elements[i];
    PushList(ans, elem);
  }
  return ans;
}

void printList(List *list) {
  for (int i = 0; i < list->size; ++i) {
    if (((MDLine_t *)list->elements[i])->type != UNDEF) {
      printf("Element: %s\nType: %s\n",
             ((MDLine_t *)list->elements[i])->data,
             MDElementTypeNames[((MDLine_t *)list->elements[i])->type]);
    }
  }
}

void readFileIntoBuffer(char *name, char **target, int *size) {
  if (*target != NULL) {
    perror("There maybe a memory leak because target is not NULL in "
           "readFileIntoBuffer()");
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
  fread(*target, 1, fileSize, file);
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
  for (int i = 0; i < bufLen; ++i) {
    if (buf[i] == '\n') {
      endIndex = i;
      int difference = endIndex - startIndex + 1;
      char *line = calloc(difference, sizeof(char));
      if (line == NULL) {
        perror(
            "Could not allocate space for line in readBufferLinesIntoList().");
        abort();
      }
      for (int j = startIndex; j < endIndex; ++j) {
        char tmp[2] = {buf[j], '\0'};
        strcat(line, tmp);
      }
      startIndex = endIndex + 1;
      PushList(list, line);
    }
  }
}
