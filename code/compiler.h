#ifndef clux_compiler_h
#define clux_compiler_h

#include "vm.h"
#include "object.h"

bool compile(const char* source, Chunk* chunk);

#endif // !clux_compiler_h
