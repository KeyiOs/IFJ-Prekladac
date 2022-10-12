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

_TOKEN_ *T_Assign(_TOKEN_ *Token, Token_Type Type, Token_Value Value);
int Scan(_TOKEN_ **Token);

#endif