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
  PARAG,
};

// clang-format off
char MDElementTypeNames[][15] = {
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
  "code block",
  "ruler",
  "image",
  "paragraph"
};
char MDtoHTML[][30] = {
  "%s\n",
  "<h1>%s</h1>\n",
  "<h2>%s</h2>\n",
  "<h3>%s</h3>\n",
  "<h4>%s</h4>\n",
  "<h5>%s</h5>\n",
  "<h6>%s</h6>\n",
  "<blockqoute>%s</blockqoute>\n",
  "%s\n",
  "%s\n",
  "%s\n",
  "<hr>\n",
  "<img src=\"%s\">\n",
  "<p>%s</p>\n"
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
List *createHtmlFromMDList(List *list);
void trimChars(char *str, int toTrim, bool front);

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

  List *htmlList = createHtmlFromMDList(linesType);
  printList(htmlList);
  for (int i = 0; i < linesType->size; ++i) {
    free(((MDLine_t *)linesType->elements[i])->data);
  }
  FreeList(linesType, true);
  FreeList(htmlList, true);

  return 0;
}

void trimChars(char *str, int toTrim, bool front) {
  if (front) {
    int len = strlen(str);
    for (int i = 0; i <= len - toTrim; ++i) {
      str[i] = str[i + toTrim];
    }
  } else {
    perror("not trimming from the front");
    abort();
  }
}

List *createHtmlFromMDList(List *list) {
  List *ans = CreateList(list->size);
  for (int i = 0; i < list->size; ++i) {
    MDLineType type = ((MDLine_t *)list->elements[i])->type;
    char *line = ((MDLine_t *)list->elements[i])->data;
    int htmlSize = strlen(MDtoHTML[type]) + strlen(line);
    char *html = malloc(sizeof(char) * htmlSize);
    switch (type) {
    case HEADER1:
      trimChars(line, 2, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case HEADER2:
      trimChars(line, 3, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case HEADER3:
      trimChars(line, 4, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case HEADER4:
      trimChars(line, 5, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case HEADER5:
      trimChars(line, 6, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case HEADER6:
      trimChars(line, 7, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    default:
      sprintf(html, MDtoHTML[UNDEF], line);
      break;
    }
    PushList(ans, html);
  }
  return ans;
}

MDLineType getMDLineType(char *line) {
  MDLineType type = UNDEF;
  char firstSix[7];
  strncpy(firstSix, line, 6);
  firstSix[6] = '\0';
  if (firstSix[0] == '#') {
    type = HEADER1;
    if (firstSix[1] == '#') {
      type = HEADER2;
      if (firstSix[2] == '#') {
        type = HEADER3;
        if (firstSix[3] == '#') {
          type = HEADER4;
          if (firstSix[4] == '#') {
            type = HEADER5;
            if (firstSix[5] == '#') {
              type = HEADER6;
            }
          }
        }
      }
    }
  } else if (firstSix[0] == '>') {
    type = BLOCK_QOUTE;
  } else if (firstSix[0] == '-') {
    type = ULIST;
    if (firstSix[1] == '-' && firstSix[2] == '-') {
      type = RULER;
    }
  } else if (firstSix[0] == '!') {
    type = IMAGE;
  } else if (firstSix[0] == '`' && firstSix[1] == '`' && firstSix[2] == '`') {
    type = CODE_BLOCK;
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
    printf("%s\n", (char *)list->elements[i]);
  }
}

#define ADD_EXTRA_SPACE_TO_LINE 8
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

  *target =
      (char *)malloc(sizeof(char) * (fileSize + 1 + ADD_EXTRA_SPACE_TO_LINE));
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
