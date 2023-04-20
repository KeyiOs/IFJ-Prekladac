/**
 * IFJ Projekt 2022
 * @authors <xhorac20> Andrej Horacek, <xkento00> Samuel Kentos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generator.h"

int IntStack[256];
int top = -1;
int ID = 0;
int Skip = 0;

// _________________________________________Pomocne Funckie_____________________________________________________________

void my_push() {
    top = top + 1;
    IntStack[top] = ID;
    ID++;
}

void my_pop() {
    top = top - 1;
}

// _________________________________________Funckie na Generovanie______________________________________________________


void G_BigStart() {
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("CALL $$main\n");
    printf("JUMP $$big_end\n");

    G_reads();
    G_readi();
    G_readf();
    G_write();
    G_floatval();
    G_intval();
    G_strval();
    G_strlen();
    G_substring();
    G_ord();
    G_chr();

    printf("\n# main body\n");
    printf("LABEL $$main\n");
    printf("PUSHFRAME\n");
}

void G_BigEnd() {
    printf("LABEL $$big_end\n");
    printf("EXIT int@0\n");
}

_STACK_ *G_CallParam(_STACK_ *local_st, _ITEMF_ *Table) {
    if (local_st == NULL) return local_st;
    while (local_st->Token.Type != T_TYPE_NULL) {
        G_PushParams(local_st, Table);
        local_st = Stack_Pop(local_st);
    }
    return local_st;
}

void G_PushParams(_STACK_ *local_st, _ITEMF_ *Table) {
    switch (local_st->Token.Type) {
        case T_TYPE_INT_DATATYPE:
            printf("PUSHS int@%s\n", local_st->Token.String);
            break;
        case T_TYPE_FLOAT_DATATYPE:
            printf("PUSHS float@%a\n", atof(local_st->Token.String));
            break;
        case T_TYPE_VARIABLE:{
            _ITEMV_ *ItemV = SearchV(&Table->Local, local_st->Token.String);
            printf("PUSHS LF@%s$%d\n", local_st->Token.String, ItemV->Dive);
            break;
        }
        case T_TYPE_STRING_DATATYPE:
            printf("PUSHS string@%s\n", local_st->Token.String);
            break;
        default:
            break;
    }
}

void G_RetVal(char *Name, int Dive) {
    printf("DEFVAR LF@%s$%i\n", Name, Dive);
    printf("POPS LF@%s$%i\n", Name, Dive);
}

void G_LABEL_start(char *label) {
    printf("LABEL func$%s\n", label);
    printf("PUSHFRAME\n");
}

void G_LABEL_end() {
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_StartFunction(char *function) {
    printf("JUMP skip$%i\n", Skip);
    G_LABEL_start(function);
}

void G_EndFunction(char *function) {
    G_LABEL_end();
    printf("#End of function %s\n", function);
}

void G_Param(_PARAM_ *param) {
    if (param == NULL) return;
    while(param != NULL) {
        printf("DEFVAR LF@%s$0\n", param->Name);
        printf("POPS LF@%s$0\n", param->Name);
        param = param->Next;
    }
    printf("DEFVAR LF@retval$1\n");
}

void G_IfGen() {
    printf("#IF $if$%d\n", ID);
}

void G_IfStart() {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $if$%d$else$\n", ID);
    my_push(ID);
}

void G_Else() {
    printf("JUMP $if$%d$end\n", IntStack[top]);
    printf("LABEL $if$%d$else$\n", IntStack[top]);
}

void G_IfEnd() {
    printf("LABEL $if$%d$end\n", IntStack[top]);
    my_pop();
}

void G_WhileStart() {
    printf("LABEL WHILE$CHECK$%d\n", ID);
}

void G_WhileJump() {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS WHILE$END$%d\n", ID);
    my_push(ID);
}

void G_WhileEnd() {
    printf("JUMP WHILE$CHECK$%d\n", IntStack[top]);
    printf("LABEL WHILE$END$%d\n", IntStack[top]);
    my_pop();
}

void G_Return(){
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\nLABEL skip$%i\n", Skip);
    Skip++;
}

void G_ADD(_WRAP_ *Wrap, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("ADDS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("ADDS\n");
    } else if (!strcmp(Val2.String, "$")) {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("ADDS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("ADDS\n");
    }
}

void G_SUB(_WRAP_ *Wrap, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("SUBS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("SUBS\n");
    } else if (!strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("PUSHS LF@Val2\n");
        printf("SUBS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("SUBS\n");
    }
}

void G_CON(_WRAP_ *Wrap, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val1\n");
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        printf("POPS LF@Val1\n");
        printf("CONCAT LF@Val1 LF@Val1 LF@Val2\n");
        printf("PUSHS LF@Val1\n");
    } else if (!strcmp(Val1.String, "$")) {
        printf("DEFVAR LF@Val1\n");
        printf("POPS LF@Val1\n");
        if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("CONCAT LF@Val1 LF@Val1 string@%s\n", Val2.String);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("CONCAT LF@Val1 LF@Val1 LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("PUSHS LF@Val1\n");
    } else if (!strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("CONCAT LF@Val2 LF@Val1 string@%s\n", Val2.String);
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("CONCAT LF@Val2 LF@Val1 LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("PUSHS LF@Val2\n");
    } else {
        printf("DEFVAR LF@Val1\n");
        if(Type1 == T_TYPE_STRING_DATATYPE){
            if(Type2 == T_TYPE_STRING_DATATYPE) printf("CONCAT LF@Val1 string@%s string@%s\n", Val1.String, Val2.String);
            else{
                _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
                printf("CONCAT LF@Val1 string@%s LF@%s$%i\n", Val1.String, Val2.String, TMP->Dive);
            }
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            if(Type2 == T_TYPE_STRING_DATATYPE) printf("CONCAT LF@Val1 LF@%s$%i string@%s\n", Val1.String, TMP->Dive, Val2.String);
            else{
                _ITEMV_ *TMP2 = SearchV(&Wrap->Table->Local, Val2.String);
                printf("CONCAT LF@Val1 LF@%s$%i LF@%s$%i\n", Val1.String, TMP->Dive, Val2.String, TMP2->Dive);
            }
        }
        printf("PUSHS LF@Val1\n");
    }
}

void G_MUL(_WRAP_ *Wrap, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("MULS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("MULS\n");
    } else if (!strcmp(Val2.String, "$")) {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("MULS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("MULS\n");
    }
}

void G_DIV(_WRAP_ *Wrap, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("DIVS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("DIVS\n");
    } else if (!strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("PUSHS LF@Val2\n");
        printf("DIVS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("DIVS\n");
    }
}

void G_EQ(_WRAP_ *Wrap, int EQ, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("EQS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("EQS\n");
    } else if (!strcmp(Val2.String, "$")) {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("EQS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("EQS\n");  
    }
    if (EQ == 0) {
        printf("PUSHS bool@false\n");
        printf("EQS\n");
    }
}

void G_SM(_WRAP_ *Wrap, int SMEQ, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("LTS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("LTS\n");
    } else if (!strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("PUSHS LF@Val2\n");
        printf("LTS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("LTS\n");  
    }
    if(SMEQ == 1){
        printf("PUSHS bool@true\n");
        printf("JUMPIFEQS sm$eq\n");
        G_EQ(Wrap, 1, Val1, Val2);
        printf("LABEL sm$eq\n");
    }
}

void G_GT(_WRAP_ *Wrap, int GTEQ, _TOKEN_ Val1, _TOKEN_ Val2){
    Token_Type Type1 = Val1.Type, Type2 = Val2.Type;
    if (!strcmp(Val1.String, "$") && !strcmp(Val2.String, "$")) printf("GTS\n");
    else if (!strcmp(Val1.String, "$")) {
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("GTS\n");
    } else if (!strcmp(Val2.String, "$")) {
        printf("DEFVAR LF@Val2\n");
        printf("POPS LF@Val2\n");
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        printf("PUSHS LF@Val2\n");
        printf("GTS\n");
    } else {
        if(Type1 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val1.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type1 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val1.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type1 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val1);
        } else if(Type1 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val1.String);
            printf("PUSHS LF@%s$%i\n", Val1.String, TMP->Dive);
        }
        if(Type2 == T_TYPE_INT_DATATYPE){
            int TMP = atoi(Val2.String);
            printf("PUSHS int@%d\n", TMP);
        } else if(Type2 == T_TYPE_FLOAT_DATATYPE){
            float TMP = atof(Val2.String);
            printf("PUSHS float@%a\n", TMP);
        } else if(Type2 == T_TYPE_STRING_DATATYPE){
            printf("PUSHS string@%s\n", Val2);
        } else if(Type2 == T_TYPE_NULL_DATATYPE){
            printf("PUSHS nil@nil\n");
        } else{
            _ITEMV_ *TMP = SearchV(&Wrap->Table->Local, Val2.String);
            printf("PUSHS LF@%s$%i\n", Val2.String, TMP->Dive);
        }
        printf("GTS\n");  
    }
    if(GTEQ == 1) {
        printf("PUSHS bool@true\n");
        printf("JUMPIFEQS gt$eq\n");
        G_EQ(Wrap, 1, Val1, Val2);
        printf("LABEL gt$eq\n");
    }
}

// _________________________________________Vstavane funkcie IFJ22______________________________________________________

void G_reads() {
    printf("#FUNCTION READS\n\n");
    printf("LABEL func$reads\n");
    printf("PUSHFRAME\n");                                  //  TF -> LF

    printf("DEFVAR LF@param1\n");                           //  LF@param1
    printf("READ LF@param1 string\n");                      //  LF@param1 = string zo vstupu
    printf("DEFVAR LF@errorCheck\n");                       //  LF@errorCheck
    printf("TYPE LF@errorCheck LF@param1\n");               //  LF@errorCheck = typ z LF@param1
    //  if (string@string != LF@errorCheck) -> label $ERROR$READS
    printf("JUMPIFNEQ $ERROR$READS string@string LF@errorCheck\n");
    printf("DEFVAR LF@strlen\n");                           //  LF@strlen
    printf("STRLEN LF@strlen LF@param1\n");                 //  LF@strlen = len(LF@param1)
    //  if (LF@strlen == 0) -> label $END$OF$READS
    printf("JUMPIFEQ $ERROR$READS LF@strlen int@0\n");
    printf("SUB LF@strlen LF@strlen int@1\n");                     //  LF@strlen = LF@strlen - 1
    printf("DEFVAR LF@getchar\n");                                 //  LF@getchar
    printf("GETCHAR LF@getchar LF@param1 LF@strlen\n");            //  LF@getchar = jeden znak z LF@param1 na LF@strlen pozicii
    //  if (LF@getchar != " ") -> label $END$OF$READS
    printf("JUMPIFNEQ $END$OF$READS LF@getchar string@\\010\n");
    printf("SETCHAR LF@param1 LF@strlen string@\\000\n");   // v LF@param1 prepise znak na pozici LF@strlen na string@\000

    printf("LABEL $END$OF$READS\n");
    printf("PUSHS LF@param1\n");             // LF@retval$1 = LF@param1
    printf("POPFRAME\n");                               // LF -> TF
    printf("RETURN\n");                                 // RETURN na CALL s TF

    printf("LABEL $ERROR$READS\n");
    printf("PUSHS nil@nil\n");               // LF@retval$1 = nil@nil
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_readi() {
    printf("#FUNCTION READI\n\n");
    printf("LABEL func$readi\n");
    printf("PUSHFRAME\n");                      //  TF -> LF

    printf("DEFVAR LF@param$1\n");
    printf("DEFVAR LF@error$check\n");
    printf("READ LF@param$1 int\n");                    // LF@param$1 = int zo standartneho vstupu
    printf("TYPE LF@error$check LF@param$1\n");         // LF@error$check = typ LF@param$1
    //  if ($ERROR$INPUTI != string@int) -> label $ERROR$READI
    printf("JUMPIFNEQ $ERROR$READI string@int LF@error$check\n");
    printf("PUSHS LF@param$1\n");           // Stack = LF@param$1
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $ERROR$READI\n");
    printf("PUSHS nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_readf() {
    printf("#FUNCTION READF\n\n");
    printf("LABEL func$readf\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@param$1\n");
    printf("DEFVAR LF@error$check\n");
    printf("READ LF@param$1 float\n");
    printf("TYPE LF@error$check LF@param$1\n");
    printf("JUMPIFNEQ $ERROR$READF string@float LF@error$check\n");
    printf("PUSHS LF@param$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $ERROR$READF\n");
    printf("PUSHS nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_write() {
    printf("#FUNCTION WRITE\n\n");
    printf("LABEL func$write\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@write$var\n");
    printf("DEFVAR LF@type$var\n");
    

    printf("LABEL $while$write\n");                              // Cyklus START
    printf("JUMPIFEQ while$end LF@write$0 int@0\n");             // LF@write$0 = 0 then jump
    printf("POPS LF@write$var\n");
    printf("TYPE LF@type$var LF@write$var\n");

    printf("JUMPIFEQ $write$int string@int LF@type$var\n");
    printf("JUMPIFEQ $write$float string@float LF@type$var\n");
    printf("JUMPIFEQ $write$null string@nil LF@type$var\n");

    printf("LABEL $write$\n");                                  //  WRITE
    printf("WRITE LF@write$var\n");
    printf("SUB LF@write$0 LF@write$0 int@1\n");
    printf("JUMP $while$write\n");                              // Cyklus END

    printf("LABEL $write$int\n");
    printf("MOVE LF@write$var LF@write$var\n");
    printf("JUMP $write$\n");

    printf("LABEL $write$float\n");
    printf("MOVE LF@write$var LF@write$var\n");
    printf("JUMP $write$\n");

    printf("LABEL $write$null\n");
    printf("MOVE LF@write$var string@\n");
    printf("JUMP $write$\n");

    printf("LABEL while$end\n");
    printf("CLEARS\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_floatval() {
    printf("#FUNCTION FLOATVAL\n\n");
    printf("LABEL func$floatval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

    printf("JUMPIFEQ $floatval$null nil@nil LF@$1\n");
    printf("JUMPIFEQ $floatval$int string@int LF@type$var\n");
    printf("PUSHS LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $floatval$int\n");
    printf("PUSHS LF@$1\n");
    printf("INT2FLOATS LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $floatval$null\n");
    printf("PUSHS int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_intval() {
    printf("#FUNCTION INTVAL\n\n");
    printf("LABEL func$intval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

    printf("JUMPIFEQ $intval$null nil@nil LF@$1\n");
    printf("JUMPIFEQ $intval$int string@float LF@type$var\n");
    printf("PUSHS LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $intval$float\n");
    printf("PUSHS LF@$1\n");
    printf("FLOAT2INTS\n");
    printf("RETURN\n");

    printf("LABEL $intval$null\n");
    printf("PUSHS float@0x0.0p+0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_strval() {
    printf("#FUNCTION STRVAL\n\n");
    printf("LABEL func$strval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

    printf("JUMPIFEQ $strval$null nil@nil LF@$1\n");
    printf("PUSHS LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $strval$null\n");
    printf("PUSHS string@""\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_strlen() {
    printf("#FUNCTION STRLEN\n\n");
    printf("LABEL func$strlen\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@lenvar$1\n");
    printf("POPS LF@lenvar$1\n");
    printf("STRLEN LF@lenvar$1 LF@lenvar$1\n");
    printf("PUSHS LF@lenvar$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_substring(){
    printf("LABEL func$substring\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@j\n");
    printf("DEFVAR LF@n\n");
    printf("DEFVAR LF@check\n");

    printf("DEFVAR LF@retval$1\n");

    printf("DEFVAR LF@$bool\n");
    printf("DEFVAR LF@$len\n");
    printf("DEFVAR LF@$tmpstring\n");

    printf("POPS LF@string\n");
    printf("POPS LF@i\n");
    printf("POPS LF@j\n");
    printf("SUB LF@n LF@j  LF@i\n");
    printf("ADD LF@n  LF@n int@1\n");

    printf("GT LF@check LF@i LF@j\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("STRLEN LF@$len LF@string\n");
    printf("GT LF@check LF@j LF@$len\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("EQ LF@check LF@i LF@$len\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("MOVE LF@retval$1 string@\n");

    printf("CLEARS\n");
    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("GTS\n");
    printf("PUSHS LF@i\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("ANDS\n");

    printf("PUSHS LF@n\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("ANDS\n");

    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$error LF@$bool bool@false\n");

    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("SUBS\n");
    printf("PUSHS LF@n\n");
    printf("LTS\n");

    printf("POPS LF@$bool\n");
    printf("ADD LF@n LF@n LF@i #defaultvalue\n");
    printf("JUMPIFEQ $substring$lts bool@false LF@$bool\n");
    printf("MOVE LF@n LF@$len\n");
    printf("LABEL $substring$lts\n");


    printf("LABEL $substring$whilestart\n");
    printf("PUSHS LF@i\n");
    printf("PUSHS LF@n\n");
    printf("LTS\n");
    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$end LF@$bool bool@false\n");

    printf("GETCHAR LF@$tmpstring LF@string LF@i\n");
    printf("CONCAT LF@retval$1 LF@retval$1 LF@$tmpstring\n");

    printf("ADD LF@i LF@i int@1\n");
    printf("JUMP $substring$whilestart\n");

    printf("LABEL $substring$error\n");
    printf("MOVE LF@retval$1 nil@nil\n");

    printf("LABEL $substring$end\n");
    printf("PUSHS LF@retval$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_ord(){
    printf("#FUNCTION ORD\n");
    printf("LABEL func$ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ord$string\n");
    printf("DEFVAR LF@ord$int\n");
    printf("DEFVAR LF@retval$1\n");

    printf("POPS LF@ord$string\n");
    printf("MOVE LF@ord$int int@0\n");
    printf("DEFVAR LF@error$check\n");
    printf("PUSHS LF@ord$int\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("DEFVAR LF@$lenght$ord\n");
    printf("STRLEN LF@$lenght$ord LF@ord$string\n");
    printf("PUSHS LF@ord$int\n");
    printf("PUSHS LF@$lenght$ord\n");
    printf("LTS\n");
    printf("ANDS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $ERROR$ORD\n");
    printf("STRI2INT LF@retval$1 LF@ord$string LF@ord$int\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $ERROR$ORD\n");
    printf("MOVE LF@retval$1 int@0\n");

    printf("PUSHS LF@retval$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_chr() {
    printf("#FUNCTION CHR\n\n");
    printf("LABEL func$chr\n");
    printf("PUSHFRAME\n");

    printf("INT2CHARS\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}