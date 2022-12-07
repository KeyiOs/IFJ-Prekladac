/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentos
 */

#include "error_handler.h"
#include "skener.h"
#include "parser.h"
#include "symtable.h"
#include "generator.h"
#include "stack.h"
#include "analyzator.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

int Line = 1;

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

    switch(Type){
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_FLOAT_DATATYPE:
        case T_TYPE_INT_DATATYPE:
        case T_TYPE_VARIABLE:
        case T_TYPE_FUNCTION: {
            int Length = strlen(String);

            Token->String = NULL;
            Token->String = (char*) malloc(Length*sizeof(char));
            if(Token->String == NULL) return NULL;
            for(int i=0; i<Length; i++) Token->String[i] = '\0';

            strcpy(Token->String,String);
            break;
        }
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
int Term(Token_Type Type){
    if(Type == T_TYPE_STRING_DATATYPE ||
       Type == T_TYPE_FLOAT_DATATYPE ||
       Type == T_TYPE_NULL ||
       Type == T_TYPE_INT_DATATYPE) return 0;

    return 1;
}

/*
 *  • Dealokuje vsetky dynamicke premenne
 */
void End(_WRAP_ *Wrap){
    if(Wrap->Source != NULL) fclose(Wrap->Source);
    free(Wrap->Stack);
    free(Wrap->Token);
    Wrap->Stack = NULL;
    Wrap->Token = NULL;
    Wrap->Table = FreeST(Wrap->Table);
    free(Wrap);
    Wrap = NULL;
}

/*
 *  • Analyza syntaxe pre klucove slovo
 */
int Keyword(_WRAP_ *Wrap){
    int ERR;                                                                // Deklaracia
    _TOKEN_ *Token = Wrap->Token;                                           //
    _ITEMF_ *Table = Wrap->Table;                                           //
    FILE* Source = Wrap->Source;                                            //
    int Character = Wrap->Character;                                        //
    int Dive = Wrap->Dive;                                                  //

    if(Token->Keyword == T_KEYWORD_FUNCTION && Dive == 0) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Meno Funkcie
        if(Token->Type != T_TYPE_FUNCTION) return 2;                        //
        char *Name = Token->String;                                         //
        if(SearchF(&Table, Name) != NULL) return 3;                         // Overenie Funkcie
        if((ERR = F_Declare(Wrap)) != 0) return ERR;                        //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Table = SearchF(&Table, Name);                                //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        Wrap->Table = Wrap->Table->Root;                                    //
        G_Return();                                                         //
    } else if(Token->Keyword == T_KEYWORD_RETURN && Dive != 0) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             //
        printf("MOVE LF@retval$1 LF@%s$%d \n", Wrap->Token->String, Wrap->Dive);
        if((ERR = Expression(Wrap, 0)) != 0) {                              // Analyzator
            if(ERR == 100 && Wrap->Table->Type != T_KEYWORD_INT) return 4;  //
            else if(ERR == 200 && Wrap->Table->Type != T_KEYWORD_FLOAT) return 4;
            else if(ERR == 300 && Wrap->Table->Type != T_KEYWORD_STRING) return 4;
        }                                                                   //
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       // Bodkociarka
        G_Return();                                                         //
    } else if(Token->Keyword == T_KEYWORD_WHILE) {
        G_WhileStart(Wrap->Dive);                                           //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Expression(Wrap, 1)) < 100 && ERR != 0) return ERR;       // Analyzator
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        G_WhileJump(Wrap->Dive);                                            //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        G_WhileEnd();                                                       //
    } else if(Token->Keyword == T_KEYWORD_ELSE) {
        G_Else();                                                           //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        G_IfEnd();                                                          //
    } else if(Token->Keyword == T_KEYWORD_IF) {
        G_IfGen(Wrap->Dive);                                                //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Expression(Wrap, 1)) < 100 && ERR != 0) return ERR;       // Analyzator
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        G_IfStart(Wrap->Dive);                                              //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
    } else if(Token->Keyword == T_KEYWORD_SUBSTRING) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // String
        if(Token->Type != T_TYPE_STRING_DATATYPE) {                         //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    // 
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if(ItemTMP->Type != T_TYPE_STRING_DATATYPE) return 4;           //
        }                                                                   //
        G_Stack_Push(Wrap->Stack, Token);                                   //
        for(int i = 0; i < 2; i++) {                                        //
            if((ERR = Scan(Wrap)) != 0) return ERR;                         // Ciarka
            if(Token->Type != T_TYPE_COMMA) return 2;                       //
            if((ERR = Scan(Wrap)) != 0) return ERR;                         // Int
            if(Token->Type != T_TYPE_INT_DATATYPE) {                        //
                if(Token->Type != T_TYPE_VARIABLE) return 2;                //
                _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);   //
                if(ItemTMP == NULL) return 5;                               //
                if(ItemTMP->Type != T_TYPE_INT_DATATYPE) return 4;          //
            }                                                               //
            G_Stack_Push(Wrap->Stack, Token);                               //
        }                                                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        G_CallStart("substring", 0);                                        //
        G_CallParam(Wrap->Stack, Wrap->Table);                              //
        G_Call("substring");                                                //
    } else if(Token->Keyword == T_KEYWORD_FLOATVAL ||
              Token->Keyword == T_KEYWORD_INTVAL) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Number
        if(Token->Type != T_TYPE_FLOAT_DATATYPE &&                          //
           Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if(ItemTMP->Type != T_TYPE_FLOAT_DATATYPE &&                    //
               ItemTMP->Type != T_TYPE_NULL_DATATYPE &&                     //
               ItemTMP->Type != T_TYPE_INT_DATATYPE) return 4;              //
        }                                                                   //
        G_Stack_Push(Wrap->Stack, Token);                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        if(Token->Keyword == T_KEYWORD_FLOATVAL){                           //
            G_CallStart("floatval", 0);                                     //
            G_CallParam(Wrap->Stack, Wrap->Table);                          //
            G_Call("floatval");                                             //
        } else if(Token->Keyword == T_KEYWORD_INTVAL){                      //
            G_CallStart("intval", 0);                                       //
            G_CallParam(Wrap->Stack, Wrap->Table);                          //
            G_Call("intval");                                               //
        }                                                                   //
    } else if(Token->Keyword == T_KEYWORD_STRVAL ||
              Token->Keyword == T_KEYWORD_STRLEN ||
              Token->Keyword == T_KEYWORD_ORD) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // String
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_NULL_DATATYPE) {                           //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if(ItemTMP->Type != T_KEYWORD_STRING &&                         //
               ItemTMP->Type != T_KEYWORD_NULL) return 4;                   //
        }                                                                   //
        G_Stack_Push(Wrap->Stack, Token);                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        if(Token->Keyword == T_KEYWORD_STRVAL){                             //
            G_CallStart("strval", 0);                                       //
            G_CallParam(Wrap->Stack, Wrap->Table);                          //
            G_Call("strval");                                               //
        } else if(Token->Keyword == T_KEYWORD_STRLEN){                      //
            G_CallStart("strlen", 0);                                       //
            G_CallParam(Wrap->Stack, Wrap->Table);                          //
            G_Call("strlen");                                               //
        } else{                                                             //
            G_CallStart("ord", 0);                                          //
            G_CallParam(Wrap->Stack, Wrap->Table);                          //
            G_Call("ord");                                                  //
        }
    } else if(Token->Keyword == T_KEYWORD_WRITE) {
        int Cnt = 0;                                                        //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        while(Token->Type != T_TYPE_CLOSED_BRACKET) {                       // Zatvorena Zatvorka
            if((ERR = Scan(Wrap)) != 0) return ERR;                         // Term
            if(Token->Type != T_TYPE_STRING_DATATYPE &&                     //
               Token->Type != T_TYPE_FLOAT_DATATYPE &&                      //      
               Token->Type != T_TYPE_NULL_DATATYPE &&                       //
               Token->Type != T_TYPE_INT_DATATYPE) {                        //
                if(Token->Type != T_TYPE_VARIABLE) return 2;                //
                _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);   //
                if(ItemTMP == NULL) return 5;                               //
                if(1 > ItemTMP->Type || ItemTMP->Type > 4) return 4;        //
            }                                                               //
            G_Stack_Push(Wrap->Stack, Token);                               //
            if((ERR = Scan(Wrap)) != 0) return ERR;                         // Ciarka
            if(Token->Type != T_TYPE_COMMA &&                               //
               Token->Type != T_TYPE_CLOSED_BRACKET) return 2;              //
            Cnt++;                                                          //
        }                                                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        G_CallStart("write", Cnt);                                          //
        Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);                //
        G_Call("write");                                                    //
    } else if(Token->Keyword == T_KEYWORD_READS ||
              Token->Keyword == T_KEYWORD_READI ||
              Token->Keyword == T_KEYWORD_READF) {
        Token_Keyword Keyword = Token->Keyword;                             // Ulozenie klucoveho slova
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        if(Keyword == T_KEYWORD_READS){                                     //
            G_CallStart("reads", 0);                                        //
            G_Call("reads");                                                //
        }                                                                   //
        else if(Keyword == T_KEYWORD_READI){                                //
            G_CallStart("readi", 0);                                        //
            G_Call("readi");                                                //
        }                                                                   //
        else{                                                               //
            G_CallStart("readf", 0);                                        //
            G_Call("readf");                                                //
        }                                                                   //
    } else if(Token->Keyword == T_KEYWORD_CHR) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Int
        if(Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if(ItemTMP->Type != T_TYPE_NULL_DATATYPE &&                     //
               ItemTMP->Type != T_TYPE_INT_DATATYPE) return 4;              //
        }                                                                   //
        G_Stack_Push(Wrap->Stack, Token);                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        G_CallStart("chr", 0);                                              //
        G_CallParam(Wrap->Stack, Wrap->Table);                              //
        G_Call("chr");                                                      //
    } else return 2;                                                        //

    return 0;
}

/*
 *  • Deklaracia funkcie
 */
int F_Declare(_WRAP_ *Wrap){
    int ERR;                                                                // Deklaracia
    _TOKEN_ *Token = Wrap->Token;                                           //
    _ITEMF_ *Table = Wrap->Table;                                           //
    char *Name = Token->String;                                             //
    if((ERR = InsertF(&Table, &Table, Token->String)) != 0) return ERR;     //
    G_StartFunction(Name);                                                  //
    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Otvorena Zatvorka
    if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                        //
    while(Token->Type != T_TYPE_CLOSED_BRACKET) {                           // Zatvorena Zatvorka
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Datovy Typ
        Token_Keyword Type = Token->Keyword;                                //
        if(Token->Type != T_TYPE_KEYWORD) return 2;                         //
        if(Token->Keyword != T_KEYWORD_STRING &&                            //
           Token->Keyword != T_KEYWORD_FLOAT &&                             //
           Token->Keyword != T_KEYWORD_INT) return 2;                       //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Premenna
        if(Token->Type != T_TYPE_VARIABLE) return 2;                        //
        if((ERR = InsertParam(Name, Token->String, Type, &Table)) != 0) return ERR;
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Ciarka/Zatvorka
        if(Token->Type != T_TYPE_COMMA &&                                   //
           Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
    }                                                                       //
    _ITEMF_ *Function = SearchF(&Wrap->Table, Name);                        //
    G_Param(Function->Params);                                              //
    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Dvojbodka
    if(Token->Type != T_TYPE_COLON) return 2;                               //
    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Datovy Typ
    if(Token->Type != T_TYPE_KEYWORD) return 2;                             //
    if(Token->Keyword != T_KEYWORD_STRING &&                                //
       Token->Keyword != T_KEYWORD_FLOAT &&                                 //
       Token->Keyword != T_KEYWORD_INT) return 2;                           //
    if((ERR = ReturnF(&Wrap->Table, Name, Wrap->Token->Keyword)) != 0) return 99;

    return 0;
}

/*
 *  • Analyza syntaxe pre funkciu
 *  • Overenie v Tabulke
 */
int Function(_WRAP_ *Wrap){
    int ERR;                                                                // Deklaracia
    _TOKEN_ *Token = Wrap->Token;                                           //
    _ITEMF_ *Table = Wrap->Table;                                           //
    char *Name = Token->String;                                             //
    _ITEMF_ *FunctionTMP = SearchF(&Table->Root, Name);                     //
    if(FunctionTMP == NULL) return 3;                                       //
    _PARAM_ *Params = FunctionTMP->Params;                                  //

    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Otovrena Zatvorka
    if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                        //
    while(Params) {                                                         // Pokial su tokeny
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Term
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_FLOAT_DATATYPE &&                          //      
           Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    // Premenna
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if((T_TYPE_STRING_DATATYPE > ItemTMP->Type || ItemTMP->Type > T_TYPE_INT_DATATYPE) &&
                ItemTMP->Type != T_TYPE_VARIABLE) return 4;                 //
            if(Params->Type - 16 != ItemTMP->Type && ItemTMP->Type != T_TYPE_VARIABLE) return 4;
        } else if(Params->Type - 16 != Token->Type) return 4;               //
        G_Stack_Push(Wrap->Stack, Token);                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Ciarka/Zatvorka
        if(Token->Type == T_TYPE_COMMA && Params->Next != NULL);            //
        else if(Token->Type == T_TYPE_CLOSED_BRACKET &&                     //
                Params->Next == NULL);                                      //
        else return 2;                                                      //
        Params = Params->Next;                                              //
    }                                                                       //
    G_CallStart(Name, 0);                                                   //
    G_CallParam(Wrap->Stack, Wrap->Table);                                  //
    G_Call(Name);                                                           //
    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Bodkociarka
    if(Token->Type != T_TYPE_SEMICOLON) return 2;                           //

    return 0;
}

/*
 *  • Analyza syntaxe pre premennu
 *  • Overenie/Deklaracia v Tabulke
 */
int Variable(_WRAP_ *Wrap){
    int ERR;                                                                // Deklaracia
    _TOKEN_ *Token = Wrap->Token;                                           //
    char *Name = Token->String;                                             //
    Token_Type Type = Token->Type;                                          //

    if((ERR = Scan(Wrap)) != 0) return ERR;                                 // Rovna sa
    if(Token->Type != T_TYPE_EQUAL) {                                       //
        int Len = -3;                                                       //
        if(Token->Type == T_TYPE_TRIPLE_EQUALS_NEG || Token->Type == T_TYPE_TRIPLE_EQUALS) Len = -5;
        else if(Token->Type == T_TYPE_SMALLER_EQUAL || Token->Type == T_TYPE_GREATER_EQUAL) Len = -4;
        fseek(Wrap->Source, Len, SEEK_CUR);                                 //
        Wrap->Character = getc(Wrap->Source);                               //
        while(Wrap->Character == ' ' || Wrap->Character == 10 || Wrap->Character == 13) {
            fseek(Wrap->Source, -2, SEEK_CUR);                              //
            Wrap->Character = getc(Wrap->Source);                           //
        }                                                                   //
        Len = -1 * strlen(Name);                                            //
        fseek(Wrap->Source, Len-2, SEEK_CUR);                               //
        Wrap->Character = getc(Wrap->Source);                               //

        if((ERR = Scan(Wrap)) != 0) return ERR;                             //
        if((ERR = Expression(Wrap, 0)) != 0) {                              // Analyzator
            if(ERR == 100) InsertV(&Wrap->Table->Local, Name, Type);        //
            else if(ERR == 200) InsertV(&Wrap->Table->Local, Name, Type);   //
            else if(ERR == 300) InsertV(&Wrap->Table->Local, Name, Type);   //
            else return ERR;                                                //
        }                                                                   //
    } else {                                                                //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             //
        if(Token->Type == T_TYPE_KEYWORD) {                                 //
            if(Token->Keyword == T_KEYWORD_CHR ||                           //
               Token->Keyword == T_KEYWORD_STRVAL ||                        //
               Token->Keyword == T_KEYWORD_READS) InsertV(&Wrap->Table->Local, Name, T_TYPE_STRING_DATATYPE);
            else if(Token->Keyword == T_KEYWORD_INTVAL ||                   //
                    Token->Keyword == T_KEYWORD_ORD ||                      //
                    Token->Keyword == T_KEYWORD_READI ||                    //
                    Token->Keyword == T_KEYWORD_STRLEN) InsertV(&Wrap->Table->Local, Name, T_TYPE_INT_DATATYPE);
            else if(Token->Keyword == T_KEYWORD_READF ||                    //
                    Token->Keyword == T_KEYWORD_FLOATVAL) InsertV(&Wrap->Table->Local, Name, T_TYPE_FLOAT_DATATYPE);
            else return 2;                                                  //
            printf("DEFVAR LF@%s$%d\n", Name, Wrap->Dive);                  //
            if((ERR = Keyword(Wrap)) != 0) return ERR;                      //
            printf("MOVE LF@%s$%d TF@retval$1\n", Name, Wrap->Dive);        //
            return 0;                                                       //
        } else if(Token->Type == T_TYPE_FUNCTION) {                         //
            printf("DEFVAR LF@%s$%d\n", Name, Wrap->Dive);                  //
            _ITEMF_ *TMP = SearchF(&Wrap->Table->Root, Token->String);      //
            if((ERR = Function(Wrap)) != 0) return ERR;                     //
            if(TMP->Type == T_KEYWORD_NULL);
            InsertV(&Wrap->Table->Local, Name, TMP->Type - 16);             //
            printf("MOVE LF@%s$%d TF@retval$1\n", Name, Wrap->Dive);        //
        } else if((ERR = Expression(Wrap, 0)) != 0) {                       // Analyzator
            printf("DEFVAR LF@%s$%i\n", Name, Wrap->Dive);                  //
            printf("MOVE LF@%s TF@retval$1\n", Name);                       //
            if(ERR == 100) InsertV(&Wrap->Table->Local, Name, T_TYPE_INT_DATATYPE);
            else if(ERR == 200) InsertV(&Wrap->Table->Local, Name, T_TYPE_FLOAT_DATATYPE);
            else if(ERR == 300) InsertV(&Wrap->Table->Local, Name, T_TYPE_STRING_DATATYPE);
            else return ERR;                                                //
        }                                                                   //
    }                                                                       //
    if(Token->Type != T_TYPE_SEMICOLON) return 2;                           //

    return 0;
}

/*
 *  Vyhodnotenie zaciatocneho tokenu
 */
int Start(_WRAP_ *Wrap){
    int ERR;                                                                // Deklaracia
    int IF = 0;                                                             //
    _TOKEN_ *Token = Wrap->Token;                                           //

    while(Token->Type != T_TYPE_EOF) {                                      // Spracuje Zaciatocne Tokeny
        if((ERR = Scan(Wrap)) != 0) return ERR;                             //

        if(IF == 1 && Token->Keyword != T_KEYWORD_IF) IF = 0;               // Overenie IF
        else if(IF == 0 && Token->Keyword == T_KEYWORD_IF) IF = 1;          //
        else if(IF == 0 && Token->Keyword == T_KEYWORD_ELSE) return 2;      //
        
        if(Token->Type == T_TYPE_KEYWORD) {                                 // Keywordy
            if((ERR = Keyword(Wrap)) != 0) return ERR;                      //
        } else if(Token->Type == T_TYPE_FUNCTION) {                         // Volanie Funkcie
            if((ERR = Function(Wrap)) != 0) return ERR;                     //
        } else if(Token->Type == T_TYPE_VARIABLE) {                         // Premenna
            if((ERR = Variable(Wrap)) != 0) return ERR;                     //
        } else if(Term(Token->Type) == 0) {                                 // Vyraz
            if((ERR = Expression(Wrap, 0)) < 100 && ERR != 0) return ERR;   //
        }                                                                   //
        else if(Token->Type != T_TYPE_SEMICOLON && Token->Type != T_TYPE_EOF) {
            if(Token->Type != T_TYPE_CLOSED_CURLY_BRACKET) return 2;        // Ukoncenie Vnorenia
            if(Wrap->Dive == 0) return 2;                                   // Overenie Vnorenia
            else return 0;                                                  //
        }                                                                   //
    }                                                                       //

    if(Wrap->Dive != 0) return 2;

    return 0;
}

/*
 *  Zaciatok programu
 */
int main(){
    _WRAP_ *Wrap = malloc(sizeof(_WRAP_));                                  // Inicializacia
    if(Wrap == NULL){                                                       // - Struktura na parametre
        ERR_Handler(99, Line);                                              //
        return 99;                                                          //
    }                                                                       //
    Wrap->Token = T_Create();                                               // Inicializacia
    if(Wrap->Token == NULL){                                                // - Token
        ERR_Handler(99, Line);                                              //
        return 99;                                                          //
    }                                                                       //
    _ITEMF_ *Table = NULL;                                                  // - Tabulka
    Wrap->Table = InitF(&Table, &Table, "main");                            //
    if(Wrap->Table == NULL) {                                               //
        ERR_Handler(99, Line);                                              //
        return 99;                                                          //
    }                                                                       //
    Wrap->Stack = Stack_Create(Wrap->Stack);                                //
    if(Wrap->Stack == NULL) {                                               //
        ERR_Handler(99, Line);                                              //
        return 99;                                                          //
    }                                                                       //
    Wrap->Character = 0;                                                    //
    Wrap->Dive = 0;                                                         //

    Wrap->Source;                                                           // Vstup
    if(!(Wrap->Source = fopen("Input.txt", "r"))) Wrap->Source = stdin;     // !REMOVE
    //Wrap->Source = stdin;
    
    int ERR = Prolog(Wrap);                                                 // Prolog
    if(ERR != 0) {                                                          //
        ERR_Handler(ERR, Line);                                             //
        return ERR;                                                         //
    }                                                                       //

    G_BigStart();                                                           // Zaciatok Generatora
    if((ERR = Start(Wrap)) != 0) {                                          // Zaciatok Analyzi
        ERR_Handler(ERR, Line);                                             //
        return ERR;                                                         //
    }                                                                       //

    G_EndFunction("main\n");                                                // Ukoncenie Generatora
    G_BigEnd();                                                             //
    End(Wrap);                                                              // Dealokovanie Pamäte

    return 0;                                                               // Ukoncenie
}