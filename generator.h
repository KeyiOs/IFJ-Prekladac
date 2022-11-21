/**
 * IFJ Projekt 2021
 * @author <xhorac20> Andrej Horacek
 */


#include <stdbool.h>

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

// _________________________________________Funckie na Generovanie______________________________________________________

/// generuje vstavane funkcie, a header
void G_BigStart();

/// generuje koniec programu a terminuje string listy
void G_BigEnd();

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