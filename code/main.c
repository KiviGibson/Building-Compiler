#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "stdio.h"
int main(int argc, const char* argv[]){
  Chunk chunk;
  initChunk(&chunk);
  int constant = addConstant(&chunk, 2.5);  
  writeChunk(&chunk, OP_CONSTANT, 15);
  writeChunk(&chunk, constant, 15);
  writeChunk(&chunk, OP_RETURN, 15);
  disassembleChunk(&chunk, "test_chunk");
  freeChunk(&chunk);

  return 0;
}

