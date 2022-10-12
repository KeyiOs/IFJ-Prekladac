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

/*
/  MinGW32-make
/  ./parser
/  Vstup
*/

/*
/ SHIFT-SHIFT-B
/ Input.txt
/ CTRL-H
*/

_TOKEN_ *T_Create(){
    _TOKEN_ *Token = malloc(sizeof(struct Token));
    if(!Token) return NULL;

    Token->Type = T_TYPE_NULL;
    Token->Following = NULL;
    Token->Preceding = NULL;
    
    return Token;
}

void T_Free(_TOKEN_ *Token){
    _TOKEN_ *T_Current = Token;
    Token = NULL;

    while(T_Current != NULL){
        _TOKEN_ *T_Next = T_Current->Following;
        free(T_Current);
        T_Current = T_Next;
    }
}

int main(){
    _TOKEN_ *Token = T_Create();                                        // Inicialization
    if(Token == NULL) return 99;                                        // 
                                                                        //
    _TOKEN_ *T_First = Token;                                           //

    int ERR = Scan(&Token);                                             // Scanner
    if(ERR != 0){                                                       //
        T_Free(T_First);;                                               //
        ERR_Handler(ERR);                                               //
         return ERR;                                                     //
    }                                                                   //
    Token = T_First;                                                    //

    T_Free(T_First);                                                    // Memory Deallocation
    Token = NULL;                                                       //
    T_First = NULL;                                                     //

    return 0;                                                           // End
}