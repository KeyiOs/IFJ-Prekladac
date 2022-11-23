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

int Line = 1;
int Token_Number = 0;

/*
 *  • Alokovanie tokenu
 */
_TOKEN_ *T_Create(){
    _TOKEN_ *Token = malloc(sizeof(struct Token));
    if(!Token) return NULL;

    Token->Type = T_TYPE_NULL;
    
    return Token;
}

/*
 *  • Zapis hodnot do tokenu
 */
_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, char *String, Token_Keyword Keyword){
    Token->Type = Type;
    Token->Keyword = T_KEYWORD_NULL;
    Token->String = "\0";
    Token_Number++;

    switch(Type){
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_VARIABLE:
        case T_TYPE_FUNCTION:
            int Length = strlen(String);

            Token->String = NULL;
            Token->String = (char*) malloc(Length*sizeof(char));
            if(Token->String == NULL) return NULL;
            for(int i=0; i<Length; i++) Token->String[i] = '\0';

            strcpy(Token->String,String);
            break;
        case T_TYPE_KEYWORD:
            Token->Keyword = Keyword;
            break;
        default:
            break;
    }

    return Token;
}

/*
 *  • Analyza termu
 *  • If term -> return 0
 *  • Else -> return 1
 */
int Term(Token_Type Type) {
    if(Type == T_TYPE_STRING_DATATYPE ||
       Type == T_TYPE_FLOAT_DATATYPE ||
       Type == T_TYPE_NULL ||
       Type == T_TYPE_INT_DATATYPE) return 0;

    return 1;
}

/*
 *  • Analyza syntaxe pre klucove slovo
 */
int Keyword(_TOKEN_ *Token, FILE* Source, int *Character, int Dive){
    int ERR;                                                                // Deklaracia
    if(Token->Keyword == T_KEYWORD_FUNCTION && Dive == 0) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Meno Funkcie
        if(Token->Type != T_TYPE_FUNCTION) return 2;                        //
        // TODO: Symtable Exists -> Error                                   //
        if((ERR = F_Declare(Token, Source, Character)) != 0) return ERR;    // Deklaracia Funkcie
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        if((ERR = Start(Token, Source, Character, Dive + 1)) != 0) return ERR;
    } else if(Token->Keyword == T_KEYWORD_RETURN && Dive != 0) {
        // TODO: Analyza vyrazov                                            // TODO: Odstranit
        while(Token->Type != T_TYPE_SEMICOLON) {                            // TODO: Odstranit
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // TODO: Odstranit
        }                                                                   // TODO: Odstranit
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       // Bodkociarka
    } else if(Token->Keyword == T_KEYWORD_WHILE) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        // TODO: Analyza vyrazov                                            // TODO: Odstranit
        while(Token->Type != T_TYPE_CLOSED_BRACKET) {                       // TODO: Odstranit
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // TODO: Odstranit
        }                                                                   // TODO: Odstranit
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        if((ERR = Start(Token, Source, Character, Dive + 1)) != 0) return ERR;
    } else if(Token->Keyword == T_KEYWORD_ELSE) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        if((ERR = Start(Token, Source, Character, Dive + 1)) != 0) return ERR;
    } else if(Token->Keyword == T_KEYWORD_IF) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        // TODO: Analyza vyrazov                                            // TODO: Odstranit
        while(Token->Type != T_TYPE_CLOSED_BRACKET) {                       // TODO: Odstranit
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // TODO: Odstranit
        }                                                                   // TODO: Odstranit
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        if((ERR = Start(Token, Source, Character, Dive + 1)) != 0) return ERR;
    } else if(Token->Keyword == T_KEYWORD_SUBSTRING) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // String
        if(Token->Type != T_TYPE_STRING_DATATYPE) {                         //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    // 
            // TODO: Symtable Check                                         //
        }                                                                   //
        for(int i = 0; i < 2; i++) {                                        //
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // Ciarka
            if(Token->Type != T_TYPE_COMMA) return 2;                       //
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // Int
            if(Token->Type != T_TYPE_INT_DATATYPE) {                        //
                if(Token->Type != T_TYPE_VARIABLE) return 2;                //
                // TODO: Symtable Check                                     //
            }                                                               //
        }                                                                   //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else if(Token->Keyword == T_KEYWORD_FLOATVAL ||
              Token->Keyword == T_KEYWORD_INTVAL) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Number
        if(Token->Type != T_TYPE_FLOAT_DATATYPE &&                          //
           Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            // TODO: Symtable Check                                         //
        }                                                                   //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else if(Token->Keyword == T_KEYWORD_STRVAL ||
              Token->Keyword == T_KEYWORD_STRLEN ||
              Token->Keyword == T_KEYWORD_ORD) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // String
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_NULL_DATATYPE) {                           //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            // TODO: Symtable Check                                         //
        }                                                                   //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else if(Token->Keyword == T_KEYWORD_WRITE) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        while(Token->Type != T_TYPE_CLOSED_BRACKET) {                       // Zatvorena Zatvorka
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // Term
            if(Token->Type != T_TYPE_STRING_DATATYPE &&                     //
               Token->Type != T_TYPE_FLOAT_DATATYPE &&                      //      
               Token->Type != T_TYPE_NULL_DATATYPE &&                       //
               Token->Type != T_TYPE_INT_DATATYPE) {                        //
                if(Token->Type != T_TYPE_VARIABLE) return 2;                //
                // TODO: Symtable Check                                     //
            }                                                               //
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // Ciarka
            if(Token->Type != T_TYPE_COMMA &&                               //
               Token->Type != T_TYPE_CLOSED_BRACKET) return 2;              //
        }                                                                   //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else if(Token->Keyword == T_KEYWORD_READS ||
              Token->Keyword == T_KEYWORD_READI ||
              Token->Keyword == T_KEYWORD_READF) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else if(Token->Keyword == T_KEYWORD_CHR) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Int
        if(Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            // TODO: Symtable Check                                         //
        }                                                                   //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
    } else return 2;

    return 0;
}

/*
 *  • Deklaracia funkcie
 */
int F_Declare(_TOKEN_ *Token, FILE* Source, int *Character){
    int ERR;                                                                // Deklaracia
    // TODO: Symtable -> Declare Function                                   //
    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Otvorena Zatvorka
    if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                        //
    while(Token->Type != T_TYPE_CLOSED_BRACKET) {                           // Zatvorena Zatvorka
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Datovy Typ
        if(Token->Type != T_TYPE_KEYWORD) return 2;                         //
        if(Token->Keyword != T_KEYWORD_STRING &&                            //
           Token->Keyword != T_KEYWORD_FLOAT &&                             //
           Token->Keyword != T_KEYWORD_INT) return 2;                       //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Premenna
        if(Token->Type != T_TYPE_VARIABLE) return 2;                        //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Ciarka/Zatvorka
        if(Token->Type != T_TYPE_COMMA &&                                   //
           Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        // TODO: Symtable -> Add Param                                      //
    }                                                                       //
    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Dvojbodka
    if(Token->Type != T_TYPE_COLON) return 2;                               //
    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Datovy Typ
    if(Token->Type != T_TYPE_KEYWORD) return 2;                             //
    if(Token->Keyword != T_KEYWORD_STRING &&                                //
       Token->Keyword != T_KEYWORD_FLOAT &&                                 //
       Token->Keyword != T_KEYWORD_INT) return 2;                           //

    return 0;
}

/*
 *  • Analyza syntaxe pre funkciu
 *  • Overenie v Tabulke
 */
int Function(_TOKEN_ *Token, FILE* Source, int *Character, int Dive){
    int ERR;                                                                // Deklaracia
    char *Name = Token->String;                                             //
    // TODO: Check Name                                                     //
    _PARAM_ *Params; // TODO: Get Tokens                                    //

    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Otovrena Zatvorka
    if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                        //
    while(Params->Next != NULL) {                                           // Pokial su tokeny
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Term
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_FLOAT_DATATYPE &&                          //      
           Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            // TODO: Symtable Check                                         //
        }                                                                   //
        // TODO: Check Param                                                //
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;         // Ciarka
        if(Token->Type == T_TYPE_COMMA && Params->Next != NULL);            //
        else if(Token->Type == T_TYPE_CLOSED_BRACKET &&                     //
                Params->Next == NULL);                                      //
        else return 2;                                                      //
    }                                                                       //
    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Otvorena Mnozinova Zatvorka
    if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;                  //
    if((ERR = Start(Token, Source, Character, Dive + 1)) != 0) return ERR;  //

    return 0;
}

/*
 *  • Analyza syntaxe pre premennu
 *  • Overenie/Deklaracia v Tabulke
 */
int Variable(_TOKEN_ *Token, FILE* Source, int *Character){
    int ERR;
    // TODO: Symtable Check                                                 //
    if((ERR = Scan(Token, Source, Character)) != 0) return ERR;             // Rovna sa
    if(Token->Type != T_TYPE_EQUAL) {                                       //
        // TODO: Analyza vyrazov + Pridat uz nacitanu premennu              // TODO: Odstranit
        while(Token->Type != T_TYPE_SEMICOLON) {                            // TODO: Odstranit
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // TODO: Odstranit
        }                                                                   // TODO: Odstranit
    } else {                                                                //
        // TODO: Analyza vyrazov                                            // TODO: Odstranit
        while(Token->Type != T_TYPE_SEMICOLON) {                            // TODO: Odstranit
            if((ERR = Scan(Token, Source, Character)) != 0) return ERR;     // TODO: Odstranit
        }                                                                   // TODO: Odstranit
    }                                                                       //
    if(Token->Type != T_TYPE_SEMICOLON) return 2;                           //
    
    return 0;
}

/*
 *  Vyhodnotenie zaciatocneho tokenu
 */
int Start(_TOKEN_ *Token, FILE* Source, int *Character, int Dive){
    int ERR;
    int IF = 0;
    while(Token->Type != T_TYPE_EOF) {
        if((ERR = Scan(Token, Source, Character)) != 0) return ERR;

        if(IF == 1 && Token->Keyword != T_KEYWORD_IF) IF = 0;
        else if(IF == 0 && Token->Keyword == T_KEYWORD_IF) IF = 1;
        else if(IF == 0 && Token->Keyword == T_KEYWORD_ELSE) return 2;
        
        if(Token->Type == T_TYPE_KEYWORD) {
            if((ERR = Keyword(Token, Source, Character, Dive)) != 0) return ERR;
        } else if(Token->Type == T_TYPE_FUNCTION) {
            if((ERR = Function(Token, Source, Character, Dive)) != 0) return ERR;
        } else if(Token->Type == T_TYPE_VARIABLE) {
            if((ERR = Variable(Token, Source, Character)) != 0) return ERR;
        } else if(Term(Token->Type) == 0); // TODO: Analyza vyrazov
        else if(Token->Type != T_TYPE_SEMICOLON && Token->Type != T_TYPE_EOF) {
            if(Token->Type != T_TYPE_CLOSED_CURLY_BRACKET) return 2;
            if(Dive == 0) return 2;
            else return 0;
        }
    }

    if(Dive != 0) return 2;

    return 0;
}

/*
 *  Zaciatok programu
 */
int main(){
    _TOKEN_ *Token = T_Create();                                            // Inicializacia
    if(Token == NULL) return 99;                                            // 
    int Character;                                                          //
    
    FILE* Source;                                                           // Vstup
    if(!(Source = fopen("Input.txt", "r"))) Source = stdin;                 //

    int ERR = Prolog(Source, &Character);                                   // Prolog
    if(ERR != 0) return ERR;                                                //

    if((ERR = Start(Token, Source, &Character, 0)) != 0) {                  // Zaciatok Analyzi
        ERR_Handler(ERR, Line, Token_Number);                               //
        return ERR;                                                         //
    }                                                                       //

    free(Token);                                                            // Uvolnenie Pamäte
    Token = NULL;                                                           //

    return 0;                                                               // Ukoncenie
}