/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentos
 */

#include "error_handler.h"
#include "skener.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int Line;
extern int Token_Number;

int Strings(_WRAP_ *Wrap, char *String, int Type){    
    char *StringNew = (char*) malloc(10*sizeof(char));
    if(StringNew == NULL) return 99;

    int Length = 0;
    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';

    switch(Type){
        case 1: // String
            Wrap->Character = getc(Wrap->Source);
            while(Wrap->Character != '"'){
                if(Wrap->Character == '$') return 1; // $ Moze byt v stringu iba cez escape sekvenciu
                if(Length%10 == 9) {
                    StringNew = (char*) realloc(StringNew, Length+10);
                    if(StringNew == NULL) return 99;
                    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';
                }
                if(Wrap->Character == '\\'){
                    Wrap->Character = getc(Wrap->Source);
                    if(Wrap->Character == '"' || Wrap->Character == '\\'){
                        StringNew[Length] = Wrap->Character;
                        Length++;
                        Wrap->Character = getc(Wrap->Source);
                        continue;
                    } else if(Wrap->Character == 'n'){
                        StringNew[Length] = 10;
                        Length++;
                        Wrap->Character = getc(Wrap->Source);
                        continue;
                    } else if(Wrap->Character == 't'){
                        StringNew[Length] = 9;
                        Length++;
                        Wrap->Character = getc(Wrap->Source);
                        continue;
                    } else if(Wrap->Character == 'x'){   // Sestnactkova escape sekvencia
                        int ES[] = {'\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        Wrap->Character = getc(Wrap->Source);
                        for(int a = 0; a < 2; a++){
                            if(48 > Wrap->Character || (Wrap->Character > 57 && 65 > Wrap->Character) || (Wrap->Character > 70 && 97 > Wrap->Character) || Wrap->Character > 102){    // (!= 0-9 && != a-f && != A-F)
                                Flag = 1;
                                StringNew[Length] = 92;
                                StringNew[Length+1] = 120;
                                Length += 2;
                                for(int b = 0; b < a; b++){
                                    StringNew[Length] = ES[b];
                                    Length++;
                                }
                                StringNew[Length] = Wrap->Character;
                                Wrap->Character = getc(Wrap->Source);
                                Length++;
                                break;
                            }

                            ES[a] = Wrap->Character;
                            Wrap->Character = getc(Wrap->Source);
                        }
                        if(Flag == 1) continue;

                        for(int a = 0; a < 2; a++){
                            if(ES[a] > 70) 
                                ES[a] = ES[a] - 87;
                            else if(ES[a] > 57) ES[a] = ES[a] - 55;
                            else ES[a] = ES[a] - 48;
                        }

                        int Converted = ES[0] * 16 + ES[1];
                        if(32 > Converted || Converted > 126) return 1;

                        StringNew[Length] = Converted;
                        Length++;
                        continue;
                    } else if('0' <= Wrap->Character && Wrap->Character <= '2'){  // Desiatkova escape sekvencia
                        int ES[] = {'\0','\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        for(int a = 0; a < 3; a++){
                            if(48 > Wrap->Character || Wrap->Character > 57){
                                Flag = 1;
                                StringNew[Length] = 92;
                                Length++;
                                for(int b = 0; b < a; b++){
                                    StringNew[Length] = ES[b];
                                    Length++;
                                }
                                StringNew[Length] = Wrap->Character;
                                Wrap->Character = getc(Wrap->Source);
                                Length++;
                                break;
                            }

                            ES[a] = Wrap->Character;
                            Wrap->Character = getc(Wrap->Source);
                        }
                        if(Flag == 1) continue;

                        int Converted = (ES[0] - 48) * 100 + (ES[1] - 48) * 10 + (ES[2] - 48);
                        if(32 > Converted || Converted > 126) return 1;

                        StringNew[Length] = Converted;
                        Length++;
                        continue;
                    } else{
                        StringNew[Length] = '\\';
                        StringNew[Length+1] = Wrap->Character;
                        Length += 2;
                        Wrap->Character = getc(Wrap->Source);
                    }
                }
                StringNew[Length] = Wrap->Character;
                Length++;
                Wrap->Character = getc(Wrap->Source);
            }
            Wrap->Character = getc(Wrap->Source);
            break;
        case 3: // Datovy typ
            while('a' <= Wrap->Character && Wrap->Character <= 'z'){
                if(Length == 6) return 1;
                StringNew[Length] = Wrap->Character;
                Length++;
                Wrap->Character = getc(Wrap->Source);
            }
            break;
        case 2: // Premenna
        case 4: // Funkcie && Klucove slova
            if(Type == 2) Wrap->Character = getc(Wrap->Source);
            if((65 > Wrap->Character || Wrap->Character > 90) && (97 > Wrap->Character || Wrap->Character > 122) && Wrap->Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Chybny prvy symbol
            while((47 < Wrap->Character && Wrap->Character < 58) || (64 < Wrap->Character && Wrap->Character < 91) || (96 < Wrap->Character && Wrap->Character < 123) || Wrap->Character == '_'){
                if(Length%10 == 9) {
                    StringNew = (char*) realloc(StringNew, Length+10);
                    if(StringNew == NULL) return 99;
                    for(int i=0; i<10; i++) StringNew[Length+i] = '\0';
                }
                StringNew[Length] = Wrap->Character;
                Length++;
                Wrap->Character = getc(Wrap->Source);
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
                Wrap->Token = T_Assign(Wrap->Token, T_TYPE_NULL_DATATYPE, String, 0);
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
    if(Type == 1) Wrap->Token = T_Assign(Wrap->Token, T_TYPE_STRING_DATATYPE, String, 0);
    else if(Type == 2) Wrap->Token = T_Assign(Wrap->Token, T_TYPE_VARIABLE, String, 0);
    else if(Type == 3 || Type == 4){
        if(KeyWord == 0) Wrap->Token = T_Assign(Wrap->Token, T_TYPE_FUNCTION, String, 0);
        else Wrap->Token = T_Assign(Wrap->Token, T_TYPE_KEYWORD, String, KeyWord);
    }
    free(StringNew);

    return 0;
}

int Prolog(_WRAP_ *Wrap){
    if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;

    char *String = (char*) malloc(25*sizeof(char));
    if(String == NULL) return 99;

    int Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    while(Wrap->Character != ' ' && Wrap->Character != 9 && Wrap->Character != 10 && Wrap->Character != 13) {
        String[Length] = Wrap->Character;
        if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        Length++;
    }
    if(strcmp(String,"<?php")) {
        free(String);
        return 2;
    }

    while(Wrap->Character == ' ' || Wrap->Character == 9) if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
    if(Wrap->Character == 10 || Wrap->Character == 13) {
        printf("Start: %i\n", Wrap->Character);
        if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        if(Wrap->Character == 10 && (Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        printf("Start 2: %i\n", Wrap->Character);
        Line++;
    } else if(Wrap->Character == '/') {
        int cmnt;
        if((cmnt = Comment(Wrap)) == 0) return 2;
        else if(cmnt == 100) {
            if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
            Line++;
        } else return cmnt;
    }

    Length = 0;
    for(int i=0; i<25; i++) String[Length+i] = '\0';
    printf("End: %i\n", Wrap->Character);
    while(Wrap->Character != ' ' && Wrap->Character != 9 && Wrap->Character != 10 && Wrap->Character != 13 && Wrap->Character != EOF && Length != 25) {
        String[Length] = Wrap->Character;
        printf("Char[%i]: %i\n", Length, Wrap->Character);
        if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        Length++;
    }
    if(Length == 25) return 1;
    else if(strcmp(String,"declare(strict_types=1);")) {
        free(String);
        return 1;
    }

    if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
    return 0;
}

int Comment(_WRAP_ *Wrap){
    if(Wrap->Character == '/'){   // Riadkovy komentar
        while(Wrap->Character != EOF && Wrap->Character != 10 && Wrap->Character != 13) if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        if(Wrap->Character == 13 && (Wrap->Character = getc(Wrap->Source)) == -1) return 1;
        return 100;
    }
    else if(Wrap->Character == '*') { // Viacriadkovy komentar
        while(Wrap->Character != EOF) {
            if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
            if(Wrap->Character == '*'){
                if((Wrap->Character = getc(Wrap->Source)) == -1) return 1;
                if(Wrap->Character == '/'){
                    Wrap->Character = getc(Wrap->Source);
                    return 100;
                }
            } else if(Wrap->Character == 10 || Wrap->Character == 13) Line++;
        }
    }

    return 0;
}

int Scan(_WRAP_ *Wrap){
    char *String = "\0";
    int ERR = 0;

    switch(Wrap->Character){
        case 9:     // Tabulator
        case 11:    //
        case ' ':   //
            while(Wrap->Character == 9 || Wrap->Character == 11 || Wrap->Character == ' ') if((Wrap->Character = getc(Wrap->Source)) == -1) {
                Wrap->Token = T_Assign(Wrap->Token, T_TYPE_EOF, String, 0);
                return 0;
            }
            return Scan(Wrap);
        case 10:    // Novy riadok
        case 13:    //
            while(Wrap->Character == 10 || Wrap->Character == 13) {
                Wrap->Character = getc(Wrap->Source); 
                Line++;
            }
            return Scan(Wrap);
        case '!':
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character != '=') return 1;
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character != '=') return 1;
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_TRIPLE_EQUALS_NEG, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '"':   // String
            ERR = Strings(Wrap, String, 1);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '$':   // Premenna
            ERR = Strings(Wrap, String, 2);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '(':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_OPEN_BRACKET, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case ')':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_CLOSED_BRACKET, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '*':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_MULTIPLICATION, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '+':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_PLUS, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case ',':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_COMMA, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '-':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_MINUS, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '.':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_CONCATENATION, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '/':
            Wrap->Character = getc(Wrap->Source);
            int cmnt;
            if((cmnt = Comment(Wrap)) == 100) return Scan(Wrap);
            else if(cmnt == 0) Wrap->Token = T_Assign(Wrap->Token, T_TYPE_DIVISION, String, 0);
            else return cmnt;
            return 0;
            break;
        case ':':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_COLON, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case ';':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_SEMICOLON, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '<':
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character == '='){
                Wrap->Token = T_Assign(Wrap->Token, T_TYPE_SMALLER_EQUAL, String, 0);
                Wrap->Character = getc(Wrap->Source);
                return 0;
                break;
            }
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_SMALLER, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '=':
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character == '='){
                Wrap->Character = getc(Wrap->Source);
                if(Wrap->Character != '=') return 1;
                Wrap->Token = T_Assign(Wrap->Token, T_TYPE_TRIPLE_EQUALS, String, 0);;
                Wrap->Character = getc(Wrap->Source);
                return 0;
                break;
            }
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_EQUAL, String, 0);
            return 0;
            break;
        case '>':
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character == '='){
                Wrap->Token = T_Assign(Wrap->Token, T_TYPE_GREATER_EQUAL, String, 0);
                Wrap->Character = getc(Wrap->Source);
                return 0;
                break;
            }
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_GREATER, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '?':   // Datovy typ
            Wrap->Character = getc(Wrap->Source);
            if(Wrap->Character == '>') {
                Wrap->Character = getc(Wrap->Source);
                if(Wrap->Character == EOF) {
                    Wrap->Token = T_Assign(Wrap->Token, T_TYPE_EOF, String, 0);
                    return 0;
                }
                return 1;
            }
            ERR = Strings(Wrap, String, 3);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case '{':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_OPEN_CURLY_BRACKET, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '}':
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_CLOSED_CURLY_BRACKET, String, 0);
            Wrap->Character = getc(Wrap->Source);
            return 0;
            break;
        case '0' ... '9': {   // Integer alebo float
            int Float = 0;
            int Len = 0;
            String = malloc(10);
            if(String == NULL) return 99;
            memset(String, '\0', 10);
            while((48 <= Wrap->Character && Wrap->Character <= 57) || Wrap->Character == '.'){
                if(Len%10 == 0) {
                    String = realloc(String, sizeof(String)+10);
                    if(String == NULL) return 99;
                    memset(String, '\0', sizeof(String));
                }
                String[Len] = Wrap->Character;
                Len++;
                if(Wrap->Character == '.'){
                    if(Float == 0) Float = 1;
                    else return 1;
                }
                Wrap->Character = getc(Wrap->Source);
            }
            if(Float == 0) Wrap->Token = T_Assign(Wrap->Token, T_TYPE_INT_DATATYPE, String, 0);
            else Wrap->Token = T_Assign(Wrap->Token, T_TYPE_FLOAT_DATATYPE, String, 0);
            return 0;
            break;
        }
        case 'a' ... 'z':
        case 'A' ... 'Z':
            ERR = Strings(Wrap, String, 4);
            if(ERR != 0) return ERR;
            return 0;
            break;
        case EOF:   // Koniec vstupu
            Wrap->Token = T_Assign(Wrap->Token, T_TYPE_EOF, String, 0);
            return 0;
            break;
        default:
            return 1;
            break;
    }

    return 99;
}