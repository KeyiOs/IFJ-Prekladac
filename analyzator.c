/**
 * IFJ Projekt 2022
 * @author <xpocho06> Marek Pochop
 */

#include "analyzator.h"
#include "skener.h"
#include "Stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 *  • Premena tokenu na terminal
 */
Term_Type Get_Token(Token_Type Type) {
    switch(Type){
        case T_TYPE_PLUS:
        case T_TYPE_MINUS:
        case T_TYPE_CONCATENATION:
            return T_ADDSUBCON;
        case T_TYPE_MULTIPLICATION:
        case T_TYPE_DIVISION:
            return T_DIVMUL;
        case T_TYPE_TRIPLE_EQUALS:
        case T_TYPE_TRIPLE_EQUALS_NEG:
            return T_EQ;
        case T_TYPE_GREATER:
        case T_TYPE_GREATER_EQUAL:
        case T_TYPE_SMALLER:
        case T_TYPE_SMALLER_EQUAL:
            return T_SMGR;
        case T_TYPE_OPEN_BRACKET:
            return T_LB;
        case T_TYPE_CLOSED_BRACKET:
            return T_RB;
        case T_TYPE_VARIABLE:
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_FLOAT_DATATYPE:
        case T_TYPE_INT_DATATYPE:
        case T_TYPE_NULL_DATATYPE:
            return T_VAL;
        default:
            return T_DOLLAR;
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
        case T_SMGR:
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
        Type = Get_Token(Stack_Tmp.Token.Type);
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
    int Return = 0;             // Navratova hodnota - int 100, float 200, string 300
    int Brackets = 0;           // Pocet zatvoriek
    _TOKEN_ TokenTMP;
    _STACK_ *Stack = Stack_Create();
    Term_Type T_TypeStack;
    Term_Type T_TypeNew;

    if(Condition == 1) {
        if((Return = Scan(Wrap)) != 0) return Return;
    }

    while(1){
        T_TypeStack = Get_Terminal(Stack);
        T_TypeNew = Get_Token(Wrap->Token->Type);
        
        if(T_TypeNew == T_RB && Brackets != 0 && (T_LB <= Get_Token(Stack->Token.Type) && Get_Token(Stack->Token.Type) <= T_VAL)) Brackets--;
        else if(T_TypeNew == T_VAL || T_TypeNew == T_LB) {
            if(Get_Token(Stack->Token.Type) == T_VAL) return 2;  // Pokud je na vstupu promenna nebo ( a na zasobniku promenna tak return 2
            else if(T_TypeNew == T_LB) Brackets++;
        } else if((T_TypeNew == T_SMGR || T_TypeNew == T_EQ) && Condition == 0) return 2;    // Aby nebyly vyrazy typu: x = 5 < 10

        if(Get_Token(Stack->Token.Type) != T_VAL && T_TypeNew != T_VAL && T_TypeNew != T_LB) return 2; // Nebere výrazy 5 +- 10...
        
        if(T_TypeNew == T_VAL && Wrap->Token->Type != T_TYPE_VARIABLE){
            if(Wrap->Token->Type == T_TYPE_INT_DATATYPE && Return < 100) Return = 100;
            else if(Wrap->Token->Type == T_TYPE_FLOAT_DATATYPE && Return < 200) Return = 200;
            else if(Wrap->Token->Type == T_TYPE_STRING_DATATYPE && Return < 300) Return = 300;
        }
        
        if(Stack->Token.Type == T_TYPE_STRING_DATATYPE && Wrap->Token->Type != T_TYPE_CONCATENATION) return 2;  // stringy - vraci pokud je string a neni .
        else if(Stack->Token.Type == T_TYPE_CONCATENATION && Wrap->Token->Type != T_TYPE_STRING_DATATYPE
            && Wrap->Token->Type != T_TYPE_VARIABLE) return 2;                                                  // vraci pokud je konkatenace ale dalsi ne str / promenna
        else if(Wrap->Token->Type == T_TYPE_STRING_DATATYPE && Stack->Token.Type != T_TYPE_CONCATENATION
            && Stack->Token.Type != T_TYPE_NULL) return 2;                                                      // vraci pokud string ale predesla neni . nebo neni 1.
        else if(Wrap->Token->Type == T_TYPE_CONCATENATION && Stack->Token.Type != T_TYPE_STRING_DATATYPE
            && Stack->Token.Type != T_TYPE_VARIABLE) return 2;                                                  // vraci pokud je . ale predesle neni str / promenna

        switch(Relation(T_TypeStack, T_TypeNew)){   // Switch relaci mezi terminaly
            case StackPush:
                Stack_Push(Stack, Wrap->Token);
                if((Return = Scan(Wrap)) != 0) return Return;
                break;
            case StackProcess:
                if(Get_Token(Stack->Token.Type) == T_VAL) TokenTMP = Stack->Token;
                else return 2;
                for(int i = 0; i < 3; i++) Stack = Stack_Pop(Stack);
                Stack_Push(Stack, &TokenTMP);
                break;
            case LR_Brackets:   // pro (E)
                if(Get_Token(Stack->Token.Type) == T_LB && Get_Token(Wrap->Token->Type) == T_RB) return 2;
                if(Get_Token(Stack->Token.Type) == T_VAL) TokenTMP = Stack->Token;
                for(int i = 0; i < 2; i++) Stack = Stack_Pop(Stack);
                Stack_Push(Stack, &TokenTMP);
                Brackets--;
                if(Condition == 0) {
                    if((Return = Scan(Wrap)) != 0) return Return;
                } else if((Return = Scan(Wrap)) != 0) return Return;
                break;
            case Finish:
                if(T_TypeStack == T_DOLLAR && T_TypeNew == T_DOLLAR){
                    if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return 0;  // Spravne se doslo az na konec vyrazu
                    else if(Get_Token(Stack->Token.Type) == T_VAL)  {  // Pokud zustala jen 1 promenna tak pop a return ze spravne
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return 0;
                    }
                    else return 2;
                } else return 2;
                break;
        }
    }
    return 0;
}
