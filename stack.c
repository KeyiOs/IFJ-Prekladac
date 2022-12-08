/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentos
 */
#include "stack.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void Stack_Init(_STACK_ *Stack) {
    Stack->Previous = NULL;
    Stack->Token.Type = T_TYPE_NULL;
    Stack->Token.String = NULL;
}

_STACK_ *Stack_Create() {
    _STACK_ *Stack_New = malloc(sizeof(_STACK_));
    if(Stack_New == NULL) return NULL;
    Stack_Init(Stack_New);
    return Stack_New;
}

int Stack_Push (_STACK_ *Stack, _TOKEN_ *Token) {
    if(Stack->Token.Type == T_TYPE_NULL) {
        Stack->Token = *Token;
    } else {
        _STACK_ *Stack_New = Stack_Create();
        if(!Stack_New) return 99;
        Stack_New->Previous = Stack->Previous;
        Stack_New->Token = Stack->Token;
        Stack->Previous = Stack_New;
        Stack->Token = *Token;
    }
    return 0;
}

_STACK_ *Stack_Pop(_STACK_ *Stack) {
    if (Stack->Previous == NULL) Stack_Init(Stack);
    else {
        _STACK_ *Stack_Tmp = Stack;
        Stack = Stack->Previous;
        Stack_Tmp->Previous = NULL;
        free(Stack_Tmp);
    }
    return Stack;
}

_STACK_ *G_Stack_Push (_STACK_ *Stack, _TOKEN_ *Token) {
    if(Stack->Token.Type == T_TYPE_NULL) {
        Stack->Token = *Token;
    } else {
        _STACK_ *Stack_New = Stack;
        while(Stack_New->Previous != NULL) Stack_New = Stack_New->Previous;
        Stack_New->Previous = Stack_Create();
        if(!Stack_New->Previous) return NULL;
        Stack_New->Previous->Token = *Token;
    }
    return Stack;
}