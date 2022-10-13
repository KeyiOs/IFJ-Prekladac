/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "error_handler.h"
#include "skener.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, Token_Value Value){
    _TOKEN_ *Token_New = T_Create();
    if(Token_New == NULL) return NULL;

    Token_New->Preceding = Token;
    Token->Type = Type;
    Token->Following = Token_New;

    switch(Type){
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_VARIABLE:
        case T_TYPE_FUNCTION:
            int Length = strlen(Value.String);

            Token->Value.String = NULL;
            Token->Value.String = (char*) malloc(Length*sizeof(char));
            for(int i=0; i<Length; i++) Token->Value.String[i] = '\0';

            strcpy(Token->Value.String,Value.String);
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

    return Token->Following;
}

/*
/   String - Pridat escape sekvencie
/   Funkcia na stringy -> ($identifikator, "String", funkcia)
/   Optimalizovat
*/

int Scan(_TOKEN_ **Token){
    Token_Value Value;
    int Length = 0;

    int Character = getchar();

    while(1){
        switch(Character){
        case 9:     // Tabulator
        case 11:    //
        case ' ':   //
            Character = getchar();
            break;
        case 10:    // New Line
        case 13:    //
            *Token = T_Assign(*Token, T_TYPE_EOL, Value);
            Character = getchar();
            break;
        case '"':   // String
            char *String1 = (char*) malloc(5*sizeof(char));
            if(String1 == NULL) return 99;

            Length = 0;
            for(int i=0; i<5; i++) String1[Length+i] = '\0';

            Character = getchar();
            while(Character != '"'){
                if(Length%5 == 4) {
                    String1 = (char*) realloc(String1, Length+5);
                    if(String1 == NULL) return 99;
                    for(int i=0; i<5; i++) String1[Length+i] = '\0';
                }
                String1[Length] = Character;
                Length++;
                Character = getchar();
            }

            Value.String = NULL;
            Value.String = (char*) malloc(Length*sizeof(char));
            for(int i=0; i<Length; i++) Value.String[i] = '\0';

            strcpy(Value.String,String1);
            free(String1);

            *Token = T_Assign(*Token, T_TYPE_STRING_DATATYPE, Value);
            free(Value.String);

            Character = getchar();
            break;
        case '$':   // Variable Identificator
            char *String2 = (char*) malloc(5*sizeof(char));
            if(String2 == NULL) return 99;

            Length = 0;
            for(int i=0; i<5; i++) String2[Length+i] = '\0';

            Character = getchar();
            if((65 > Character || Character > 90) && (97 > Character || Character > 122) && Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Including invalid first symbol
            while(Character != ' ' && Character != EOF && Character != 10){
                if((48 > Character || Character > 57) && (65 > Character || Character > 90) && (97 > Character || Character > 122) && Character != '_') return 1;   // Added numbers as viable symbols
                if(Length%5 == 4) {
                    String2 = (char*) realloc(String2, Length+5);
                    if(String2 == NULL) return 99;
                    for(int i=0; i<5; i++) String2[Length+i] = '\0';
                }
                String2[Length] = Character;
                Length++;
                Character = getchar();
            }

            Value.String = NULL;
            Value.String = (char*) malloc(Length*sizeof(char));
            for(int i=0; i<Length; i++) Value.String[i] = '\0';

            strcpy(Value.String,String2);
            free(String2);

            *Token = T_Assign(*Token, T_TYPE_VARIABLE, Value);
            free(Value.String);

            Character = getchar();
            break;
        case '(':
            *Token = T_Assign(*Token, T_TYPE_OPEN_BRACKET, Value);
            Character = getchar();
            break;
        case ')':
            *Token = T_Assign(*Token, T_TYPE_CLOSED_BRACKET, Value);
            Character = getchar();
            break;
        case '*':
            *Token = T_Assign(*Token, T_TYPE_MULTIPLICATION, Value);
            Character = getchar();
            break;
        case '+':
            *Token = T_Assign(*Token, T_TYPE_PLUS, Value);
            Character = getchar();
            break;
        case '-':
            *Token = T_Assign(*Token, T_TYPE_MINUS, Value);
            Character = getchar();
            break;
        case '.':
            *Token = T_Assign(*Token, T_TYPE_CONCATENATION, Value);
            Character = getchar();
            break;
        case '/':
            Character = getchar();
            if(Character == '/'){   // Line Comment
                while(Character != EOF && Character != 10 && Character != 13) Character = getchar();
                break;
            }
            else if(Character == '*') { // Multiline Comment
                while(Character != EOF) {
                    Character = getchar();
                    if(Character == '*'){
                        Character = getchar();
                        if(Character == '/') break;
                    }
                }
                break;
            }
            *Token = T_Assign(*Token, T_TYPE_DIVISION, Value);
            break;
        case ':':
            *Token = T_Assign(*Token, T_TYPE_COLON, Value);
            Character = getchar();
            break;
        case ';':
            *Token = T_Assign(*Token, T_TYPE_SEMICOLON, Value);
            Character = getchar();
            break;
        case '<':
            Character = getchar();
            if(Character == '='){
                *Token = T_Assign(*Token, T_TYPE_SMALLER_EQUAL, Value);
                Character = getchar();
                break;
            }
            *Token = T_Assign(*Token, T_TYPE_SMALLER, Value);
            Character = getchar();
            break;
        case '=':
            Character = getchar();
            if(Character == '='){
                Character = getchar();
                if(Character != '=') return 1;
                *Token = T_Assign(*Token, T_TYPE_TRIPLE_EQUALS, Value);;
                Character = getchar();
                break;
            }
            *Token = T_Assign(*Token, T_TYPE_EQUAL, Value);
            break;
        case '>':
            Character = getchar();
            if(Character == '='){
                *Token = T_Assign(*Token, T_TYPE_GREATER_EQUAL, Value);
                Character = getchar();
                break;
            }
            *Token = T_Assign(*Token, T_TYPE_GREATER, Value);
            Character = getchar();
            break;
        case '?':   // Datatype Identificator
            char String3[7];
            for(int i=0; i<7; i++) String3[i] = '\0';

            Length = 0;
            Character = getchar();
            while('a' <= Character && Character <= 'z'){
                if(Length == 6) return 1;
                String3[Length] = Character;
                Length++;
                Character = getchar();
            }
            if(!strcmp(String3,"float")) *Token = T_Assign(*Token, T_TYPE_FLOAT_KEYWORD, Value);
            else if(!strcmp(String3,"int")) *Token = T_Assign(*Token, T_TYPE_INT_KEYWORD, Value);
            else if(!strcmp(String3,"string")) *Token = T_Assign(*Token, T_TYPE_STRING_KEYWORD, Value);
            else return 1;
            break;
        case '{':
            *Token = T_Assign(*Token, T_TYPE_OPEN_CURLY_BRACKET, Value);
            Character = getchar();
            break;
        case '}':
            *Token = T_Assign(*Token, T_TYPE_CLOSED_CURLY_BRACKET, Value);
            Character = getchar();
            break;
        case '0' ... '9':   // Integer or Float Values
            int Float = 0;
            float Number = 0.0;
            Length = 0;
            while(Character != ' ' && Character != 10 && Character != EOF){
                if((48 > Character || Character > 57) && Character != '.') return 1;
                if(Float == 1) Length ++;
                if(Character == '.'){
                    if(Float == 0) Float = 1;
                    else return 1;
                }
                else Number = Number * 10 + Character - 48;
                Character = getchar();
            }
            if(Float == 0){
                Value.Integer = Number;
                *Token = T_Assign(*Token, T_TYPE_INT_DATATYPE, Value);
                break;
            }
            for(int a = 0; a < Length; a ++) Number = Number / 10;
            Value.Float = Number;
            *Token = T_Assign(*Token, T_TYPE_FLOAT_DATATYPE, Value);
            break;
        case 'a' ... 'z':
        case 'A' ... 'Z':
            char *String4 = (char*) malloc(5*sizeof(char));
            if(String4 == NULL) return 99;

            Length = 0;
            for(int i=0; i<5; i++) String4[Length+i] = '\0';

            if((65 > Character || Character > 90) && (97 > Character || Character > 122) && Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Including invalid first symbol
            while(Character != ' ' && Character != EOF && Character != 10){
                if((48 > Character || Character > 57) && (65 > Character || Character > 90) && (97 > Character || Character > 122) && Character != '_') return 1;   // Added numbers as viable symbols
                if(Length%5 == 4) {
                    String4 = (char*) realloc(String4, Length+5);
                    if(String4 == NULL) return 99;
                    for(int i=0; i<5; i++) String4[Length+i] = '\0';
                }
                String4[Length] = Character;
                Length++;
                Character = getchar();
            }

            Value.String = NULL;
            Value.String = (char*) malloc(Length*sizeof(char));
            for(int i=0; i<Length; i++) Value.String[i] = '\0';

            int KeyWord = 0;
            if(!strcmp(String4, "function")) KeyWord = 4;
            else if(!strcmp(String4, "return")) KeyWord = 5;
            else if(!strcmp(String4, "while")) KeyWord = 6;
            else if(!strcmp(String4, "void")) KeyWord = 7;
            else if(!strcmp(String4, "else")) KeyWord = 8;
            else if(!strcmp(String4, "if")) KeyWord = 9;

            strcpy(Value.String,String4);
            free(String4);

            if(KeyWord == 0) *Token = T_Assign(*Token, T_TYPE_FUNCTION, Value);
            else *Token = T_Assign(*Token, KeyWord, Value);
            free(Value.String);

            Character = getchar();
            break;
        case EOF:   // End of File
            *Token = T_Assign(*Token, T_TYPE_EOF, Value);
            return 0;
            break;
        default:
            return 1;
            break;
        }
    }
}