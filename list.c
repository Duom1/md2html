#include "list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// function used to create a new list
List *CreateList(int cap) {
  List *list = malloc(sizeof(List));
  if (list == NULL) {
    fprintf(stderr, "Unable to create list in CreateList().\n");
    abort();
  }
  list->elements = malloc(sizeof(void *) * cap);
  if (list == NULL) {
    fprintf(stderr,
            "Unable to create space of %i for elements in CreateList().\n",
            cap);
    abort();
  }
  list->capcity = cap;
  list->size = 0;
  return list;
}

// this function is used to free the momory that the list uses
void FreeList(List *list, bool freeElem) {
  if (freeElem) {
    for (int i = 0; i < list->size; ++i) {
      // If this element has allocated momory other that it self YOU are
      // reponsible for freeing it!
      free(list->elements[i]);
    }
  }
  free(list->elements);
  free(list);
}

// push an itemn to the top of the list
void PushList(List *list, void *ap) {
  if (list->size > list->capcity) {
    ExplandList(list, AUTO_EXP); // AUTO_EXP is set in the list.h file
  }
  list->elements[list->size] = ap;
  ++list->size;
}

// pop an item from the top of the list
void *PopList(List *list) {
  void *ans = list->elements[list->size - 1];
  --list->size;
  TruncCheck(list, AUTO_TRUNC, AUTO_EXP);
  return ans;
}

// explands the list by the amount given
void ExplandList(List *list, int expBy) {
  list->elements =
      realloc(list->elements, sizeof(void *) * (list->capcity + expBy));
  if (list == NULL) {
    fprintf(stderr, "Unable to add space to elements by %i in ExplandList().\n",
            expBy);
    abort();
  }
  list->capcity += expBy;
}

// truncates the list to given amount
void TruncList(List *list, int truncTo) {
  list->elements = realloc(list->elements, sizeof(void *) * truncTo);
  if (list == NULL) {
    fprintf(stderr, "Unable to truncate elements to %i in TruncList().\n",
            truncTo);
    abort();
  }
  list->capcity = truncTo;
  if (list->size > list->capcity) {
    list->size = list->capcity + 1;
  }
}

// This function trncates the list if the size diffrence between the list size
// and capcity is too big.
void TruncCheck(List *list, int dif, int truncToPlus) {
  if (abs(list->capcity - list->size) >= dif) {
    TruncList(list, list->size + truncToPlus);
  }
}
