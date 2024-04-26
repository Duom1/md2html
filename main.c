#include <stdio.h>
#include <stdlib.h>

#define ARGMAX 2
#define ARGMIN 2
#define EXIT_INV_ARG 1
#define EXIT_FILE_ERR 2

int main(int argc, char **argv) {
  if (argc > ARGMAX || argc < ARGMIN) {
    fprintf(stderr, "%s: incorrect amount of arguments\n", argv[0]);
    return EXIT_INV_ARG;
  }

  FILE *file_p = fopen(argv[1], "r");

  if (file_p == NULL) {
    fprintf(stderr, "could not open file\n");
    return EXIT_FILE_ERR;
  }

  fseek(file_p, 0, SEEK_END);
  int file_s = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);

  char *file_b = malloc(file_s + 1);

  if (file_b == NULL) {
    fprintf(stderr, "could not allocate space for file\n");
    fclose(file_p);
    return EXIT_FILE_ERR;
  }

  fread(file_b, 1, file_s, file_p);
  file_b[file_s] = '\0';

  printf("%s", file_b);

  free(file_b);
  return EXIT_SUCCESS;
}
