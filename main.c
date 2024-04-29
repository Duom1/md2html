#include "list.h"
#include <ctype.h>
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
  BLOCK_QUOTE,
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
  "block quote",
  "list",
  "unordered list",
  "code block",
  "ruler",
  "image",
  "paragraph"
};
char MDtoHTML[][40] = {
  "%s\n",                           // for elements that contain html
  "<h1>%s</h1>\n",
  "<h2>%s</h2>\n",
  "<h3>%s</h3>\n",
  "<h4>%s</h4>\n",
  "<h5>%s</h5>\n",
  "<h6>%s</h6>\n",
  "\n<blockquote>%s</blockquote>\n",
  "<li>%s</li>\n",
  "<li>%s</li>\n",
  "<code>%s</code>\n",              // code block needs to be done
  "<hr>\n",
  "<img src=\"%s\" alt=\"%s\" width=\"500\">\n",
  "<p>%s</p>\n"
};
// clang-format on
#define MAX_MULTILINE_ELEM_SE 7
char ulistStart[MAX_MULTILINE_ELEM_SE] = "<ul>\n";
char ulistEnd[MAX_MULTILINE_ELEM_SE] = "</ul>\n";
char listStart[MAX_MULTILINE_ELEM_SE] = "<ol>\n";
char listEnd[MAX_MULTILINE_ELEM_SE] = "</ol>\n";

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
  int len = strlen(str);
  if (toTrim >= len) {
    perror("trying to trim too much");
    abort();
  }
  if (front) {
    for (int i = 0; i <= len - toTrim; ++i) {
      str[i] = str[i + toTrim];
    }
  } else {
    str[len - toTrim] = '\0';
  }
}

List *createHtmlFromMDList(List *list) {
  MDLine_t *emptyExtra = malloc(sizeof(MDLine_t));
  emptyExtra->type = UNDEF;
  emptyExtra->data = malloc(sizeof(char));
  *(char *)emptyExtra->data = '\0';
  PushList(list, emptyExtra);

  List *ans = CreateList(list->size);
  bool multiLineElement = false;
  MDLineType multiLineType = UNDEF;
  for (int i = 0; i < list->size; ++i) {
    char *line = ((MDLine_t *)list->elements[i])->data;
    if (strcmp(line, "") == 0 && i != list->size - 1) {
      continue;
    }
    MDLineType type = ((MDLine_t *)list->elements[i])->type;
    if (multiLineElement && type != multiLineType) {
      char *endMultiline = calloc(MAX_MULTILINE_ELEM_SE, sizeof(char));
      if (endMultiline == NULL) {
        perror("could not allocate space for endMultiline in "
               "createHtmlFromMDList()");
        abort();
      }
      switch (multiLineType) {
      case ULIST:
        strcat(endMultiline, ulistEnd);
        break;
      case LIST:
        strcat(endMultiline, listEnd);
        break;
      }
      PushList(ans, endMultiline);
      multiLineElement = false;
      if (i == list->size - 1) {
        continue;
      }
    }
    int lineLen = strlen(line);
    int htmlSize = strlen(MDtoHTML[type]) + lineLen + 32;
    char *html = malloc(sizeof(char) * htmlSize);
    if (html == NULL) {
      perror("could not allocate space for html in createHtmlFromMDList()");
      abort();
    }
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
    case BLOCK_QUOTE:
      trimChars(line, 2, true);
      sprintf(html, MDtoHTML[type], line);
      break;
    case RULER:
      sprintf(html, MDtoHTML[type], NULL);
      break;
    case ULIST:
      if (!multiLineElement) {
        char *startUList = calloc(MAX_MULTILINE_ELEM_SE, sizeof(char));
        if (startUList == NULL) {
          perror("failed to allocate momory for startUList in "
                 "createHtmlFromMDList()");
          abort();
        }
        strcat(startUList, ulistStart);
        PushList(ans, startUList);
        multiLineElement = true;
      }
      trimChars(line, 2, true);
      multiLineType = type;
      sprintf(html, MDtoHTML[type], line);
      break;
    case LIST:
      if (!multiLineElement) {
        char *startList = calloc(MAX_MULTILINE_ELEM_SE, sizeof(char));
        if (startList == NULL) {
          perror("failed to allocate momory for startUList in "
                 "createHtmlFromMDList()");
          abort();
        }
        strcat(startList, listStart);
        PushList(ans, startList);
        multiLineElement = true;
      }
      trimChars(line, 3, true);
      multiLineType = type;
      sprintf(html, MDtoHTML[type], line);
      break;
    case IMAGE:
      trimChars(line, 2, true);
      trimChars(line, 1, false);
      int imgTextStart = 0;
      int imgTextEnd = 0;
      for (int j = 0; j < lineLen; ++j) {
        if (line[j] == ']') {
          imgTextEnd = j;
        }
      }
      char *altText = calloc(imgTextEnd, sizeof(char));
      for (int j = imgTextStart; j < imgTextEnd; ++j) {
        char imgTmp[2] = {line[j], '\0'};
        strcat(altText, imgTmp);
      }
      imgTextStart = imgTextEnd + 2;
      imgTextEnd = lineLen - 1;
      char *fileText = calloc(imgTextEnd - imgTextStart, sizeof(char));
      for (int j = imgTextStart; j < imgTextEnd; ++j) {
        char imgTmp[2] = {line[j], '\0'};
        strcat(fileText, imgTmp);
      }
      sprintf(html, MDtoHTML[type], fileText, altText);
      break;
    default:
      sprintf(html, MDtoHTML[PARAG], line);
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
    type = BLOCK_QUOTE;
  } else if (firstSix[0] == '-') {
    type = ULIST;
    if (firstSix[1] == '-' && firstSix[2] == '-') {
      type = RULER;
    }
  } else if (firstSix[0] == '!') {
    type = IMAGE;
  } else if (firstSix[0] == '`' && firstSix[1] == '`' && firstSix[2] == '`') {
    type = CODE_BLOCK;
  } else if (isdigit(firstSix[0]) && firstSix[1] == '.') {
    type = LIST;
  }
  return type;
}

List *appendMDType(List *list) {
  List *ans = CreateList(list->size);
  for (int i = 0; i < list->size; ++i) {
    MDLine_t *elem = malloc(sizeof(MDLine_t));
    if (elem == NULL) {
      perror("could not allocate space for elem in appendMDType()");
      abort();
    }
    elem->type = getMDLineType((char *)list->elements[i]);
    elem->data = (char *)list->elements[i];
    PushList(ans, elem);
  }
  return ans;
}

void printList(List *list) {
  for (int i = 0; i < list->size; ++i) {
    printf("%s", (char *)list->elements[i]);
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

  *target = (char *)malloc(sizeof(char) * (fileSize + 1));
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
