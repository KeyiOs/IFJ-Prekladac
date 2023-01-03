/**
 * IFJ Projekt 2022
 * @author <xpocho06> Marek Pochop
 */
#ifndef ANALYZATOR_CHECK
#define ANALYZATOR_CHECK

#include "analyzator.h"
#include "parser.h"
#include "skener.h"
#include "symtable.h"
#include "stack.h"
#include "generator.h"

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack

// Veci z precedencní tabulky
typedef enum {
    T_ADDSUBCON = 1,// + | - | .
    T_DIVMUL,       // * | /
    T_EQ,           // === | !==
    T_SMGT,         // < | > | <= | >=
    T_LB,           // (
    T_RB,           // )
    T_VAL,          // promenná
    T_DOLLAR,       // $
} Term_Type;

// Relace mezi terminály (podle tabulky zase)
typedef enum {
    StackProcess = 1,
    Finish,
    StackPush,
    LR_Brackets,
    ERROR,
} Rel_Type;

Term_Type Get_Precedence(Token_Type Type);              // Uroven precedencie Tokenu
Rel_Type Relation(Term_Type stack, Term_Type entry);    // Urcuje vztah medzi tokenmi
Term_Type Get_Terminal(_STACK_ *Stack);                 // Najdenie terminalu na staku
int Expression(_WRAP_ *Wrap, int Condition);            // Hlavna Funkcia Analyzi

#endif