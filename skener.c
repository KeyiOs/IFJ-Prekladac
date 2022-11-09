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
        case 1: // String
            *Character = getchar();
            while(*Character != '"'){
                if(*Character == '$') return 1; // $ Moze byt v stringu iba cez escape sekvenciu
                if(Length%10 == 9) {
                    String = (char*) realloc(String, Length+10);
                    if(String == NULL) return 99;
                    for(int i=0; i<10; i++) String[Length+i] = '\0';
                }s
                if(*Character == '\\'){
                    *Character = getchar();
                    if(*Character == '"' || *Character == '\\'){
                        String[Length] = *Character;
                        Length++;
                        *Character = getchar();
                        continue;
                    } else if(*Character == 'n'){
                        String[Length] = 10;
                        Length++;
                        *Character = getchar();
                        continue;
                    } else if(*Character == 't'){
                        String[Length] = 9;
                        Length++;
                        *Character = getchar();
                        continue;
                    } else if(*Character == 'x'){   // Sestnactkova escape sekvencia
                        int ES[] = {'\0','\0'}; // ES - Escape Sekvence
                        int Flag = 0;   // Neplatna escape sekvencia

                        *Character = getchar();
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
                                *Character = getchar();
                                Length++;
                                break;
                            }

                            ES[a] = *Character;
                            *Character = getchar();
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
                                *Character = getchar();
                                Length++;
                                break;
                            }

                            ES[a] = *Character;
                            *Character = getchar();
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
                        *Character = getchar();
                    }
                }
                String[Length] = *Character;
                Length++;
                *Character = getchar();
            }
            *Character = getchar();
            break;
        case 3: // Datovy typ
            *Character = getchar();
            while('a' <= *Character && *Character <= 'z'){
                if(Length == 6) return 1;
                String[Length] = *Character;
                Length++;
                *Character = getchar();
            }
            break;
        case 2: // Premenna
        case 4: // Funkcie && Klucove slova
            if(Type == 2) *Character = getchar();
            if((65 > *Character || *Character > 90) && (97 > *Character || *Character > 122) && *Character != '_') return 1; // ((!= A-Z) && (!= a-z) && != '_') -> Chybny prvy symbol
            while(*Character != ' ' && *Character != EOF && *Character != 10){
                if((48 > *Character || *Character > 57) && (65 > *Character || *Character > 90) && (97 > *Character || *Character > 122) && *Character != '_') return 1;   // Chybny symbol v postupnosti (Pridane cisla)
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
        case 3: // Datovy typ
            if(!strcmp(String,"float")) *Token = T_Assign(*Token, T_TYPE_FLOAT_KEYWORD, Value);
            else if(!strcmp(String,"int")) *Token = T_Assign(*Token, T_TYPE_INT_KEYWORD, Value);
            else if(!strcmp(String,"string")) *Token = T_Assign(*Token, T_TYPE_STRING_KEYWORD, Value);
            else return 1;
            break;
        case 4: // Funkcie && Klucove slova
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
    int ERR = 0;

    int Character = getchar();

    while(1){
        switch(Character){
        case 9:     // Tabulator
        case 11:    //
        case ' ':   //
            Character = getchar();
            break;
        case 10:    // Novy riadok
        case 13:    //
            *Token = T_Assign(*Token, T_TYPE_EOL, Value);
            Character = getchar();
            break;
        case '"':   // String
            ERR = Strings(&Character, Value, Token, 1);
            if(ERR != 0) return ERR;
            break;
        case '$':   // Premenna
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
            if(Character == '/'){   // Riadkovy komentar
                while(Character != EOF && Character != 10 && Character != 13) Character = getchar();
                break;
            }
            else if(Character == '*') { // Viacriadkovy komentar
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
        case '?':   // Datovy typ
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
        case '0' ... '9':   // Integer alebo float
            int Float = 0;
            float Number = 0.0;
            int Length = 0;
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
        case EOF:   // Koniec vstupu
            *Token = T_Assign(*Token, T_TYPE_EOF, Value);
            return 0;
            break;
        default:
            return 1;
            break;
        }
    }
}