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

int Strings(int *Character, char *String, _TOKEN_ *Token, int Type, FILE* Source){    
    char *StringNew = (char*) malloc(10*sizeof(char));
    if(StringNew == NULL) return 99;

    int Length = 0;
    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';

    switch(Type){
        case 1: // String
            *Character = getc(Source);
            while(*Character != '"'){
                if(*Character == '$') return 1; // $ Moze byt v stringu iba cez escape sekvenciu
                if(Length%10 == 9) {
                    StringNew = (char*) realloc(StringNew, Length+10);
                    if(StringNew == NULL) return 99;
                    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';
                }
                if(*Character == '\\'){
                    *Character = getc(Source);
                    if(*Character == '"' || *Character == '\\'){
                        StringNew[Length] = *Character;
                        Length++;
                        *Character = getc(Source);
                        continue;
                    } else if(*Character == 'n'){
                        StringNew[Length] = 10;
                        Length++;
                        *Character = getc(Source);
                        continue;
                    } else if(*Character == 't'){
                        StringNew[Length] = 9;
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
                                StringNew[Length] = 92;
                                StringNew[Length+1] = 120;
                                Length += 2;
                                for(int b = 0; b < a; b++){
                                    StringNew[Length] = ES[b];
                                    Length++;
                                }
                                StringNew[Length] = *Character;
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

                        StringNew[Length] = Converted;
                        Length++;
                        continue;
                    } else if('0' <= *Character && *Character <= '2'){  // Desiatkova escape sekvencia
                        int ES[] = {'\0','\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        for(int a = 0; a < 3; a++){
                            if(48 > *Character || *Character > 57){
                                Flag = 1;
                                StringNew[Length] = 92;
                                Length++;
                                for(int b = 0; b < a; b++){
                                    StringNew[Length] = ES[b];
                                    Length++;
                                }
                                StringNew[Length] = *Character;
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

                        StringNew[Length] = Converted;
                        Length++;
                        continue;
                    } else{
                        StringNew[Length] = '\\';
                        StringNew[Length+1] = *Character;
                        Length += 2;
                        *Character = getc(Source);
                    }
                }
                StringNew[Length] = *Character;
                Length++;
                *Character = getc(Source);
            }
            *Character = getc(Source);
            break;
        case 3: // Datovy typ
            while('a' <= *Character && *Character <= 'z'){
                if(Length == 6) return 1;
                StringNew[Length] = *Character;
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
                    StringNew = (char*) realloc(StringNew, Length+10);
                    if(StringNew == NULL) return 99;
                    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';
                }
                StringNew[Length] = *Character;
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
            if(!strcmp(StringNew, "function")) KeyWord = 1;
            else if(!strcmp(StringNew, "return")) KeyWord = 2;
            else if(!strcmp(StringNew, "while")) KeyWord = 3;
            else if(!strcmp(StringNew, "else")) KeyWord = 4;
            else if(!strcmp(StringNew, "if")) KeyWord = 5;

            else if(!strcmp(StringNew, "substring")) KeyWord = 6;
            else if(!strcmp(StringNew, "floatval")) KeyWord = 7;
            else if(!strcmp(StringNew, "intval")) KeyWord = 8;
            else if(!strcmp(StringNew, "strval")) KeyWord = 9;
            else if(!strcmp(StringNew, "strlen")) KeyWord = 10;
            else if(!strcmp(StringNew, "write")) KeyWord = 11;
            else if(!strcmp(StringNew, "reads")) KeyWord = 12;
            else if(!strcmp(StringNew, "readi")) KeyWord = 13;
            else if(!strcmp(StringNew, "readf")) KeyWord = 14;
            else if(!strcmp(StringNew, "ord")) KeyWord = 15;
            else if(!strcmp(StringNew, "chr")) KeyWord = 16;

            else if(!strcmp(StringNew, "string")) KeyWord = 17;
            else if(!strcmp(StringNew, "float")) KeyWord = 18;
            else if(!strcmp(StringNew, "int")) KeyWord = 20;
            else if(!strcmp(StringNew, "void")) KeyWord = 21;

            else if(!strcmp(StringNew, "null")) {
                Token = T_Assign(Token, T_TYPE_NULL_DATATYPE, String, 0);
                return 0;
            }
            break;
        default:
            break;
    }

    if(Type != 3){
        String = NULL;
        String = (char*) malloc(Length*sizeof(char));
        if(String == NULL) return 99;
        for(int i=0; i<Length; i++) String[i] = '\0';

        strcpy(String,StringNew);
    }
    if(Type == 1) Token = T_Assign(Token, T_TYPE_STRING_DATATYPE, String, 0);
    else if(Type == 2) Token = T_Assign(Token, T_TYPE_VARIABLE, String, 0);
    else if(Type == 3 || Type == 4){
        if(KeyWord == 0) Token = T_Assign(Token, T_TYPE_FUNCTION, String, 0);
        else Token = T_Assign(Token, T_TYPE_KEYWORD, String, KeyWord);
    }
    free(StringNew);

    return 0;
}

int Prolog(FILE* Source, int *Character){
    if((*Character = getc(Source)) == -1) return 1;

    char *String = (char*) malloc(25*sizeof(char));
    if(String == NULL) return 99;

    int Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    while(*Character != ' ' && *Character != 9 && *Character != 10 && *Character != 13) {
        String[Length] = *Character;
        if((*Character = getc(Source)) == -1) return 1;
        Length++;
    }
    if(strcmp(String,"<?php")) {
        free(String);
        return 2;
    }
    
    while(*Character == ' ' || *Character == 9) if((*Character = getc(Source)) == -1) return 1;
    if(*Character == 10 || *Character == 13) {
        if((*Character = getc(Source)) == -1) return 1;
        if(*Character == 13 && (*Character = getc(Source)) == -1) return 1;
        Line++;
    } else if(*Character == '/') {
        int cmnt;
        if((cmnt = Comment(Character, Source)) == 0) return 2;
        else if(cmnt == 100) {
            if((*Character = getc(Source)) == -1) return 1;
            Line++;
        } else return cmnt;
    }

    Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    while(*Character != ' ' && *Character != 9 && *Character != 10 && *Character != 13 && *Character != EOF && Length != 25) {
        String[Length] = *Character;
        if((*Character = getc(Source)) == -1) return 1;
        Length++;
    }
    if(Length == 25) return 1;
    else if(strcmp(String,"declare(strict_types=1);")) {
        free(String);
        return 1;
    }

    if((*Character = getc(Source)) == -1) return 1;
    return 0;
}

int Comment(int *Character, FILE* Source){
    if(*Character == '/'){   // Riadkovy komentar
        while(*Character != EOF && *Character != 10 && *Character != 13) if((*Character = getc(Source)) == -1) return 1;
        if(*Character == 13 && (*Character = getc(Source)) == -1) return 1;
        return 100;
    }
    else if(*Character == '*') { // Viacriadkovy komentar
        while(*Character != EOF) {
            if((*Character = getc(Source)) == -1) return 1;
            if(*Character == '*'){
                if((*Character = getc(Source)) == -1) return 1;
                if(*Character == '/'){
                    *Character = getc(Source);
                    return 100;
                }
            } else if(*Character == 10 || *Character == 13) Line++;
        }
    }

    return 0;
}

int Scan(_TOKEN_ *Token, FILE* Source, int *Character){
    char *String = "\0";
    int ERR = 0;

    switch(*Character){
        case 9:     // Tabulator
        case 11:    //
        case ' ':   //
            while(*Character == 9 || *Character == 11 || *Character == ' ') if((*Character = getc(Source)) == -1) {
                Token = T_Assign(Token, T_TYPE_EOF, String, 0);
                return 0;
            }
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
            Token = T_Assign(Token, T_TYPE_TRIPLE_EQUALS_NEG, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '"':   // String
            ERR = Strings(Character, String, Token, 1, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '$':   // Premenna
            ERR = Strings(Character, String, Token, 2, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '(':
            Token = T_Assign(Token, T_TYPE_OPEN_BRACKET, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ')':
            Token = T_Assign(Token, T_TYPE_CLOSED_BRACKET, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '*':
            Token = T_Assign(Token, T_TYPE_MULTIPLICATION, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '+':
            Token = T_Assign(Token, T_TYPE_PLUS, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ',':
            Token = T_Assign(Token, T_TYPE_COMMA, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '-':
            Token = T_Assign(Token, T_TYPE_MINUS, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '.':
            Token = T_Assign(Token, T_TYPE_CONCATENATION, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '/':
            *Character = getc(Source);
            int cmnt;
            if((cmnt = Comment(Character, Source)) == 100) return Scan(Token, Source, Character);
            else if(cmnt == 0) Token = T_Assign(Token, T_TYPE_DIVISION, String, 0);
            else return cmnt;
            return 0;
            break;
        case ':':
            Token = T_Assign(Token, T_TYPE_COLON, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case ';':
            Token = T_Assign(Token, T_TYPE_SEMICOLON, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '<':
            *Character = getc(Source);
            if(*Character == '='){
                Token = T_Assign(Token, T_TYPE_SMALLER_EQUAL, String, 0);
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_SMALLER, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '=':
            *Character = getc(Source);
            if(*Character == '='){
                *Character = getc(Source);
                if(*Character != '=') return 1;
                Token = T_Assign(Token, T_TYPE_TRIPLE_EQUALS, String, 0);;
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_EQUAL, String, 0);
            return 0;
            break;
        case '>':
            *Character = getc(Source);
            if(*Character == '='){
                Token = T_Assign(Token, T_TYPE_GREATER_EQUAL, String, 0);
                *Character = getc(Source);
                return 0;
                break;
            }
            Token = T_Assign(Token, T_TYPE_GREATER, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '?':   // Datovy typ
            *Character = getc(Source);
            if(*Character == '>') {
                *Character = getc(Source);
                if(*Character == EOF) {
                    Token = T_Assign(Token, T_TYPE_EOF, String, 0);
                    return 0;
                }
                return 1;
            }
            ERR = Strings(Character, String, Token, 3, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '{':
            Token = T_Assign(Token, T_TYPE_OPEN_CURLY_BRACKET, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '}':
            Token = T_Assign(Token, T_TYPE_CLOSED_CURLY_BRACKET, String, 0);
            *Character = getc(Source);
            return 0;
            break;
        case '0' ... '9': {   // Integer alebo float
            int Float = 0;
            while((48 <= *Character && *Character <= 57) || *Character == '.'){
                if(*Character == '.'){
                    if(Float == 0) Float = 1;
                    else return 1;
                }
                *Character = getc(Source);
            }
            if(Float == 0){
                Token = T_Assign(Token, T_TYPE_INT_DATATYPE, String, 0);
                return 0;
            }
            Token = T_Assign(Token, T_TYPE_FLOAT_DATATYPE, String, 0);
            return 0;
            break;
        }
        case 'a' ... 'z':
        case 'A' ... 'Z':
            ERR = Strings(Character, String, Token, 4, Source);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case EOF:   // Koniec vstupu
            Token = T_Assign(Token, T_TYPE_EOF, String, 0);
            return 0;
            break;
        default:
            return 1;
            break;
    }

    return 99;
}