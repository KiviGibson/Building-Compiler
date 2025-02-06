#ifndef clux_vm_h
#define clux_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct{
  Chunk* chunk;
  uint8_t* ip;
  Value stack[STACK_MAX];
  Value* stackTop;
} VM;

typedef enum{
  INTERPRET_OK,
  INTERPRET_COMPILER_ERR,
  INTERPRET_RUNTIME_ERR,
} InterpretResult;

void initVM();
void freeVM();
void push(Value value);
Value pop();
InterpretResult interpret(const char* source);

#endif // !clux_vm_h

