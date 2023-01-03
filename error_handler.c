/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentos
 */

#include "error_handler.h"

#include <stdlib.h>
#include <stdio.h>

void ERR_Handler(int ERR, int Line) {
    switch(ERR) {
        case 1:
            fprintf(stderr, "Lexical error: Unexpected character or series of characters. [Line %i]\n", Line);
            break;
        case 2:
            fprintf(stderr, "Syntactical error. [Line %i]\n", Line);
            break;
        case 3:
            fprintf(stderr, "Semantic Error: Undefined function or redefinition of function. [Line %i]\n", Line);
            break;
        case 4:
            fprintf(stderr, "Semantic Error: Wrong amount/datatype of parameters passed to function or wrong datatype or return variable. [Line %i]\n", Line);
            break;
        case 5:
            fprintf(stderr, "Semantic Error: Undefined variable. [Line %i]\n", Line);
            break;
        case 6:
            fprintf(stderr, "Semantic Error: Missing/Leftover expression in function return statement. [Line %i]\n", Line);
            break;
        case 7:
            fprintf(stderr, "Semantic Error: Using incompatible types in aritmetical, logical or string exression. [Line %i]\n", Line);
            break;
        case 8:
            fprintf(stderr, "Semantic Error. [Line %i]\n", Line);
            break;
        case 99:
            fprintf(stderr, "Internal program error: Try to restart and run again. [Line %i]\n", Line);
            break;
        default:
            break;
    }
}