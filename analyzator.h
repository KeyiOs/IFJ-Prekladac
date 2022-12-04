/**
 * IFJ Projekt 2022
 * @author <xpocho06> Marek Pochop
 */

#include "analyzator.h"
#include "parser.h"
#include "skener.h"
#include "symtable.h"

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack

#ifndef ANALYZATOR_CHECK
#define ANALYZATOR_CHECK

//Veci z precedencní tabulky
typedef enum {
    T_ADDSUBCON = 1,// + | - | .
    T_TEQNTEQ,      // === | !==
    T_DIVMUL,       // * | /
    T_DOLLAR,       // $
    T_SMGREQ,       // < | > | <= | >=
    T_VAL,          // promenná
    T_LB,           // (
    T_RB,           // )
} Term_Type;

//Relace mezi terminály (podle tabulky zase)
typedef enum {
    T_CLOSED_B = 1, // >
    T_NOTHING,      //
    T_OPEN_B,       // <
    T_EQUAL,        // =
} Rel_Type;

Term_Type Get_Term(Token_Type Type);                                    //typ terminalu tokenu
Rel_Type Relation(Term_Type stack, Term_Type entry);                    //relace mezi 2 tokeny
Term_Type find_first_terminal(_STACK_ *Stack);                          //nalezeni terminalu na stacku
int expressions(_WRAP_ *Wrap, int eq);                                  //eq - promenna s = -> 1, promenna bez = -> 0

#endif
