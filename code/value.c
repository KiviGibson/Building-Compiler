#include "stdio.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray* array){
  array->capacity = 0;
  array->count = 0;
  array->values = NULL;
}

void writeValueArray(ValueArray* array, Value value){
  if(array->capacity < array->count + 1){
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
  }
  array->values[array->count++] = value;
}

void freeValueArray(ValueArray* array){
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}
void printValue(Value value){
  switch(value.type){
    case BOOL_TYPE:
      printf(AS_BOOL(value) ? "True" : "False");
      break;
    case NIL_TYPE: printf("nil"); break;
    case NUMBER_TYPE: printf("%g", AS_NUMBER(value)); break; 
  }
 
}

bool valueEqual(Value a, Value b){
  if (a.type != b.type) return false;
  switch(a.type){
    case BOOL_TYPE: return AS_BOOL(a) == AS_BOOL(b);
    case NIL_TYPE: return true;
    case NUMBER_TYPE: return AS_NUMBER(a) == AS_NUMBER(b);
    default: return false;
  }
}

