/**
 * IFJ Projekt 2021
 * @author <xkento00> Samuel Kentos
 */
#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token
#define _STACK_ struct Stack

#ifndef STACK_PARSER_H
#define STACK_PARSER_H

void Stack_Init ( _STACK_ *Stack );
_STACK_ *Stack_Create ( );
int Stack_Push ( _STACK_ *Stack, _TOKEN_ *Token );
_STACK_ *Stack_Pop ( _STACK_ *Stack );

#endif