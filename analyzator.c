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
Term_Type Get_Term(Token_Type Type) {
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
            return T_TEQNTEQ;
        case T_TYPE_GREATER:
        case T_TYPE_GREATER_EQUAL:
        case T_TYPE_SMALLER:
        case T_TYPE_SMALLER_EQUAL:
            return T_SMGREQ;
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
            if(Entry == T_DIVMUL || Entry == T_VAL || Entry == T_LB) return T_OPEN_B;
            else return T_CLOSED_B;
        case T_DIVMUL:
            if(Entry == T_VAL || Entry == T_LB) return T_OPEN_B;
            else return T_CLOSED_B;
        case T_SMGREQ:
            if(Entry == T_ADDSUBCON || Entry == T_DIVMUL || Entry == T_VAL || Entry == T_LB) return T_OPEN_B;
            else return T_CLOSED_B;
        case T_TEQNTEQ:
            if(Entry == T_TEQNTEQ || Entry == T_RB || Entry == T_DOLLAR) return T_CLOSED_B;
            else return T_OPEN_B;
        case T_LB:
            if(Entry == T_DOLLAR) return T_NOTHING; 
            else if(Entry == T_RB) return T_EQUAL; 
            else return T_OPEN_B; 
        case T_RB:
        case T_VAL:
            if(Entry == T_LB || Entry == T_VAL) return T_NOTHING; 
            else return T_CLOSED_B; 
        case T_DOLLAR:
            if(Entry == T_RB || Entry == T_DOLLAR) return T_NOTHING; 
            else return T_OPEN_B; 
    }
    
    return T_NOTHING;
}

/*
 *  • Vyhladanie najnovsieho terminalu na staku
 */
Term_Type Get_Terminal(_STACK_ *Stack) {
    _STACK_ Stack_Tmp = *Stack;
    Term_Type Type;

    while(Stack_Tmp.Token.Type != T_TYPE_NULL){
        Type = Get_Term(Stack_Tmp.Token.Type);
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
    int ERR = 0;                // Navratova hodnota
    int Brackets = 0;           // Pocet zatvoriek
    _TOKEN_ TokenTMP;
    Token_Type Type = 0;
    int Ret_value = 0;          //int 100, float 200, string 300
    _STACK_ *Stack = Stack_Create();
    Term_Type T_TypeStack;
    Term_Type T_TypeNew;

    if(Condition == 1) {
        if((ERR = Scan(Wrap)) != 0) return ERR;
    }

    while(1){
        T_TypeStack = Get_Terminal(Stack);
        T_TypeNew = Get_Term(Wrap->Token->Type);
        
        if(T_TypeNew == T_RB && Brackets != 0 && T_TypeStack == T_LB) Brackets--;
        else if(T_TypeNew == T_VAL || T_TypeNew == T_LB) {
            if(Get_Term(Stack->Token.Type) == T_VAL) return 2;  // Pokud je na vstupu promenna nebo ( a na zasobniku promenna tak return 2
            else if(T_TypeNew == T_LB) Brackets++;
        } else if((T_TypeNew == T_SMGREQ || T_TypeNew == T_TEQNTEQ) && Condition == 0) return 2;    // Aby nebyly vyrazy typu: x = 5 < 10

        if(Get_Term(Stack->Token.Type) != T_VAL && T_TypeNew != T_VAL && T_TypeNew != T_LB) return 2; //nebere výrazy 5 +- 10...
        
        if(Wrap->Token->Type == T_TYPE_NULL_DATATYPE && Condition == 0) return 2;                               // vraci pokud je x = null treba

        if(Stack->Token.Type == T_TYPE_STRING_DATATYPE && Wrap->Token->Type != T_TYPE_CONCATENATION) return 2;  // stringy - vraci pokud je string a neni .
        if(Stack->Token.Type == T_TYPE_CONCATENATION && Wrap->Token->Type != T_TYPE_STRING_DATATYPE
            && Wrap->Token->Type != T_TYPE_VARIABLE) return 2;                                                  // vraci pokud je konkatenace ale dalsi ne str / promenna
        if(Wrap->Token->Type == T_TYPE_STRING_DATATYPE && Stack->Token.Type != T_TYPE_CONCATENATION
            && Stack->Token.Type != T_TYPE_NULL) return 2;                                                      // vraci pokud string ale predesla neni . nebo neni 1.
        if(Wrap->Token->Type == T_TYPE_CONCATENATION && Stack->Token.Type != T_TYPE_STRING_DATATYPE
            && Stack->Token.Type != T_TYPE_VARIABLE) return 2;                                                  // vraci pokud je . ale predesle neni str / promenna
        
        if(T_TypeNew == T_VAL && Wrap->Token->Type != T_TYPE_VARIABLE){
            if(Wrap->Token->Type == T_TYPE_INT_DATATYPE && (Type == T_TYPE_INT_DATATYPE || Type == 0)){
                    Type = T_TYPE_INT_DATATYPE;
                    Ret_value = 100;
            }
            if(Wrap->Token->Type == T_TYPE_FLOAT_DATATYPE && Type != T_TYPE_STRING_DATATYPE){
                Type = T_TYPE_FLOAT_DATATYPE;
                Ret_value = 200;
            }
            if(Wrap->Token->Type == T_TYPE_STRING_DATATYPE && (Type == 0 || Type == T_TYPE_STRING_DATATYPE)){
                Type = T_TYPE_STRING_DATATYPE;
                Ret_value = 300;
            }
        }
        
        switch(Relation(T_TypeStack, T_TypeNew)){   // Switch relaci mezi terminaly
            case T_OPEN_B:
                Stack_Push(Stack, Wrap->Token);
                if((ERR = Scan(Wrap)) != 0) return ERR;
                break;
            case T_CLOSED_B:
                if(Get_Term(Stack->Token.Type) == T_VAL) TokenTMP = Stack->Token;
                for(int i = 0; i < 3; i++) Stack = Stack_Pop(Stack);
                Stack_Push(Stack, &TokenTMP);
                break;
            case T_EQUAL:           // pro (E)
                if(Get_Term(Stack->Token.Type) == T_LB && Get_Term(Wrap->Token->Type) == T_RB){ //prazdna funkce
                    Stack = Stack_Pop(Stack);
                    if((ERR = Scan(Wrap)) != 0) return ERR;
                    return 0;
                }
                if(Get_Term(Stack->Token.Type) == T_VAL) TokenTMP = Stack->Token;
                for(int i = 0; i < 2; i++) Stack = Stack_Pop(Stack);
                Stack_Push(Stack, &TokenTMP);
                if(Condition == 0) {
                    if((ERR = Scan(Wrap)) != 0) return ERR;
                } else {
                    if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return 0;
                    else if(Get_Term(Stack->Token.Type) == T_VAL){
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return 0;
                    } else return 2;
                }
                break;
            case T_NOTHING:
                if(T_TypeStack == T_DOLLAR && T_TypeNew == T_DOLLAR){
                    if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return Ret_value;  //spravne se doslo az na konec vyrazu
                    else if(Get_Term(Stack->Token.Type) == T_VAL){  //pokud zustala jen 1 promenna tak pop a return ze spravne
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && Brackets == 0) return Ret_value;
                    }
                    else return 2;
                }
                else return 2;
                break;
        }
    }
    return 0;
}
