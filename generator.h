/**
 * IFJ Projekt 2021
 * @author <xhorac20> Andrej Horacek
 */

#include <stdbool.h>
#include <string.h>

/// Jednotlive prvky v String liste
typedef struct StringElement {
    struct StringElement *ptr;
    char *data;
} *StringElementPtr;

/// Struktura String List
typedef struct {
    StringElementPtr Actual;
    StringElementPtr First;
    StringElementPtr Last;
} StringList;

#ifndef GENERATOR_H
#define GENERATOR_H

// _________________________________________Pomocne Funckie_____________________________________________________________

/// Pushne int na vrchol zasobniku a inkrementuje ID
void my_push();

/// Popne int z vrcholu zasobniku
void my_pop();

// _________________________________________Instruction List Funkcie____________________________________________________

/// Inicializacia Stringlistu
/// @param List - ukazovatel na Stringlist
void InitListString(StringList *List);

/// Ukoncenie Stringlistu
/// @param List - ukazovatel na Stringlist
void DisposeListString(StringList *List);

/// Vlozenie stringu na 1. poziciu v Stringliste
/// @param List - ukazovatel na Stringlist
/// @param val - vkladany string
void InsertFirstString(StringList *List, char *val);

/// Vlozenie stringu na poslednu poziciu v Stringliste
/// @param List - ukazovatel na Stringlist
/// @param val - vkladany string
void InsertLastString(StringList *List, char *val);

/// Zmazanie strignu z 1. pozicie v Stringliste
/// @param List - ukazovatel na Stringlist
void DeleteFirstString(StringList *List);

// _________________________________________Funckie na Generovanie______________________________________________________

/// generuje vstavane funkcie, a header
void G_BigStart();

/// generuje koniec programu a terminuje string listy
void G_BigEnd();

/// Generovanie premennej
/// @param id - id premennej
/// @param scope - rozsah
/// @param in_for - True ak je vo for
void G_defvar(char* id,int scope, bool in_for);

/// Generovanie navratovej premennej
/// @param NumberOfReturns - pocet generovanych navratovych premennych
void gen_retvals(int number_of_return_values);

/// Generovanie parametrov
/// @param params - ukazovatel na string s parametramy
void gen_params(string* params);

/// Generovanie parametrov a ich inicializacia
/// @param last - pointer na strukturu token
/// @param local_st -  pointer na strukturu zasobnik
void gen_call_params(token_t *last, st_stack_t *local_st);

/// Generovanie instrukcie na vlozenie hodnoty do zasobniku
/// @param type typ tokrnu
/// @param value pointer na string
/// @param local_st pointer na strukturu zasobnik
void gen_pushs_param(token_type type, string *value, st_stack_t *local_st);

/// Generuje meno premennej ktore vlozi na 1. miesto v Stringliste Vars
/// @param var_name - meno premennej
/// @param scope - rozsah
void gen_add_to_vars(char *var_name, int scope);

/// Vlozi string instrukcii do string listu EXP
/// @param exp - string instrukcii
/// @param in_for - True ak je vo for
void gen_add_to_exp(char *exp, bool in_for);

/// Generuje instrukciu pre vytvorenie TF premennej a vlozi do nej hodnotu pred zacaitkom for
/// @param NumberOfVariables - pocet premennych
void gen_for_assign(int NumberOfVariables);

/// Inicializacia premennej
/// @param NumberOfVariables - pocet premennych
/// @param in_for - True ak je vo for
void gen_assign(int NumberOfVariables, bool in_for);

/// Generuje instruckiu na presun hodnoty z navratovej premmenj do lokalnej
/// @param NumberOfVariables - pocet premennych
void gen_assign_return(int NumberOfVariables);

/// Generuje instrukciu na priradenie hodnoty do navratovej premennej
void gen_set_retvals(int NumberOfReturns, bool in_for);

/// Generovanie FOR - kontrola podmienok
/// @param expression - podmienka
void G_for_start(char *expression);

/// Generovanie FOR - skok na koniec
void G_for_jump();

/// Generovanie FOR - koniec
void G_for_end();

/// generovanie CALL
/// @param function - meno funkcie
/// @param count_of_vars - pocet vstupov danej funkcie
void G_call_start(char *function, int count_of_vars);

/// generovanie CALL-u
/// @param function - meno funkcie
void G_call(char *function);

/// generovanie LABEL zaciatok
/// @param label - meno LABEL
void G_LABEL_start(char *label);

/// generovanie LABEL koniec
void G_LABEL_end();

/// generovanie start funkcie
/// @param function - meno funkcie
void G_start_of_function(char *function);

/// generovanie end funkcie
void G_end_of_function();

/// generovanie start z IF
void G_if_start();

/// generovanie else z IF
void G_else();

/// generovanie koniec z IF
void G_if_end();

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