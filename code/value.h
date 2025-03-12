#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum{
  BOOL_TYPE,
  NIL_TYPE,
  NUMBER_TYPE,
  OBJ_TYPE,
} ValueType;

typedef struct{
  ValueType type;
  union{
    bool boolean;
    double number;
    Obj* obj;
  } as;
} Value;

#define BOOL_VAL(value) ((Value){BOOL_TYPE, {.boolean = value}})
#define NIL_VAL(value) ((Value){NIL_TYPE, {.number = 0}})
#define NUMBER_VAL(value) ((Value){NUMBER_VAL,{.number = value}})
#define OBJ_VAL(object) ((Value){OBJ_TYPE,{.obj = (Obj*)object }})

#define AS_BOOL(value) (value).as.boolean
#define AS_NUMBER(value) (value).as.number
#define AS_OBJ(value) (value).as.obj

#define IS_NUMBER(value) ((value).type == NUMBER_TYPE)
#define IS_NIL(value) ((value).type == NIL_TYPE)
#define IS_BOOL(value) ((value).type == BOOL_TYPE)
#define IS_OBJ(value) ((value).type == OBJ_TYPE)

typedef struct{
  int capacity;
  int count;
  Value* values;
} ValueArray;

bool valueEquals(Value a, Value B);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

#endif // !clox_value_h

