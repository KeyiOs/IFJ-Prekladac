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
            if(Token->Value.String == NULL) return NULL;
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

int Strings(int *Character, Token_Value Value, _TOKEN_ **Token, int Type){
    char *String = (char*) malloc(10*sizeof(char));
    if(String == NULL) return 99;

    int Length = 0;
    for(int i=0; i<10; i++) String[Length+i] = '\0';

    switch(Type){
        case 1:
            *Character = getchar();
            while(*Character != '"'){
                if(Length%10 == 9) {
                    String = (char*) realloc(String, Length+10);
                    if(String == NULL) return 99;
                    for(int i=0; i<10; i++) String[Length+i] = '\0';
                }
                String[Length] = *Character;
                Length++;
                *Character = getchar();
            }
            *Character = getchar();
            break;
        case 3: // Datatypes
            *Character = getchar();
            while('a' <= *Character && *Character <= 'z'){
                if(Length == 6) return 1;
                String[Length] = *Character;
                Length++;
                *Character = getchar();
            }
            break;
        case 2: // Identificators
        case 4: // Functions && Keywords
            if(Type == 2) *Character = getchar();
            if((65 > *Character || *Character > 90) && (97 > *Character || *Character > 122) && *Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Including invalid first symbol
            while(*Character != ' ' && *Character != EOF && *Character != 10){
                if((48 > *Character || *Character > 57) && (65 > *Character || *Character > 90) && (97 > *Character || *Character > 122) && *Character != '_') return 1;   // Added numbers as viable symbols
                if(Length%10 == 9) {
                    String = (char*) realloc(String, Length+10);
                    if(String == NULL) return 99;
                    for(int i=0; i<10; i++) String[Length+i] = '\0';
                }
                String[Length] = *Character;
                Length++;
                *Character = getchar();
            }
            break;
        default:
            break;
    }

    int KeyWord = 0;
    switch(Type){
        case 3: // Datatypes
            if(!strcmp(String,"float")) *Token = T_Assign(*Token, T_TYPE_FLOAT_KEYWORD, Value);
            else if(!strcmp(String,"int")) *Token = T_Assign(*Token, T_TYPE_INT_KEYWORD, Value);
            else if(!strcmp(String,"string")) *Token = T_Assign(*Token, T_TYPE_STRING_KEYWORD, Value);
            else return 1;
            break;
        case 4: // Functions && Keywords
            if(!strcmp(String, "function")) KeyWord = 4;
            else if(!strcmp(String, "return")) KeyWord = 5;
            else if(!strcmp(String, "while")) KeyWord = 6;
            else if(!strcmp(String, "void")) KeyWord = 7;
            else if(!strcmp(String, "else")) KeyWord = 8;
            else if(!strcmp(String, "if")) KeyWord = 9;
            break;
        default:
            break;
    }

    if(Type != 3){
        Value.String = NULL;
        Value.String = (char*) malloc(Length*sizeof(char));
        if(Value.String == NULL) return 99;
        for(int i=0; i<Length; i++) Value.String[i] = '\0';

        strcpy(Value.String,String);
        free(String);

        if(Type == 1) *Token = T_Assign(*Token, T_TYPE_STRING_DATATYPE, Value);
        else if(Type == 2) *Token = T_Assign(*Token, T_TYPE_VARIABLE, Value);
        else if(Type == 4){
            if(KeyWord == 0) *Token = T_Assign(*Token, T_TYPE_FUNCTION, Value);
            else *Token = T_Assign(*Token, KeyWord, Value);
        }
        free(Value.String);
    } else free(String);

    return 0;
}

int Scan(_TOKEN_ **Token){
    Token_Value Value;
    int Length = 0;
    int ERR = 0;

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
            ERR = Strings(&Character, Value, Token, 1);
            if(ERR != 0) return ERR;
            break;
        case '$':   // Variable Identificator
            ERR = Strings(&Character, Value, Token, 2);
            if(ERR != 0) return ERR;
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
                        if(Character == '/'){
                            Character = getchar();
                            break;
                        }
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
            ERR = Strings(&Character, Value, Token, 3);
            if(ERR != 0) return ERR;
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
            ERR = Strings(&Character, Value, Token, 4);
            if(ERR != 0) return ERR;
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