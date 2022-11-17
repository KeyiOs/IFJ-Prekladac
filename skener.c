/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "error_handler.h"
#include "skener.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int Line;
extern int Token_Number;

int Strings(int *Character, Token_Value Value, _TOKEN_ *Token, int Type, FILE* Source){    
    char *String = (char*) malloc(10*sizeof(char));
    if(String == NULL) return 99;

    int Length = 0;
    for(int i=0; i<10; i++) String[Length+i] = '\0';

    switch(Type){
        case 1: // String
            *Character = getc(Source);
            while(*Character != '"'){
                if(*Character == '$') return 1; // $ Moze byt v stringu iba cez escape sekvenciu
                if(Length%10 == 9) {
                    String = (char*) realloc(String, Length+10);
                    if(String == NULL) return 99;
                    for(int i=0; i<10; i++) String[Length+i] = '\0';
                }
                if(*Character == '\\'){
                    *Character = getc(Source);
                    if(*Character == '"' || *Character == '\\'){
                        String[Length] = *Character;
                        Length++;
                        *Character = getc(Source);
                        continue;
                    } else if(*Character == 'n'){
                        String[Length] = 10;
                        Length++;
                        *Character = getc(Source);
                        continue;
                    } else if(*Character == 't'){
                        String[Length] = 9;
                        Length++;
                        *Character = getc(Source);
                        continue;
                    } else if(*Character == 'x'){   // Sestnactkova escape sekvencia
                        int ES[] = {'\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        *Character = getc(Source);
                        for(int a = 0; a < 2; a++){
                            if(48 > *Character || (*Character > 57 && 65 > *Character) || (*Character > 70 && 97 > *Character) || *Character > 102){    // (!= 0-9 && != a-f && != A-F)
                                Flag = 1;
                                String[Length] = 92;
                                String[Length+1] = 120;
                                Length += 2;
                                for(int b = 0; b < a; b++){
                                    String[Length] = ES[b];
                                    Length++;
                                }
                                String[Length] = *Character;
                                *Character = getc(Source);
                                Length++;
                                break;
                            }

                            ES[a] = *Character;
                            *Character = getc(Source);
                        }
                        if(Flag == 1) continue;

                        for(int a = 0; a < 2; a++){
                            if(ES[a] > 70) ES[a] = ES[a] - 87;
                            else if(ES[a] > 57) ES[a] = ES[a] - 55;
                            else ES[a] = ES[a] - 48;
                        }

                        int Converted = ES[0] * 16 + ES[1];
                        if(32 > Converted || Converted > 126) return 1;

                        String[Length] = Converted;
                        Length++;
                        continue;
                    } else if('0' <= *Character && *Character <= '2'){  // Desiatkova escape sekvencia
                        int ES[] = {'\0','\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        for(int a = 0; a < 3; a++){
                            if(48 > *Character || *Character > 57){
                                Flag = 1;
                                String[Length] = 92;
                                Length++;
                                for(int b = 0; b < a; b++){
                                    String[Length] = ES[b];
                                    Length++;
                                }
                                String[Length] = *Character;
                                *Character = getc(Source);
                                Length++;
                                break;
                            }

                            ES[a] = *Character;
                            *Character = getc(Source);
                        }
                        if(Flag == 1) continue;

                        int Converted = (ES[0] - 48) * 100 + (ES[1] - 48) * 10 + (ES[2] - 48);
                        if(32 > Converted || Converted > 126) return 1;

                        String[Length] = Converted;
                        Length++;
                        continue;
                    } else{
                        String[Length] = '\\';
                        String[Length+1] = *Character;
                        Length += 2;
                        *Character = getc(Source);
                    }
                }
                String[Length] = *Character;
                Length++;
                *Character = getc(Source);
            }
            *Character = getc(Source);
            break;
        case 3: // Datovy typ
            while('a' <= *Character && *Character <= 'z'){
                if(Length == 6) return 1;
                String[Length] = *Character;
                Length++;
                *Character = getc(Source);
            }
            break;
        case 2: // Premenna
        case 4: // Funkcie && Klucove slova
            if(Type == 2) *Character = getc(Source);
            if((65 > *Character || *Character > 90) && (97 > *Character || *Character > 122) && *Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Chybny prvy symbol
            while((47 < *Character && *Character < 58) || (64 < *Character && *Character < 91) || (96 < *Character && *Character < 123) || *Character == '_'){
                if(Length%10 == 9) {
                    String = (char*) realloc(String, Length+10);
                    if(String == NULL) return 99;
                    for(int i=0; i<10; i++) String[Length+i] = '\0';
                }
                String[Length] = *Character;
                Length++;
                *Character = getc(Source);
            }
            break;
        default:
            break;
    }

    int KeyWord = 0;
    switch(Type){
        case 3: // Datovy typ
        case 4: // Funkcie && Klucove slova
            if(!strcmp(String, "function")) KeyWord = 1;
            else if(!strcmp(String, "return")) KeyWord = 2;
            else if(!strcmp(String, "while")) KeyWord = 3;
            else if(!strcmp(String, "void")) KeyWord = 4;
            else if(!strcmp(String, "else")) KeyWord = 5;
            else if(!strcmp(String, "if")) KeyWord = 6;

            else if(!strcmp(String, "substring")) KeyWord = 7;
            else if(!strcmp(String, "floatval")) KeyWord = 8;
            else if(!strcmp(String, "intval")) KeyWord = 9;
            else if(!strcmp(String, "strval")) KeyWord = 10;
            else if(!strcmp(String, "strlen")) KeyWord = 11;
            else if(!strcmp(String, "write")) KeyWord = 12;
            else if(!strcmp(String, "reads")) KeyWord = 13;
            else if(!strcmp(String, "readi")) KeyWord = 14;
            else if(!strcmp(String, "readf")) KeyWord = 15;
            else if(!strcmp(String, "ord")) KeyWord = 16;
            else if(!strcmp(String, "chr")) KeyWord = 17;

            else if(!strcmp(String,"string")) KeyWord = 18;
            else if(!strcmp(String,"float")) KeyWord = 19;
            else if(!strcmp(String, "null")) KeyWord = 20;
            else if(!strcmp(String,"int")) KeyWord = 21;

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
    }
    if(Type == 1) Token = T_Assign(Token, T_TYPE_STRING_DATATYPE, Value, 0);
    else if(Type == 2) Token = T_Assign(Token, T_TYPE_VARIABLE, Value, 0);
    else if(Type == 4){
        if(KeyWord == 0) Token = T_Assign(Token, T_TYPE_FUNCTION, Value, 0);
        else Token = T_Assign(Token, T_TYPE_KEYWORD, Value, KeyWord);
    }
    free(Value.String);

    return 0;
}

int Prolog(FILE* Source, int *Character){
    *Character = getc(Source);

    char *String = (char*) malloc(25*sizeof(char));
    if(String == NULL) return 99;

    int Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    while(*Character != ' ' && *Character != 9 && *Character != 10 && *Character != 13) {
        String[Length] = *Character;
        *Character = getc(Source);
        Length++;
    }
    if(strcmp(String,"<?php")) {
        free(String);
        return 1;
    }
    
    while(*Character == ' ' || *Character == 9) *Character = getc(Source);
    if(*Character == 10 || *Character == 13) {
        *Character = getc(Source);
        Line++;
    } else if(*Character == '/') {
        if(Comment(&*Character, Source) == 0) return 1;
        else {
            *Character = getc(Source);
            Line++;
        }
    }

    Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    while(*Character != ' ' && *Character != 9 && *Character != 10 && *Character != 13 && *Character != EOF && Length != 25) {
        String[Length] = *Character;
        *Character = getc(Source);
        Length++;
    }
    if(Length == 25) return 1;
    else if(strcmp(String,"declare(strict_types=1);")) {
        free(String);
        return 1;
    }

    *Character = getc(Source);
    return 0;
}

int Comment(int *Character, FILE* Source){
    if(*Character == '/'){   // Riadkovy komentar
        while(*Character != EOF && *Character != 10 && *Character != 13) *Character = getc(Source);
        return 1;
    }
    else if(*Character == '*') { // Viacriadkovy komentar
        while(*Character != EOF) {
            *Character = getc(Source);
            if(*Character == '*'){
                *Character = getc(Source);
                if(*Character == '/'){
                    *Character = getc(Source);
                    return 1;
                }
            } else if(*Character == 10 || *Character == 13) Line++;
        }
        return 1;
    }

    return 0;
}

int Scan(_TOKEN_ *Token, FILE* Source, int *Character){
    Token_Value Value;
    int ERR = 0;

    switch(*Character){
        case 9:     // Tabulator
        case 11:    //
        case ' ':   //
            while(*Character == 9 || *Character == 11 || *Character == ' ') *Character = getc(Source);
            return Scan(Token, Source, Character);
        case 10:    // Novy riadok
        case 13:    //
            while(*Character == 10 || *Character == 13) {
                *Character = getc(Source); 
                Line++;
            }
            return Scan(Token, Source, Character);
        case '!':
            *Character = getc(Source);
            if(*Character != '=') return 1;
            *Character = getc(Source);
            if(*Character != '=') return 1;
            Token = T_Assign(Token, T_TYPE_TRIPLE_EQUALS_NEG, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '"':   // String
            ERR = Strings(&*Character, Value, Token, 1, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '$':   // Premenna
            ERR = Strings(&*Character, Value, Token, 2, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '(':
            Token = T_Assign(Token, T_TYPE_OPEN_BRACKET, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ')':
            Token = T_Assign(Token, T_TYPE_CLOSED_BRACKET, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '*':
            Token = T_Assign(Token, T_TYPE_MULTIPLICATION, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '+':
            Token = T_Assign(Token, T_TYPE_PLUS, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ',':
            Token = T_Assign(Token, T_TYPE_COLON, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '-':
            Token = T_Assign(Token, T_TYPE_MINUS, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '.':
            Token = T_Assign(Token, T_TYPE_CONCATENATION, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '/':
            *Character = getc(Source);
            if(Comment(&*Character, Source) == 1) return Scan(Token, Source, Character);
            Token = T_Assign(Token, T_TYPE_DIVISION, Value, 0);
            return 0;
            break;
        case ':':
            Token = T_Assign(Token, T_TYPE_COLON, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ';':
            Token = T_Assign(Token, T_TYPE_SEMICOLON, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '<':
            *Character = getc(Source);
            if(*Character == '='){
                Token = T_Assign(Token, T_TYPE_SMALLER_EQUAL, Value, 0);
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_SMALLER, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '=':
            *Character = getc(Source);
            if(*Character == '='){
                *Character = getc(Source);
                if(*Character != '=') return 1;
                Token = T_Assign(Token, T_TYPE_TRIPLE_EQUALS, Value, 0);;
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_EQUAL, Value, 0);
            return 0;
            break;
        case '>':
            *Character = getc(Source);
            if(*Character == '='){
                Token = T_Assign(Token, T_TYPE_GREATER_EQUAL, Value, 0);
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_GREATER, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '?':   // Datovy typ
            *Character = getc(Source);
            if(*Character == '>') {
                *Character = getc(Source);
                if(*Character == EOF) return 0;
                return 1;
            }
            ERR = Strings(&*Character, Value, Token, 3, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '{':
            Token = T_Assign(Token, T_TYPE_OPEN_CURLY_BRACKET, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '}':
            Token = T_Assign(Token, T_TYPE_CLOSED_CURLY_BRACKET, Value, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '0' ... '9':   // Integer alebo float
            int Float = 0;
            float Number = 0.0;
            int Length = 0;
            while(*Character != ' ' && *Character != ')' && *Character != ';' && *Character != 10 && *Character != 13 && *Character != EOF){
                if((48 > *Character || *Character > 57) && *Character != '.') return 1;
                if(Float == 1) Length ++;
                if(*Character == '.'){
                    if(Float == 0) Float = 1;
                    else return 1;
                }
                else Number = Number * 10 + *Character - 48;
                *Character = getc(Source);
            }
            if(Float == 0){
                Value.Integer = Number;
                Token = T_Assign(Token, T_TYPE_INT_DATATYPE, Value, 0);
                return 0;
                break;
            }
            for(int a = 0; a < Length; a ++) Number = Number / 10;
            Value.Float = Number;
            Token = T_Assign(Token, T_TYPE_FLOAT_DATATYPE, Value, 0);
            return 0;
            break;
        case 'a' ... 'z':
        case 'A' ... 'Z':
            ERR = Strings(&*Character, Value, Token, 4, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case EOF:   // Koniec vstupu
            Token = T_Assign(Token, T_TYPE_EOF, Value, 0);
            return 0;
            break;
        default:
            return 1;
            break;
    }

    return 99;
}