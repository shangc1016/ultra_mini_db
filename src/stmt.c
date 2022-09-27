#include "include/stmt.h"

#include <stdio.h>
#include <string.h>

PrepareResult prepare_stmt(InputBuffer *input_buffer, Statement *statement) {
  if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STMT_SELECT;
    return PREPARE_SUCCESS;
  } else if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STMT_INSERT;
    return PREPARE_SUCCESS;
  } else {
    statement->type = STMT_UNRECOGNIZED;
    return PREPARE_UNRECOGNIZED_COMMAND;
  }
}

void execute_stmt(Statement *statement) {
  switch (statement->type) {
    case STMT_INSERT:
      fprintf(stdout, "statement insert.\n");
      break;
    case STMT_SELECT:
      fprintf(stdout, "statement select.\n");
      break;
    default:
      fprintf(stdout, "Unrecognized stmt.\n");
  }
}