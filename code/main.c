#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "stdio.h"
#include "vm.h"

int main(int argc, const char* argv[]){
  initVM();  
  Chunk chunk;
  initChunk(&chunk);
  
  int constant = addConstant(&chunk, 2.5);  
  writeChunk(&chunk, OP_CONSTANT, 15); // add 2.5 to stack
  writeChunk(&chunk, constant, 15);
  
  constant = addConstant(&chunk, 3.5);
  writeChunk(&chunk, OP_CONSTANT, 15); // add 3.5 to stack
  writeChunk(&chunk, constant, 15);

  writeChunk(&chunk, OP_ADD, 15); // replace with 2.5 + 3.5 on stack

  constant = addConstant(&chunk, 2.0);
  writeChunk(&chunk, OP_CONSTANT, 15); // add 2.0 to stack
  writeChunk(&chunk, constant, 15);

  writeChunk(&chunk, OP_DEVIDE, 15); // repleace (2.5 + 3.5) / 2.0 on stack
  writeChunk(&chunk, OP_NEGATE, 15); // repleace -((2.5 + 3.5) / 2.0) on stack
  
  writeChunk(&chunk, OP_RETURN, 15); //return -((2.5 + 3.5) / 2.0)
  disassembleChunk(&chunk, "test_chunk");
  interpret(&chunk);
  freeVM();
  freeChunk(&chunk);

  return 0;
}

