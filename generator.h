/**
 * IFJ Projekt 2021
 * @author <xhorac20> Andrej Horacek
 */


#include <stdbool.h>

#ifndef GENERATOR_H
#define GENERATOR_H

void G_BigStart();

void G_BigEnd();

void G_call_start(char *function, int count_of_vars);

void G_call(char *function);

void G_LABEL_start(char *label);

void G_LABEL_end();

void G_start_of_function(char *function);

void G_end_of_function();

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