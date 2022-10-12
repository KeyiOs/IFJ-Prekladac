/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token

#ifndef PARSER_CHECK
#define PARSER_CHECK

typedef enum {
    T_TYPE_PROLOG = 1,  // Prolog
    T_TYPE_PROLOG2,
    T_TYPE_PROLOG_END,

    T_TYPE_FUNCTION_KEYWORD,    // Built-In Functions
    T_TYPE_RETURN_KEYWORD,
    T_TYPE_WHILE_KEYWORD,
    T_TYPE_VOID_KEYWORD,
    T_TYPE_ELSE_KEYWORD,
    T_TYPE_IF_KEYWORD,

    T_TYPE_STRING_KEYWORD,  // Declarations
    T_TYPE_FLOAT_KEYWORD,
    T_TYPE_NULL_KEYWORD,
    T_TYPE_INT_KEYWORD,

    T_TYPE_STRING_DATATYPE, // Datatypes
    T_TYPE_FLOAT_DATATYPE,
    T_TYPE_NULL_DATATYPE,
    T_TYPE_INT_DATATYPE,
    
    T_TYPE_FUNCTION,
    T_TYPE_VARIABLE,

    T_TYPE_TRIPLE_EQUALS_NEG,   // Operational Symbols
    T_TYPE_MULTIPLICATION,
    T_TYPE_TRIPLE_EQUALS,
    T_TYPE_CONCATENATION,
    T_TYPE_SMALLER_EQUAL,
    T_TYPE_GREATER_EQUAL,
    T_TYPE_DIVISION,
    T_TYPE_SMALLER,
    T_TYPE_GREATER,
    T_TYPE_EQUAL,
    T_TYPE_MINUS,
    T_TYPE_PLUS,

    T_TYPE_SUBSTRING,   // Generator Functions
    T_TYPE_FLOATVAL,
    T_TYPE_INTVAL,
    T_TYPE_STRVAL,
    T_TYPE_STRLEN,
    T_TYPE_WRITE,
    T_TYPE_READS,
    T_TYPE_READI,
    T_TYPE_READF,
    T_TYPE_ORD,
    T_TYPE_CHR,

    T_TYPE_CLOSED_CURLY_BRACKET,    // Symbols
    T_TYPE_OPEN_CURLY_BRACKET,
    T_TYPE_CLOSED_BRACKET,
    T_TYPE_OPEN_BRACKET,
    T_TYPE_SEMICOLON,
    T_TYPE_COLON,
    T_TYPE_COMMA,
    
    T_TYPE_EOL,
    T_TYPE_EOF,
    
    T_TYPE_NULL,    // Empty Token

    T_TYPE_QUOTATION_MARK,  // Not Specified
    T_TYPE_QUESTIONMARK,
    T_TYPE_DOLAR
} Token_Type;

typedef union {
    int Integer;
    char *String;
    float Float;
} Token_Value;

struct Token {
    Token_Type Type;
    Token_Value Value;
    _TOKEN_ *Preceding;
    _TOKEN_ *Following;
};

_TOKEN_ *T_Create();
void T_Free(_TOKEN_ *Token);

#endif  