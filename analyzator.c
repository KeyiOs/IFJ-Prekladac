/**
 * IFJ Projekt 2022
 * @author <xpocho06> Marek Pochop
 */

#include "analyzator.h"
#include "skener.h"
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//Typ terminálu podle typu tokenu
Term_Type Get_Term(Token_Type Type) {
    switch(Type){
        case T_TYPE_PLUS:
        case T_TYPE_MINUS:
        case T_TYPE_CONCATENATION:
            return T_ADDSUBCON;
        case T_TYPE_TRIPLE_EQUALS:
        case T_TYPE_TRIPLE_EQUALS_NEG:
            return T_TEQNTEQ;
        case T_TYPE_MULTIPLICATION:
        case T_TYPE_DIVISION:
            return T_DIVMUL;
        case T_TYPE_GREATER:
        case T_TYPE_GREATER_EQUAL:
        case T_TYPE_SMALLER:
        case T_TYPE_SMALLER_EQUAL:
            return T_SMGREQ;
        case T_TYPE_VARIABLE:
        case T_TYPE_STRING_DATATYPE:
        case T_TYPE_FLOAT_DATATYPE:
        case T_TYPE_INT_DATATYPE:
            return T_VAL;
        case T_TYPE_OPEN_BRACKET:
            return T_LB;
        case T_TYPE_CLOSED_BRACKET:
            return T_RB;
        default:
            return T_DOLLAR;
    }
}

//Relace mezi terminalem na stacku a typem vstupniho tokenu
Rel_Type Relation(Term_Type stack, Term_Type entry) {
    switch(stack){
        case T_ADDSUBCON:
            if(entry == T_DIVMUL || entry == T_VAL || entry == T_LB){
                return T_OPEN_B;
            }
            else{
                return T_CLOSED_B;
            }
        case T_DIVMUL:
            if(entry == T_VAL || entry == T_LB){
                return T_OPEN_B;
            }
            else{
                return T_CLOSED_B;
            }
        case T_SMGREQ:
            if(entry == T_ADDSUBCON || entry == T_DIVMUL || entry == T_VAL || entry == T_LB){
                return T_OPEN_B;
            }
            else{
                return T_CLOSED_B;
            }
        case T_TEQNTEQ:
            if(entry == T_TEQNTEQ || entry == T_RB || entry == T_DOLLAR){
                return T_CLOSED_B;
            }
            else{
                return T_OPEN_B;
            }
        case T_LB:
            if(entry == T_DOLLAR){
                return T_NOTHING;
            }
            else if(entry == T_RB){
                return T_EQUAL;
            }
            else{
                return T_OPEN_B;
            }
        case T_RB:
        case T_VAL:
            if(entry == T_LB || entry == T_VAL){
                return T_NOTHING;
            }
            else{
                return T_CLOSED_B;
            }
        case T_DOLLAR:
            if(entry == T_RB || entry == T_DOLLAR){
                return T_NOTHING;
            }
            else{
                return T_OPEN_B;
            }
    }
    return T_NOTHING;
}

//hledani prvniho terminalu na stacku (prob jinak) (ignoruje promenn/cisla/...)
Term_Type find_first_terminal(_STACK_ *Stack) {
    _STACK_ *Stack_Tmp = Stack;
    Term_Type Type;

    while(Stack_Tmp->Previous != NULL){
        Type = Get_Term(Stack_Tmp->Token.Type);
        if((Type >= T_ADDSUBCON && Type <= T_RB) && Type != T_VAL){
            return Type;
        }
        Stack_Tmp = Stack_Tmp->Previous;
    }

    Type = Get_Term(Stack_Tmp->Token.Type);
    if ((Type >= T_ADDSUBCON && Type <= T_RB) && Type != T_VAL){
        return Type;
    }

    return T_DOLLAR;
}

//prochazi vyrazy
int expressions(_WRAP_ *Wrap, int eq) {
    int ERR = 0;
    int loop = 0;
    int lb = 0; //jen urcuje jestli se zavolala funkce kvuli ( nebo kvuli promenne
    int brackets = 0; //hlida pocet zavorek
    _TOKEN_ tmp_token;
    _STACK_ *Stack = Stack_Create();
    Term_Type current_t;
    Term_Type new_t;

    if(Get_Term(Wrap->Token->Type) == T_LB) lb = 1;
    else if(eq == 1){ //preskakuje = cuz netreba na stacku i guess
        if((ERR = Scan(Wrap)) != 0) return ERR;
        if(Wrap->Token->Type == T_TYPE_FUNCTION || Wrap->Token->Type == T_TYPE_KEYWORD){ //nutne pro x = funkce/keyword
            if((ERR = Scan(Wrap)) != 0) return ERR;
        }
        if(Get_Term(Wrap->Token->Type) == T_DOLLAR) return 2; //aby nebyly veci typu: x = ;
    }
    else if(eq == 0 && Get_Term(Wrap->Token->Type) == T_VAL) return 2; //aby nebyly veci typu: x x + 10

    while(loop < 1){
        current_t = find_first_terminal(Stack);
        new_t = Get_Term(Wrap->Token->Type);
        if(Get_Term(Stack->Token.Type) == T_LB) brackets++;
        else if(new_t == T_RB && brackets != 0 && current_t == T_LB) brackets--;
        if((new_t == T_VAL || new_t == T_LB) && Get_Term(Stack->Token.Type) == T_VAL){ //pokud je na vstupu promenna nebo ( a na zasobniku promenna tak return 2
            return 2;
        }
        else if((new_t == T_SMGREQ || new_t == T_TEQNTEQ) && lb == 0) return 2; //aby nebyly vyrazy typu: x = 5 < 10

        switch(Relation(current_t, new_t)){ //switch relaci mezi terminaly
            case T_OPEN_B:
                Stack_Push(Stack, Wrap->Token);
                if((ERR = Scan(Wrap)) != 0) return ERR;
                break;
            case T_CLOSED_B:
                if(Get_Term(Stack->Token.Type) == T_VAL){
                    tmp_token = Stack->Token;
                }
                for(int i = 0; i < 3; i++){
                    Stack = Stack_Pop(Stack);
                }
                Stack_Push(Stack, &tmp_token);
                break;
            case T_EQUAL:           //pro (E)
                if(Get_Term(Stack->Token.Type) == T_LB && Get_Term(Wrap->Token->Type) == T_RB){ //prazdna funkce
                    Stack = Stack_Pop(Stack);
                    if((ERR = Scan(Wrap)) != 0) return ERR;
                    return 0;
                }
                if(Get_Term(Stack->Token.Type) == T_VAL){
                    tmp_token = Stack->Token;
                }
                for(int i = 0; i < 2; i++){
                    Stack = Stack_Pop(Stack);
                }
                Stack_Push(Stack, &tmp_token);
                if(lb == 0){
                    if((ERR = Scan(Wrap)) != 0) return ERR;
                }
                else{
                    if(Stack->Token.Type == T_TYPE_NULL && brackets == 0) return 0;
                    else if(Get_Term(Stack->Token.Type) == T_VAL){
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && brackets == 0) return 0;
                    }
                    else return 2;
                }
                break;
            case T_NOTHING:
                if(current_t == T_DOLLAR && new_t == T_DOLLAR){
                    if(Stack->Token.Type == T_TYPE_NULL && brackets == 0) return 0;  //spravne se doslo az na konec vyrazu
                    else if(Get_Term(Stack->Token.Type) == T_VAL){  //pokud zustala jen 1 promenna tak pop a return ze spravne
                        Stack = Stack_Pop(Stack);
                        if(Stack->Token.Type == T_TYPE_NULL && brackets == 0) return 0;
                    }
                    else return 2;
                }
                else return 2;
                break;
        }

    }
    return 0;
}
