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

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack

// Veci z precedencní tabulky
typedef enum {
    T_ADDSUBCON = 1,// + | - | .
    T_DIVMUL,       // * | /
    T_TEQNTEQ,      // === | !==
    T_SMGREQ,       // < | > | <= | >=
    T_LB,           // (
    T_RB,           // )
    T_VAL,          // promenná
    T_DOLLAR,       // $
} Term_Type;

// Relace mezi terminály (podle tabulky zase)
typedef enum {
    T_CLOSED_B = 1, // >
    T_NOTHING,      // $
    T_OPEN_B,       // <
    T_EQUAL,        // =
} Rel_Type;

Term_Type Get_Term(Token_Type Type);                    // Typ Terminalu Tokenu
Rel_Type Relation(Term_Type stack, Term_Type entry);    // Urcuje vztah medzi tokenmi
Term_Type Get_Terminal(_STACK_ *Stack);                 // Najdenie terminalu na staku
int Expression(_WRAP_ *Wrap, int Condition);            // Hlavna Funkcia Analyzi

#endif