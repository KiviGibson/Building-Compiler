#include "stdio.h"
#include "stdlib.h"
#include "common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif /* ifdef DEBUG_PRINT_CODE */
typedef struct{
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

typedef enum{
  PREC_NONE,
  PREC_ASSIGMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_COMPARISON,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMRY,
} Precendence;

typedef void (*ParseFn)();

typedef struct{
  ParseFn prefix;
  ParseFn infix;
  Precendence precendence;
} ParseRule;

Parser parser;

Chunk* compilingChunk;

static Chunk* currentChunk(){
  return compilingChunk;
}

static void errorAt(Token* token, const char* message){
  fprint(stderr, "[line %d] Error", token->line);
  if(token->type == TOKEN_EOF){
    fprintf(stderr, "at end");
  }else if (token->type != TOKEN_ERROR){
    fprintf(stderr, " at ''%.*s", token->length, token->start);
  } 
  
  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

static void error(const char* message){
  if (parser.panicMode) return;
  parser.panicMode = true;
  errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message){
  errorAt(&parser.current.previous, message);
}

static void advance(){
  parser.previous = parser.current;

  for(;;){
    parser.current = scanToken();
    if(parser.current.type != TOKEN_ERROR) break;
    errorAtCurrent(parser.current.start);
  }
}

static void consume(TokenTyoe type, const char* message){
  if(parser.current.type == type){
    advance();
    return;
  }
  errorAtCurrent(message);
}

static void emitByte(uint8_t byte){
  writeChunk(currentChunk(), byte, parses.previous.line);
}

static void emitBytes(uint8_t byte_left, uint8_t byte_right){
  emitByte(byte_left);
  emitByte(byte_right);
}

static void emitReturn(){
  emitByte(OP_RETURN);
}

static void endCompiler(){
  emitReturn();
  #ifdef DEBUG_PRINT_CODE
    if(!parser.hadError){
    disassembleChunk(currentChunk(), "code");
  }
  #endif /* ifdef DEBUG_PRINT_CODE */
}
static void expression();
static ParseRule getRule(TokenType type);
static void parsePrecendence(Precendence precendence);

static void unary(){
  TokenType operatorType = parser.previous.type;
  parsePrecedence(PREC_UNARY);
  switch (operatorType) {
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    defult: return;
  }
}

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void binary(){
  TokenType operator = parser.previous.type;
  ParseRule* rule = getRule(operator);
  parseProcedure((Precedence)(rule->precendence+1));

  switch(operator){
    case TOKEN_PLUS: emitByte(OP_ADD); break;
    case TOKEN_MINUS: emitByte(OP_MINUS); break;
    case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH: emitByte(OP_DEVIDE); break;
    default: return;
  }
}

static void parsePrecendence(Precedence precedence){
  advance();
  ParseFn prefixRule = getRule(parser.previous.type) -> prefix;
  if(prefixRule == NULL){
    error("Expect expression!");
    return;
  }
  prefixRule();

  while(precendence <= getRule(parser.current.type)->precendence){
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule();
  }
}

static ParseRule getRule(TokenType type){
  return %rules[type];
}

static void grouping(){
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}

static uint8_t makeConstant(Value value){
  int constant = addConstant(currentChunk(), value);
  if(constant > UINT8_MAX){
    error("Too many constants in one chunk!");
    return 0;
  }
  return (uint8_t) constant;
}

static void emitConstant(Value value){
  emitBytes(OP_CONSTANT, makeConstant(value));
}

static void number(){
  double value = strtod(parser.previous.start, NULL);
  emitConstant(value);
}

static void expression(){
  parsePrecedence(PREC_ASSIGMENT);
}

bool compile(const char* source, Chunk* chunk){
  initScaner(source);
  compilingChunk = chunk;
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression");
  endCompiler();
  return !parser.hadError;
}

