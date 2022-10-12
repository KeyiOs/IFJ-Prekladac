/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "error_handler.h"

#include <stdlib.h>
#include <stdio.h>

void ERR_Handler(int ERR) {
    switch(ERR) {
        case 1:
            fprintf(stderr, "Lexical error: Unexpected character or series of characters.\n");
            break;
        case 2:
            fprintf(stderr, "Syntactical error.\n");
            break;
        case 3:
            fprintf(stderr, "Semantic Error: Undefined function/variable or redefinition of function/variable.\n");
            break;
        case 4:
            fprintf(stderr, "Semantic Error: Wrong amount/datatype of parameters passed to function or wrong datatype or return variable.\n");
            break;
        case 5:
            fprintf(stderr, "Semantic Error: Undefined function/variable\n");
            break;
        case 6:
            fprintf(stderr, "Semantic Error: Missing/Leftover expression in function return statement\n");
            break;
        case 7:
            fprintf(stderr, "Semantic Error: Using incompatible types in aritmetical, logical or string exression\n");
            break;
        case 8:
            fprintf(stderr, "Semantic Error\n");
            break;
        case 99:
            fprintf(stderr, "Internal program error: Try to restart and run again\n");
            break;
        default:
            break;
    }
}