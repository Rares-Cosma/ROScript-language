#pragma once
#include <cstdint>

enum OpCode : uint8_t {
    OP_PUSH_INT,
    OP_PUSH_FLOAT,
    OP_PUSH_STRING,
    OP_PUSH_BOOL,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EE, // ==
    OP_NE, // !=
    OP_GT, // >
    OP_LT, // <
    OP_GE, // >=
    OP_LE, // <=
    OP_AND,
    OP_OR,
    OP_LOAD_VAR,
    OP_STORE_VAR,
    OP_JMP,
    OP_JMP_IF_FALSE,
    OP_CALL,
    OP_CALL_DEFAULT,
    OP_RET,
    OP_HALT,
    OP_TYPE_INT,
    OP_TYPE_FLOAT,
    OP_TYPE_STRING,
    OP_TYPE_BOOL,
};