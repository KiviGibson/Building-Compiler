#ifndef clux_chunk_h
#define clux_chunk_h

#include "common.h"
#include "value.h"

typedef enum{
  OP_CONSTANT,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DEVIDE,
  OP_RETURN,
} OpCode;

typedef struct{
  int count;
  int capacity;
  int* lines;
  ValueArray constants;  
  uint8_t* code;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
#endif
