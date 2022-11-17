/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token

#ifndef SKENER_CHECK
#define SKENER_CHECK

int Prolog(FILE* Source, int *Character);
int Comment(int *Character, FILE* Source);
int Strings(int *Character, Token_Value Value, _TOKEN_ **Token, int Type, FILE* Source);
int Scan(_TOKEN_ **Token, FILE* Source, int *Character);

#endif