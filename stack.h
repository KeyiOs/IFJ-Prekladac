/**
 * IFJ Projekt 2021
 * @author <xkento00> Samuel Kentos
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define __TOKEN__ struct Token_t

#ifndef STACK_PARSER_H
#define STACK_PARSER_H

void Stack_Init ( __TOKEN__ *Stack );
__TOKEN__ *Stack_Create ( );
__TOKEN__ *Stack_Push ( __TOKEN__ *Stack, __TOKEN__ *Token );
__TOKEN__ *Stack_Pop ( __TOKEN__ *Stack );

#endif