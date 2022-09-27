#ifndef __META_COMMAND_H_
#define __META_COMMAND_H_

#include "config.h"
#include "shell.h"

MetaCommandResult do_meta_command(InputBuffer *);

void execute_meta_command(InputBuffer *);

#endif