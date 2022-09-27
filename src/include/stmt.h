#ifndef __STMT_H_
#define __STMT_H_

#include "config.h"
#include "shell.h"

typedef struct {
  StatementType type;
} Statement;

PrepareResult prepare_stmt(InputBuffer *, Statement *);
void execute_stmt(Statement *);
#endif