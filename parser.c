/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "error_handler.h"
#include "skener.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/*
/  MinGW32-make
/  ./parser
/  Vstup
*/

/*
/ SHIFT-SHIFT-B
/ Input.txt
/ CTRL-H
*/

#include "stack.h"

int Line = 1;
int Token_Number = 1;

_TOKEN_ *T_Create(){
    _TOKEN_ *Token = malloc(sizeof(struct Token));
    if(!Token) return NULL;

    Token->Type = T_TYPE_NULL;
    
    return Token;
}

_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, Token_Value Value, Token_Keyword Keyword){
    Token->Type = Type;
    Token->Keyword = T_TYPE_NULL_KEYWORD;
    Token->Value.String = "\0";

    switch(Type){
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_VARIABLE:
        case T_TYPE_FUNCTION:
            int Length = strlen(Value.String);

            Token->Value.String = NULL;
            Token->Value.String = (char*) malloc(Length*sizeof(char));
            if(Token->Value.String == NULL) return NULL;
            for(int i=0; i<Length; i++) Token->Value.String[i] = '\0';

            strcpy(Token->Value.String,Value.String);
            break;
        case T_TYPE_KEYWORD:
            Token->Keyword = Keyword;
            break;
        case T_TYPE_FLOAT_DATATYPE:
            Token->Value.Float = Value.Float;
            break;
        case T_TYPE_INT_DATATYPE:
            Token->Value.Integer = Value.Integer;
            break;
        default:
            break;
    }

    return Token;
}

int Start(_TOKEN_ *Token, FILE* Source, int *Character){
    int ERR;
    _STACK_ *Stack = Stack_Create();
    for(int i = 0; i < 3; i++) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;
        Stack_Push(Stack, Token);
    }
    for(int i = 0; i < 3; i++) {
        Stack = Stack_Pop(Stack);
    }

    if(Token->Type == T_TYPE_KEYWORD){
        
    }

    return 0;
}

int main(){
    _TOKEN_ *Token = T_Create();                                        // Inicialization
    if(Token == NULL) return 99;                                        // 
    int Character;                                                      //
    
    FILE* Source;                                                       // Input
    if(!(Source = fopen("Input.txt", "r"))) Source = stdin;             //

    int ERR = Prolog(Source, &Character);                               // Prolog
    if(ERR != 0) return ERR;                                            //

    if((ERR = Start(Token, Source, &Character)) != 0) {                 // Analysis Start
        ERR_Handler(ERR, Line, Token_Number);                           //
        return ERR;                                                     //
    }                                                                   //

    free(Token);                                                        // Memory Deallocation
    Token = NULL;                                                       //

    return 0;                                                           // End
}