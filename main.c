#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void printIntList(List *list);

int main(int argc, char **argv) {
  List *intList = CreateList(120);
  printf("capacity of the list = %i\n", intList->capcity);
  int test = 97;
  int cap = 23;
  for (int i = 0; i < cap; ++i) {
    int *elem = malloc(sizeof(int));
    *elem = test;
    test += 22;
    PushList(intList, elem);
  }
  printf("capacity of the list = %i\n", intList->capcity);
  printIntList(intList);
  puts("");
  printf("popped = %i\n", *(int *)PopList(intList));
  printf("capacity of the list = %i\n", intList->capcity);
  printIntList(intList);
  puts("");
  FreeList(intList);
  return EXIT_SUCCESS;
}

void printIntList(List *list) {
  for (int i = 0; i < list->size; ++i) {
    int c = *(int*)list->elements[i];
    printf("%i: %i, ", i, c);
  }
}
