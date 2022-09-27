#include "include/meta_command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/config.h"

MetaCommandResult do_meta_command(InputBuffer *input_buffer) {
  if (strncmp(input_buffer->buffer, ".exit", 5) == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

void execute_meta_command(InputBuffer *input_buffer) {
  switch (do_meta_command(input_buffer)) {
    case META_COMMAND_SUCCESS:
      break;
    case META_COMMAND_UNRECOGNIZED_COMMAND:
      fprintf(stdout, "Unrecognized meta command '%s'\n", input_buffer->buffer);
  }
}