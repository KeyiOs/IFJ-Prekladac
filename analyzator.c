/**
 * IFJ Projekt 2022
 * @authors <xpocho06> Marek Pochop, <xkento00> Samuel Kentos
 */
#include "analyzator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 *  • Vrati uroven precedencie tokenu
 */
Term_Type Get_Precedence(Token_Type Type) {
    switch(Type){
        case T_TYPE_PLUS:
        case T_TYPE_MINUS:
        case T_TYPE_CONCATENATION:
            return T_ADDSUBCON;             // + - .
        case T_TYPE_MULTIPLICATION:
        case T_TYPE_DIVISION:
            return T_DIVMUL;                // / *
        case T_TYPE_TRIPLE_EQUALS:
        case T_TYPE_TRIPLE_EQUALS_NEG:
            return T_EQ;                    // === !==
        case T_TYPE_GREATER:
        case T_TYPE_GREATER_EQUAL:
        case T_TYPE_SMALLER:
        case T_TYPE_SMALLER_EQUAL:
            return T_SMGT;                  // > < >= <=
        case T_TYPE_OPEN_BRACKET:
            return T_LB;                    // (
        case T_TYPE_CLOSED_BRACKET:
            return T_RB;                    // )
        case T_TYPE_VARIABLE:
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_FLOAT_DATATYPE:
        case T_TYPE_INT_DATATYPE:
        case T_TYPE_NULL_DATATYPE:
            return T_VAL;                   // Ciselna hodnota, Premenna
        default:
            return T_DOLLAR;                // Znak mimo analyzu
    }
}

/*
 *  • Urcuje vstah medzi terminalom na staku a typom vstupneho tokenu
 */
Rel_Type Relation(Term_Type Stack, Term_Type Entry) {
    switch(Stack){
        case T_ADDSUBCON:
            if(Entry == T_DIVMUL || Entry == T_VAL || Entry == T_LB) return StackPush;
            else return StackProcess;
        case T_DIVMUL:
            if(Entry == T_VAL || Entry == T_LB) return StackPush;
            else return StackProcess;
        case T_SMGT:
            if(Entry == T_ADDSUBCON || Entry == T_DIVMUL || Entry == T_VAL || Entry == T_LB) return StackPush;
            else return StackProcess;
        case T_EQ:
            if(Entry == T_EQ || Entry == T_RB || Entry == T_DOLLAR) return StackProcess;
            else return StackPush;
        case T_LB:
            if(Entry == T_DOLLAR) return Finish;
            else if(Entry == T_RB) return LR_Brackets;
            else return StackPush;
        case T_RB:
            if(Entry == T_LB || Entry == T_VAL) return Finish;
            else return StackProcess;
        case T_DOLLAR:
            if(Entry == T_RB || Entry == T_DOLLAR) return Finish;
            else return StackPush;
    }

    return Finish;
}

/*
 *  • Vyhladanie najnovsieho terminalu na staku
 */
Term_Type Get_Terminal(_STACK_ *Stack) {
    _STACK_ Stack_Tmp = *Stack;
    Term_Type Type;

    while(Stack_Tmp.Token.Type != T_TYPE_NULL){
        Type = Get_Precedence(Stack_Tmp.Token.Type);
        if(Type >= T_ADDSUBCON && Type <= T_RB) return Type;
        if(Stack_Tmp.Previous == NULL) break;
        Stack_Tmp = *Stack_Tmp.Previous;
    }

    return T_DOLLAR;
}

/*
 *  • Hlavna funkcia analyzi
 */
int Expression(_WRAP_ *Wrap, int Condition) {
    int ERR = 0;                        // Navratova hodnota
    int Return = 0;                     // int 100, float 200, string 300
    int Brackets = 0;                   // Pocet zatvoriek
    _TOKEN_ TokenTMP;                   // Pre docasne ulozenie tokenu
    _STACK_ *Stack = Stack_Create();    // Skratka ku staku
    Term_Type StackTerminal;            // Typ posledneho terminalu na staku
    Term_Type CurrentT_Type;            // Typ noveho tokenu

    if(Condition == 1) { if((ERR = Scan(Wrap)) != 0) return ERR; }  // Preskocenie prvej zatvorky

    while(1){
        StackTerminal = Get_Terminal(Stack);                // Ulozi posledny terminal zo staku
        CurrentT_Type = Get_Precedence(Wrap->Token->Type);  // Ulozi typ aktualneho tokenu

        if(Wrap->Token->Type == T_TYPE_VARIABLE){
            _ITEMV_ *ItemTMP = SearchV(&Wrap->Table->Local, Wrap->Token->String);
            if(ItemTMP == NULL) return 5;
        }

        if(CurrentT_Type == T_RB && Brackets != 0 && (Get_Precedence(Stack->Token.Type) == T_LB || Get_Precedence(Stack->Token.Type) == T_VAL)) Brackets--;   // Znizi pocet zatvoriek v pripade validneho zapisu
        else if(CurrentT_Type == T_VAL || CurrentT_Type == T_LB) {
            if(Get_Precedence(Stack->Token.Type) == T_VAL) return 2;    // Vrati chybu ak dostaneme 'x' alebo '(' a na zasobniku mame 'x'
            else if(CurrentT_Type == T_LB) Brackets++;                  // Inkrementuje pocet zatvoriek ak dostaneme '('
        } else if((CurrentT_Type == T_SMGT || CurrentT_Type == T_EQ) && Condition == 0) return 2;    // Aby nebyly vyrazy typu: x = 5 < 10;

        if(Get_Precedence(Stack->Token.Type) != T_VAL && CurrentT_Type != T_VAL && CurrentT_Type != T_LB) return 2;  // Aby sme nedostali 2 terminaly v rade
        
        if(CurrentT_Type == T_VAL && Wrap->Token->Type != T_TYPE_VARIABLE){     // Zistenie datoveho typu vyrazu
            if(Wrap->Token->Type == T_TYPE_INT_DATATYPE) {
                if(Return < 100) Return = 100;                                  // Do premennej priradime po skonceni INT
                else if(Return == 300) return 7;                                // Chyba typovej kompatibility
            } else if(Wrap->Token->Type == T_TYPE_FLOAT_DATATYPE){
                if(Return < 200) Return = 200;                                  // Do premennej priradime po skonceni FLOAT
                else if(Return == 300) return 7;                                // Chyba typovej kompatibility
            } else if(Wrap->Token->Type == T_TYPE_STRING_DATATYPE){
                if(Return < 100) Return = 300;                                  // Do premennej priradime po skonceni STRING
                else if(Return < 300) return 7;                                 // Chyba typovej kompatibility
            } else if(Wrap->Token->Type == T_TYPE_NULL_DATATYPE);
        } else if(CurrentT_Type == T_VAL) {
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Wrap->Token->String);
            if(TMP->Type == T_TYPE_INT_DATATYPE) {
                if(Return < 100) Return = 100;                                  // Do premennej priradime po skonceni INT
                else if(Return == 300) return 7;                                // Chyba typovej kompatibility
            } else if(TMP->Type == T_TYPE_FLOAT_DATATYPE){
                if(Return < 200) Return = 200;                                  // Do premennej priradime po skonceni FLOAT
                else if(Return == 300) return 7;                                // Chyba typovej kompatibility
            } else if(TMP->Type == T_TYPE_STRING_DATATYPE){
                if(Return < 100) Return = 300;                                  // Do premennej priradime po skonceni STRING
                else if(Return < 300) return 7;                                 // Chyba typovej kompatibility
            } else if(TMP->Type == T_TYPE_NULL_DATATYPE);
        }

        if(Return == 300 && (CurrentT_Type < T_EQ && Wrap->Token->Type != T_TYPE_CONCATENATION)) return 2;  // Neplatna operacia pre stringy
        else if(Return < 300 && Wrap->Token->Type == T_TYPE_CONCATENATION) return 2;    // Nemame STRING ale dostali sme CONCAT

        switch(Relation(StackTerminal, CurrentT_Type)){   // Vyhodnoty vztah medzi terminalmi
            case StackPush:
                Stack_Push(Stack, Wrap->Token);
                if((ERR = Scan(Wrap)) != 0) return ERR;
                break;
            case StackProcess:
                if(Get_Precedence(Stack->Token.Type) == T_VAL){     // Vytvorenie nahradneho tokenu pre spracovanu cast vyrazu
                    TokenTMP.Keyword = T_KEYWORD_EMPTY;
                    if(Return < 300) TokenTMP.Type = T_TYPE_FLOAT_DATATYPE;
                    else TokenTMP.Type = T_TYPE_STRING_DATATYPE;
                    TokenTMP.String = "$";
                } else return 2;
                _TOKEN_ Val1, Val2;
                int String = 0;
                _TOKEN_ Symbol;
                for(int i = 0; i < 3; i++) {
                    if(i == 0) Val2 = Stack->Token;
                    else if(i == 1) Symbol = Stack->Token;
                    else if(i == 2) Val1 = Stack->Token;
                    Stack = Stack_Pop(Stack);
                }
                Stack_Push(Stack, &TokenTMP);   // Vlozenie nahradneho tokenu na stak
                if(Symbol.Type == T_TYPE_PLUS) G_ADD(Wrap, Val1, Val2);
                else if(Symbol.Type == T_TYPE_MINUS) G_SUB(Wrap, Val1, Val2);
                else if(Symbol.Type == T_TYPE_CONCATENATION) G_CON(Wrap, Val1, Val2);
                else if(Symbol.Type == T_TYPE_MULTIPLICATION) G_MUL(Wrap, Val1, Val2);
                else if(Symbol.Type == T_TYPE_DIVISION) G_DIV(Wrap, Val1, Val2);
                else if(Symbol.Type == T_TYPE_TRIPLE_EQUALS) {
                    if(Return == 300) G_EQ(Wrap, 1, Val1, Val2);
                    else G_EQ(Wrap, 1, Val1, Val2);
                } else if(Symbol.Type == T_TYPE_TRIPLE_EQUALS_NEG) {
                    if(Return == 300) G_EQ(Wrap, 0, Val1, Val2);
                    else G_EQ(Wrap, 0, Val1, Val2);
                } else if(Symbol.Type == T_TYPE_SMALLER) {
                    if(Return == 300) G_SM(Wrap, 0, Val1, Val2);
                    else G_SM(Wrap, 0, Val1, Val2);
                } else if(Symbol.Type == T_TYPE_GREATER) {
                    if(Return == 300) G_GT(Wrap, 0, Val1, Val2);
                    else G_GT(Wrap, 0, Val1, Val2);
                } else if(Symbol.Type == T_TYPE_SMALLER_EQUAL) {
                    if(Return == 300) G_SM(Wrap, 1, Val1, Val2);
                    else G_SM(Wrap, 1, Val1, Val2);
                } else if(Symbol.Type == T_TYPE_GREATER_EQUAL) {
                    if(Return == 300) G_GT(Wrap, 1, Val1, Val2);
                    else G_GT(Wrap, 1, Val1, Val2);
                }
                break;
            case LR_Brackets:   // Pro ('x')
                if(Get_Precedence(Stack->Token.Type) == T_LB &&                                 // Dostali sme ()
                   Get_Precedence(Wrap->Token->Type) == T_RB) return 2;                         //
                if(Get_Precedence(Stack->Token.Type) == T_VAL) TokenTMP = Stack->Token;         // Dostali sme ('x')
                for(int i = 0; i < 2; i++) Stack = Stack_Pop(Stack);                            // ('x') -> 'x'
                Stack_Push(Stack, &TokenTMP);                                                   //
                if((ERR = Scan(Wrap)) != 0) return ERR;
                break;
            case Finish:
                if(StackTerminal == T_DOLLAR && CurrentT_Type == T_DOLLAR){
                    if(strcmp(Stack->Token.String, "$") != 0){
                        if(Stack->Token.Type == T_TYPE_INT_DATATYPE) printf("PUSHS int$%s\n", Stack->Token.String);
                        else if(Stack->Token.Type == T_TYPE_FLOAT_DATATYPE) printf("PUSHS float$%s\n", Stack->Token.String);
                        else if(Stack->Token.Type == T_TYPE_STRING_DATATYPE) printf("PUSHS string$%s\n", Stack->Token.String);
                        else if(Stack->Token.Type == T_TYPE_VARIABLE){
                            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Stack->Token.String);
                            printf("PUSHS LF$%s$%i\n", Stack->Token.String, TMP->Dive);
                        }
                    }
                    if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return Return;        // Spravne se doslo az na konec vyrazu
                    else if(Get_Precedence(Stack->Token.Type) == T_VAL)  {                      // Pokud zustala jen 1 promenna tak pop a return ze spravne
                        printf("POPS LF@retval$1\n");                                           // Ulozenie vysledku pre zapisanie do premennej
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return Return;    // Spravne se doslo az na konec vyrazu
                        else return 2;                                                          // Zly pocet zatvoriek
                    } else return 2;
                } else if(StackTerminal == T_DOLLAR && CurrentT_Type == T_RB && Condition == 1) return Return;
                else return 2;
                break;
        }
    }

    return Return;
}