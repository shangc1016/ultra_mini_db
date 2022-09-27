#ifndef __CONFIG_H_
#define __CONFIG_H_

// 元数据解析返回值
typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

// SQL语句准备返回值
typedef enum {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_COMMAND,
} PrepareResult;

// SQL语句类型判别
typedef enum {
  STMT_INSERT,
  STMT_SELECT,
  STMT_UNRECOGNIZED,
} StatementType;

#endif