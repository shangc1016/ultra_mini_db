#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/meta_command.h"
#include "include/shell.h"
#include "include/stmt.h"

#define loop for (;;)

int main(int argc, char *argv[]) {
  InputBuffer *input_buffer = new_input_buffer();
  loop {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {
      // meta command
      execute_meta_command(input_buffer);
    } else {
      // sql command
      Statement statement;
      prepare_stmt(input_buffer, &statement);
      execute_stmt(&statement);
    }
  }
  return 0;
}