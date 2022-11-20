/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack
#define _PARAM_ struct Param

#ifndef PARSER_CHECK
#define PARSER_CHECK

typedef enum {
    T_TYPE_STRING_DATATYPE = 1, // Datatypes
    T_TYPE_FLOAT_DATATYPE,
    T_TYPE_NULL_DATATYPE,
    T_TYPE_INT_DATATYPE,
    
    T_TYPE_KEYWORD,
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

    T_TYPE_CLOSED_CURLY_BRACKET,    // Symbols
    T_TYPE_OPEN_CURLY_BRACKET,
    T_TYPE_CLOSED_BRACKET,
    T_TYPE_OPEN_BRACKET,
    T_TYPE_SEMICOLON,
    T_TYPE_COLON,
    T_TYPE_COMMA,
    
    T_TYPE_EOF,
    
    T_TYPE_NULL
} Token_Type;

typedef enum{
    T_KEYWORD_FUNCTION = 1,    // Built-In Functions
    T_KEYWORD_RETURN,
    T_KEYWORD_WHILE,
    T_KEYWORD_ELSE,
    T_KEYWORD_IF,

    T_KEYWORD_SUBSTRING,   // Generator Functions
    T_KEYWORD_FLOATVAL,
    T_KEYWORD_INTVAL,
    T_KEYWORD_STRVAL,
    T_KEYWORD_STRLEN,
    T_KEYWORD_WRITE,
    T_KEYWORD_READS,
    T_KEYWORD_READI,
    T_KEYWORD_READF,
    T_KEYWORD_ORD,
    T_KEYWORD_CHR,

    T_KEYWORD_STRING,  // Declarations
    T_KEYWORD_FLOAT,
    T_KEYWORD_VOID,
    T_KEYWORD_NULL,
    T_KEYWORD_INT,
} Token_Keyword;

typedef union {
    int Integer;
    char *String;
    float Float;
} Token_Value;

struct Token {
    Token_Type Type;
    Token_Keyword Keyword;
    Token_Value Value;
};

struct Stack {
    struct Token Token;
    struct Stack *Previous;
};

struct Param {
    Token_Type Type;
    char *Name;
    struct Param *Next;
};

_TOKEN_ *T_Create();
_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, Token_Value Value, Token_Keyword Keyword);

int Term(Token_Type Type);

int Keyword(_TOKEN_ *Token, FILE* Source, int *Character, int Dive);
int F_Declare(_TOKEN_ *Token, FILE* Source, int *Character);
int Function(_TOKEN_ *Token, FILE* Source, int *Character, int Dive);
int Variable(_TOKEN_ *Token, FILE* Source, int *Character);
int Start(_TOKEN_ *Token, FILE* Source, int *Character, int Dive);

#endif  