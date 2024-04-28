#ifndef INCLUDE_MD2HTML_LIST_H_
#define INCLUDE_MD2HTML_LIST_H_

#include <stdbool.h>

#define AUTO_EXP 10
#define AUTO_TRUNC 20

typedef struct {
  int capcity;      // how much space is available
  int size;         // is the index for the first available spot
  void **elements;   // this is the list it self
} List;

List *CreateList(int cap);
void PushList(List *list, void *ap);
void *PopList(List *list);
void FreeList(List *list, bool freeElem);
void ExplandList(List *list, int expBy);
void TruncList(List *list, int truncBy);
void TruncCheck(List *list, int dif, int truncToPlus);

#endif  // INCLUDE_MD2HTML_LIST_H_
