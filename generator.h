/**
 * IFJ Projekt 2022
 * @author <xhorac20> Andrej Horacek
 */

#include "parser.h"
#include "stack.h"
#include "symtable.h"

#include <stdbool.h>
#include <string.h>

#ifndef GENERATOR_H
#define GENERATOR_H

// _________________________________________Pomocne Funckie_____________________________________________________________

/// Pushne int na vrchol zasobniku a inkrementuje ID
void my_push(int ID);

/// Popne int z vrcholu zasobniku
void my_pop();

// _________________________________________Funckie na Generovanie______________________________________________________

/// generuje vstavane funkcie, a header
void G_BigStart();

/// generuje koniec programu a terminuje string listy
void G_BigEnd();

/// Generovanie parametrov a ich inicializacia
/// @param last - pointer na strukturu token
/// @param local_st - pointer na strukturu zasobnik
void G_CallParam(_STACK_ *local_st, _ITEMF_ *Table);

/// Generovanie instrukcie na vlozenie hodnoty do zasobniku
/// @param type typ tokrnu
/// @param value pointer na string
/// @param local_st pointer na strukturu zasobnik
void G_PushParams(_STACK_ *local_st, _ITEMF_ *Table);

/// generovanie CALL
/// @param function - meno funkcie
/// @param count_of_vars - pocet vstupov danej funkcie
void G_CallStart(char *function, int count_of_vars);

/// generovanie CALL-u
/// @param function - meno funkcie
void G_Call(char *function);

/// generovanie LABEL zaciatok
/// @param label - meno LABEL
void G_LABEL_start(char *label);

/// generovanie LABEL koniec
void G_LABEL_end();

/// generovanie start funkcie
/// @param function - meno funkcie
void G_StartFunction(char *function);

/// generovanie end funkcie
void G_EndFunction(char *function);

/// Generovanie parametrov
/// @param params - ukazovatel na string s parametramy
void G_Param(_PARAM_ *param);

/// generovanie labelu pre IF
void G_IfGen(int ID);

/// generovanie vyhodnotenia pre IF
void G_IfStart(int ID);

/// generovanie else z IF
void G_Else();

/// generovanie koniec z IF
void G_IfEnd();

/// Generovanie FOR - kontrola podmienok
/// @param expression - podmienka
void G_WhileStart(int ID);

/// Generovanie FOR - skok na koniec
void G_WhileJump(int ID);

/// Generovanie FOR - koniec
void G_WhileEnd();

/// Generovanie RETURN
void G_Return();




/// Generovanie premennej
/// @param id - id premennej
/// @param scope - rozsah
/// @param in_for - True ak je vo for
void G_DefVar(char *Name, int Dive, bool in_for);

/// Generuje meno premennej ktore vlozi na 1. miesto v _STACK_ Vars
/// @param var_name - meno premennej
/// @param scope - rozsah
void G_AddToVariable(char *var_name, int scope);

/// Vlozi string instrukcii do string listu EXP
/// @param exp - string instrukcii
/// @param in_for - True ak je vo for
void G_AddToExponent(char *exp, bool in_for);

/// Generuje instrukciu pre vytvorenie TF premennej a vlozi do nej hodnotu pred zacaitkom for
/// @param NumberOfVariables - pocet premennych
void G_ForAss(int NumberOfVariables);

// _________________________________________Vstavane funkcie IFJ22______________________________________________________

void G_readi();

void G_reads();

void G_readf();

void G_write();

void G_floatval();

void G_intval();

void G_strval();

void G_strlen();

void G_substring();

void G_ord();

void G_chr();

#endif