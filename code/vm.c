#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "stdio.h"
#include "debug.h"

VM vm;

static void resetStack(){
  vm.stackTop = vm.stack;
}

void push(Value value){
  *(vm.stackTop++) = value;
}

Value pop(){
  return *(--vm.stackTop);
}

void initVM(){
  resetStack();
}

void freeVM(){
}

static InterpretResult run(){
  #define READ_BYTE() (*vm.ip++)
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()]) 
  #define BINARY_OP(op) \
    do{ \
    double a = pop(); \
    double b = pop(); \
    push(b op a); \
    }while(false)

  for(;;){
    #ifdef DEBUG_TRACE_EXECUTION
      printf("---> ");
      for(Value* slot = vm.stack; slot <vm.stackTop; slot++){
        printf("[");
        printValue(*slot);
        printf("]");
      }
      printf("\n");
      disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
    #endif
    uint8_t instruction;
    switch(instruction = READ_BYTE()){
      case OP_CONSTANT:
        Value constant = READ_CONSTANT();
        push(constant);
      break;
      case OP_NEGATE: push(-pop()); break;
      case OP_ADD: BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DEVIDE: BINARY_OP(/); break;
      case OP_RETURN:
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
    }
  }
  #undef READ_BYTE 
  #undef READ_CONSTANT
  #undef BINARY_OP
}

InterpretResult interpret(const char* source){
  Chunk chunk;
  initChunk(&chunk);

  if(!compile(source, &chunk)){
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERR;
  }
  
  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}


