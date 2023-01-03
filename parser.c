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
            Token->String = malloc((Length+1)*sizeof(char));
            if(Token->String == NULL) return NULL;
            for(int i=0; i<Length+1; i++) Token->String[i] = '\0';

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
        if(Token->Type == T_TYPE_KEYWORD){                                  //
            if(5 < Token->Type && Token->Type < 17) return 2;               //
            else return 3;                                                  //
        }                                                                   //
        else if(Token->Type != T_TYPE_FUNCTION) return 2;                   //
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
        Wrap->Table->Local = EditVariable(&Wrap->Table->Local, Wrap->Dive); //
        G_Return();                                                         //
    } else if(Token->Keyword == T_KEYWORD_RETURN && Dive != 0) {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             //
        if((ERR = Expression(Wrap, 0)) != 0) {                              // Analyzator
            if(ERR == 100 && Wrap->Table->Type != T_KEYWORD_INT) return 4;  //
            else if(ERR == 200 && Wrap->Table->Type != T_KEYWORD_FLOAT) return 4;
            else if(ERR == 300 && Wrap->Table->Type != T_KEYWORD_STRING) return 4;
        }                                                                   //
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       // Bodkociarka
    } else if(Token->Keyword == T_KEYWORD_WHILE) {
        G_WhileStart();                                                     //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Expression(Wrap, 1)) < 100 && ERR != 0) return ERR;       // Analyzator
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        G_WhileJump(Wrap);                                                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        Wrap->Table->Local = EditVariable(&Wrap->Table->Local, Wrap->Dive); //
        G_WhileEnd(Wrap);                                                   //
    } else if(Token->Keyword == T_KEYWORD_ELSE) {
        G_Else(Wrap);                                                       //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        Wrap->Table->Local = EditVariable(&Wrap->Table->Local, Wrap->Dive); //
        G_IfEnd(Wrap);                                                      //
    } else if(Token->Keyword == T_KEYWORD_IF) {
        G_IfGen();                                                          //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Expression(Wrap, 1)) < 100 && ERR != 0) return ERR;       // Analyzator
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  // Zatvorena Zatvorka
        G_IfStart(Wrap);                                                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Mnozinova Zatvorka
        if(Token->Type != T_TYPE_OPEN_CURLY_BRACKET) return 2;              //
        Wrap->Dive++;                                                       //
        if((ERR = Start(Wrap)) != 0) return ERR;                            //
        Wrap->Dive--;                                                       //
        Wrap->Table->Local = EditVariable(&Wrap->Table->Local, Wrap->Dive); //
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
        Stack_Push(Wrap->Stack, Token);                                     //
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
            Stack_Push(Wrap->Stack, Token);                                 //
        }                                                                   //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);                //
        printf("CREATEFRAME\n");                                            // ! Chybal novy riadok
        printf("CALL func$substring\n");                                    //
    } else if(Token->Keyword == T_KEYWORD_FLOATVAL ||
              Token->Keyword == T_KEYWORD_INTVAL) {
        Token_Keyword TypeTMP = Token->Keyword;
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
        if(TypeTMP == T_KEYWORD_FLOATVAL){                                  //
            Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);            //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$floatval\n");                                 //
        } else if(TypeTMP == T_KEYWORD_INTVAL){                             //
            Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);            //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$intval\n");                                   //
        }                                                                   //
    } else if(Token->Keyword == T_KEYWORD_STRVAL ||
              Token->Keyword == T_KEYWORD_STRLEN ||
              Token->Keyword == T_KEYWORD_ORD) {
        Token_Keyword TypeTMP = Token->Keyword;
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // String
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_NULL_DATATYPE) {                           //
            if(Token->Type != T_TYPE_VARIABLE) return 2;                    //
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if(ItemTMP->Type != T_TYPE_STRING_DATATYPE &&                   // ! T_KEYWORD_STRING -> T_TYPE_STRING_DATATYPE
               ItemTMP->Type != T_TYPE_NULL_DATATYPE) return 4;             // ! T_KEYWORD_NULL -> T_TYPE_NULL_DATATYPE
        }                                                                   //
        Stack_Push(Wrap->Stack, Token);                                     //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        if(TypeTMP == T_KEYWORD_STRVAL){                                    //
            Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);            //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$strval\n");                                   //
        } else if(TypeTMP == T_KEYWORD_STRLEN){                             //
            Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);            //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$strlen\n");                                   //
        } else{                                                             //
            Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);            //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$ord\n");                                      //
        }
    } else if(Token->Keyword == T_KEYWORD_WRITE) {
        int Cnt = 0;                                                        //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Otvorena Zatvorka
        if(Token->Type != T_TYPE_OPEN_BRACKET) return 2;                    //
        while(Token->Type != T_TYPE_CLOSED_BRACKET) {                       // Zatvorena Zatvorka
            if((ERR = Scan(Wrap)) != 0) return ERR;                         // Term
            if(Token->Type == T_TYPE_CLOSED_BRACKET) break;                 //
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
        printf("CREATEFRAME\n");                                            //
        printf("DEFVAR TF@write$0\n");                                      //
        printf("MOVE TF@write$0 int@%d\n", Cnt);                            //
        _STACK_ *TMP = Stack_Create();                                      //
        while(Wrap->Stack->Token.Type != T_TYPE_NULL){                      //
            Stack_Push(TMP, &Wrap->Stack->Token);                           //
            Wrap->Stack = Stack_Pop(Wrap->Stack);                           //
        }                                                                   //
        Wrap->Stack = G_CallParam(TMP, Wrap->Table);                        //
        printf("CALL func$write\n");                                        //
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
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$reads\n");                                    //
        }                                                                   //
        else if(Keyword == T_KEYWORD_READI){                                //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$readi\n");                                    //
        } else{                                                             //
            printf("CREATEFRAME\n");                                        // ! Chybal novy riadok
            printf("CALL func$readf\n");                                    //
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
        Stack_Push(Wrap->Stack, Token);                                     //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type != T_TYPE_CLOSED_BRACKET) return 2;                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Bodkociarka
        if(Token->Type != T_TYPE_SEMICOLON) return 2;                       //
        Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);                //
        printf("CREATEFRAME\n");                                            // ! Chybal novy riadok
        printf("CALL func$chr\n");                                          //
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
        if(Token->Type == T_TYPE_CLOSED_BRACKET) break;                     //
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
       Token->Keyword != T_KEYWORD_VOID &&                                  //
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
    if(!Params){
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Zatvorena Zatvorka
        if(Token->Type == T_TYPE_STRING_DATATYPE ||                         //
           Token->Type == T_TYPE_FLOAT_DATATYPE ||                          //      
           Token->Type == T_TYPE_NULL_DATATYPE ||                           //
           Token->Type == T_TYPE_INT_DATATYPE ||                            //
           Token->Type == T_TYPE_VARIABLE) return 4;                        //
    }
    while(Params) {                                                         // Pokial su tokeny
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Term
        if(Token->Type != T_TYPE_STRING_DATATYPE &&                         //
           Token->Type != T_TYPE_FLOAT_DATATYPE &&                          //      
           Token->Type != T_TYPE_NULL_DATATYPE &&                           //
           Token->Type != T_TYPE_INT_DATATYPE) {                            //
            if(Token->Type != T_TYPE_VARIABLE) {                            //
                if(Token->Type == T_TYPE_CLOSED_BRACKET) return 4;          //
                else return 2;                                              //
            }                                                               //
            _ITEMV_ *ItemTMP = SearchV(&Table->Local, Token->String);       //
            if(ItemTMP == NULL) return 5;                                   //
            if((T_TYPE_STRING_DATATYPE > ItemTMP->Type || ItemTMP->Type > T_TYPE_INT_DATATYPE) &&
                ItemTMP->Type != T_TYPE_VARIABLE) return 4;                 //
            if(Params->Type - 16 != ItemTMP->Type && ItemTMP->Type != T_TYPE_VARIABLE) return 4;
        } else if(Params->Type - 16 != Token->Type) return 4;               //
        Stack_Push(Wrap->Stack, Token);                                     //
        if(Wrap->Stack == NULL) return 99;                                  //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Ciarka/Zatvorka
        if(Token->Type == T_TYPE_COMMA && Params->Next != NULL);            //
        else if(Token->Type == T_TYPE_CLOSED_BRACKET && Params->Next == NULL);
        else if(Params->Next == NULL && Token->Type == T_TYPE_COMMA) {      //
            if((ERR = Scan(Wrap)) != 0) return ERR;                         //
            if(Token->Type == T_TYPE_STRING_DATATYPE ||                     //
               Token->Type == T_TYPE_FLOAT_DATATYPE ||                      //      
               Token->Type == T_TYPE_NULL_DATATYPE ||                       //
               Token->Type == T_TYPE_INT_DATATYPE ||                        //
               Token->Type == T_TYPE_VARIABLE) return 4;                    //
            else return 2;                                                  //
        } else return 2;                                                    //
        Params = Params->Next;                                              //
    }                                                                       //
    Wrap->Stack = G_CallParam(Wrap->Stack, Wrap->Table);                    //
    printf("CREATEFRAME\n");                                                // ! Chybal novy riadok
    printf("CALL func$%s\n", Name);                                         //

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

    if(Wrap->Character == ' ') {
        if((ERR = Scan(Wrap)) != 0) return ERR;                             // Rovna sa
    }
    
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
            if(ERR == 100) InsertV(&Wrap->Table->Local, Name, Type, Wrap->Dive);
            else if(ERR == 200) InsertV(&Wrap->Table->Local, Name, Type, Wrap->Dive);
            else if(ERR == 300) InsertV(&Wrap->Table->Local, Name, Type, Wrap->Dive);
            else return ERR;                                                //
        }                                                                   //
    } else {                                                                //
        if((ERR = Scan(Wrap)) != 0) return ERR;                             //
        _ITEMV_ *TMP3 = SearchV(&Wrap->Table->Local, Name);                 //
        if(Token->Type == T_TYPE_KEYWORD) {                                 //
            Token_Keyword TMP = Token->Keyword;                             // ! Pridany riadok
            if((ERR = Keyword(Wrap)) != 0) return ERR;                      // ! Presunutie z riadku 470
            Token->Keyword = TMP;                                           // ! Pridany riadok
            if(Token->Keyword == T_KEYWORD_CHR ||                           //
               Token->Keyword == T_KEYWORD_STRVAL ||                        //
               Token->Keyword == T_KEYWORD_SUBSTRING ||                     // ! Pridanie spracovania substringu
               Token->Keyword == T_KEYWORD_READS) InsertV(&Wrap->Table->Local, Name, T_TYPE_STRING_DATATYPE, Wrap->Dive);
            else if(Token->Keyword == T_KEYWORD_INTVAL ||                   //
                    Token->Keyword == T_KEYWORD_ORD ||                      //
                    Token->Keyword == T_KEYWORD_READI ||                    //
                    Token->Keyword == T_KEYWORD_STRLEN) InsertV(&Wrap->Table->Local, Name, T_TYPE_INT_DATATYPE, Wrap->Dive);
            else if(Token->Keyword == T_KEYWORD_READF ||                    //
                    Token->Keyword == T_KEYWORD_FLOATVAL) InsertV(&Wrap->Table->Local, Name, T_TYPE_FLOAT_DATATYPE, Wrap->Dive);
            else return 2;                                                  //
            if(TMP3 == NULL) G_RetVal(Name, Wrap->Dive);                    //
            else printf("POPS LF@%s$%i\n", TMP3->Name, TMP3->Dive);
            return 0;                                                       //
        } else if(Token->Type == T_TYPE_FUNCTION) {                         //
            _ITEMF_ *TMP = SearchF(&Wrap->Table->Root, Token->String);      //
            if((ERR = Function(Wrap)) != 0) return ERR;                     //
            InsertV(&Wrap->Table->Local, Name, TMP->Type - 16, Wrap->Dive); //
            if(TMP3 == NULL) G_RetVal(Name, Wrap->Dive);                    //
            else printf("POPS LF@%s$%i\n", TMP3->Name, TMP3->Dive);         //
        } else if((ERR = Expression(Wrap, 0)) != 0) {                       // Analyzator
            if(TMP3 == NULL) {                                              //
                if(ERR == 100) InsertV(&Wrap->Table->Local, Name, T_TYPE_INT_DATATYPE, Wrap->Dive);
                else if(ERR == 200) InsertV(&Wrap->Table->Local, Name, T_TYPE_FLOAT_DATATYPE, Wrap->Dive);
                else if(ERR == 300) InsertV(&Wrap->Table->Local, Name, T_TYPE_STRING_DATATYPE, Wrap->Dive);
                else if(ERR == 400) InsertV(&Wrap->Table->Local, Name, T_TYPE_NULL_DATATYPE, Wrap->Dive);
                else return ERR;                                            //
                G_RetVal(Name, Wrap->Dive);                                 //
            } else {                                                        //
                if(ERR == 100) InsertV(&Wrap->Table->Local, Name, T_TYPE_INT_DATATYPE, TMP3->Dive);
                else if(ERR == 200) InsertV(&Wrap->Table->Local, Name, T_TYPE_FLOAT_DATATYPE, TMP3->Dive);
                else if(ERR == 300) InsertV(&Wrap->Table->Local, Name, T_TYPE_STRING_DATATYPE, TMP3->Dive);
                else if(ERR == 400) InsertV(&Wrap->Table->Local, Name, T_TYPE_NULL_DATATYPE, Wrap->Dive);
                else return ERR;                                            //
                printf("POPS LF@%s$%i\n", TMP3->Name, TMP3->Dive);          //
            }                                                               //
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
    Wrap->Source = stdin;

    if(!(Wrap->Source = fopen("qwfdsdafv.txt", "r"))) Wrap->Source = stdin; // ! Odstranit

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