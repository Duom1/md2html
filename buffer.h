#ifndef INCLUDE_MD2HTML_BUFFER_H_
#define INCLUDE_MD2HTML_BUFFER_H_

typedef struct {
  int len;
  char *data;
  int used;
} Buffer;

enum Buf_slice_type {
  HEADER1 = 1,
  HEADER2,
  HEADER3,
  HEADER4,
  HEADER5,
  HEADER6,
  PARAG,
  BLOCK_QOUTE,
  IMG
};

typedef struct {
  int start;
  int end;
  int type;
} Buf_slice;

#endif  // INCLUDE_MD2HTML_BUFFER_H_
