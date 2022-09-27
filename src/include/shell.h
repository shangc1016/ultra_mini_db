#ifndef __SHELL_H_
#define __SHELL_H_

#include <sys/types.h>

typedef struct {
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

void print_prompt();

InputBuffer *new_input_buffer();

void read_input(InputBuffer *);

#endif