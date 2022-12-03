/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */
#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack
#define _PARAM_ struct Param
#define _ITEMF_ struct ItemF
#define _ITEMV_ struct ItemV
#define _WRAP_ struct Wrap

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
    T_KEYWORD_NULL,
    T_KEYWORD_INT,
    T_KEYWORD_VOID,

    T_KEYWORD_EMPTY,
} Token_Keyword;

struct Token {
    Token_Type Type;
    Token_Keyword Keyword;
    char *String;
};

struct Stack {
    struct Token Token;
    struct Stack *Previous;
};

struct Param {
    Token_Keyword Type;
    char *Name;
    _PARAM_ *Next;
};

struct ItemV {
    char *Name;
    Token_Type Type;
    int Dive;
    int Init;
    _ITEMV_ *Left;
    _ITEMV_ *Right;
};

struct ItemF {
    char *Name;
    Token_Keyword Type;
    _PARAM_ *Params;
    _ITEMV_ *Local;
    _ITEMF_ *Root;
    _ITEMF_ *Left;
    _ITEMF_ *Right;
};

struct Wrap {
    char Character;
    _TOKEN_ *Token;
    _ITEMF_ *Table;
    _STACK_ *Stack;
    FILE* Source;
    int Dive;
};

_TOKEN_ *T_Create();
_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, char *String, Token_Keyword Keyword);

int Term(Token_Type Type);
void End(_WRAP_ *Wrap);

int Keyword(_WRAP_ *Wrap);
int F_Declare(_WRAP_ *Wrap);
int Function(_WRAP_ *Wrap);
int Variable(_WRAP_ *Wrap);
int Start(_WRAP_ *Wrap);

#endif