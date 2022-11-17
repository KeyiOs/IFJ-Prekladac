/**
 * IFJ Projekt 2021
 * @author <xhorac20> Andrej Horacek
 */

#include <stdio.h>
#include <stdlib.h>                                             //GF - Global Frame
#include <string.h>                                             //TF - Temporery Frame
#include <stdbool.h>                                            //LF - Lokal Frame

#include "generator.h"

void G_BigStart() {
    printf(".IFJcode22\n");

    printf("CALL $function$main\n");
    printf("JUMP $$big_end\n");
}

void G_BigEnd() {
    printf("LABEL $$big_end\n");
    printf("EXIT int@0\n");
}

void G_reads() {
    printf("#FUNCTION READS\n\n");
    printf("LABEL function$reads\n");
    printf("PUSHFRAME\n");                           //  TF -> LF

    printf("DEFVAR LF@retval1\n");                         //  LF@retval1
    printf("DEFVAR LF@param1\n");                           //  LF@param1
    printf("READ LF@param1 string\n");                      //  LF@param1 = string zo vstupu
    printf("DEFVAR LF@errorCheck\n");                       //  LF@errorCheck
    printf("TYPE LF@errorCheck LF@param1\n");               //  LF@errorCheck = typ z LF@param1
    //  if (string@string != LF@errorCheck) -> label $ERROR$READS
    printf("JUMPIFNEQ $ERROR$READS string@string LF@errorCheck\n");
    printf("DEFVAR LF@strlen\n");                           //  LF@strlen
    printf("STRLEN LF@strlen LF@param1\n");                 //  LF@strlen = len(LF@param1)
    //  if (LF@strlen == 0) -> label $END$OF$READS
    printf("JUMPIFEQ $ERROR$READS LF@strlen int@0\n");
    printf("SUB LF@strlen LF@strlen int@1\n");                     //  LF@strlen = LF@strlen - 1
    printf("DEFVAR LF@getchar\n");                                 //  LF@getchar
    printf("GETCHAR LF@getchar LF@param1 LF@strlen\n");            //  LF@getchar = jeden znak z LF@param1 na LF@strlen pozicii
    //  if (LF@getchar != " ") -> label $END$OF$READS
    printf("JUMPIFNEQ $END$OF$READS LF@getchar string@\\010\n");
    printf("SETCHAR LF@param1 LF@strlen string@\\000\n");   // v LF@param1 prepise znak na pozici LF@strlen na string@\000

    printf("LABEL $END$OF$READS\n");
    printf("MOVE LF@retval1 LF@param1\n");             // LF@retval$1 = LF@param1
    printf("POPFRAME\n");                               // LF -> TF
    printf("RETURN\n");                                 // RETURN na CALL s TF

    printf("LABEL $ERROR$READS\n");
    printf("MOVE LF@retval1 nil@nil\n");               // LF@retval$1 = nil@nil
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_readi() {
    printf("#FUNCTION READI\n\n");
    printf("LABEL func$readi\n");
    printf("PUSHFRAME\n");                      //  TF -> LF

    printf("DEFVAR LF@retval1\n");
    printf("DEFVAR LF@param$1\n");
    printf("DEFVAR LF@error$check\n");
    printf("READ LF@param$1 int\n");                    // LF@param$1 = int zo standartneho vstupu
    printf("TYPE LF@error$check LF@param$1\n");         // LF@error$check = typ LF@param$1
    //  if ($ERROR$INPUTI != string@int) -> label $ERROR$READI
    printf("JUMPIFNEQ $ERROR$READI string@int LF@error$check\n");
    printf("MOVE LF@retval$1 LF@param$1\n");           // LF@retval$1 = LF@param$1
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $ERROR$READI\n");
    printf("MOVE LF@retval$1 nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_readf() {
    printf("#FUNCTION READF\n\n");
    printf("LABEL func$readf\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@retval$1\n");
    printf("DEFVAR LF@param$1\n");
    printf("DEFVAR LF@error$check\n");
    printf("READ LF@param$1 float\n");
    printf("TYPE LF@error$check LF@param$1\n");
    printf("JUMPIFNEQ $ERROR$READF string@float LF@error$check\n");
    printf("MOVE LF@retval$1 LF@param$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $ERROR$READF\n");
    printf("MOVE LF@retval$1 nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_write() {
    printf("#FUNCTION WRITE\n\n");
    printf("LABEL func$write\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@write$var\n");
    printf("DEFVAR LF@type$var");

    printf("LABEL $while$write\n");                              // Cyklus START
    printf("JUMPIFEQ while$end LF@$0 int@0\n");
    printf("POPS LF@write$var\n");
    printf("TYPE LF@type$var LF@write$var\n");

    printf("JUMPIFEQ $write$int string@int LF@type$var\n");
    printf("JUMPIFEQ $write$float string@float LF@type$float\n");
    printf("JUMPIFEQ $write$null string@nil LF@type$nil\n");

    printf("LABEL $write$ \n");                                 //  WRITE
    printf("WRITE LF@write$var\n");
    printf("SUB LF@$0 LF@$0 int@1\n");
    printf("JUMP $while$write\n");                              // Cyklus END

    printf("LABEL $write$int\n");       //TODO int bude vytištěna pomocí '%d'
    printf("MOVE LF@write$var \n");
    printf("JUMP $write$\n");

    printf("LABEL $write$float\n");     //TODO float pak pomocí '%a'
    printf("MOVE LF@write$var \n");
    printf("JUMP $write$\n");

    printf("LABEL $write$null\n");
    printf("MOVE LF@write$var string@\" \"\n");
    printf("JUMP $write$\n");

    printf("LABEL while$end\n");
    printf("CLEARS\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_floatval() {
    printf("#FUNCTION FLOATVAL\n\n");
    printf("LABEL func$floatval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@retval$1\n");
    printf("DEFVAR LF@type$var\"\n");
    printf("TYPE LF@type$var\" LF@$1\n");

    printf("JUMPIFEQ $floatval$null nil@nil LF@$1\n");
    printf("JUMPIFEQ $floatval$int string@int LF@type$var\n");
    printf("MOVE LF@retval$1 LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $floatval$int\n");
    printf("INT2FLOAT LF@retval$1 LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $floatval$null\n");
    printf("MOVE LF@retval$1 int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_intval() {
    printf("#FUNCTION INTVAL\n\n");
    printf("LABEL func$intval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@retval$1\n");
    printf("DEFVAR LF@type$var\"\n");
    printf("TYPE LF@type$var\" LF@$1\n");

    printf("JUMPIFEQ $intval$null nil@nil LF@$1\n");
    printf("JUMPIFEQ $intval$int string@float LF@type$var\n");
    printf("MOVE LF@retval$1 LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $intval$float\n");
    printf("FLOAT2INT LF@retval$1 LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $intval$null\n");
    printf("MOVE LF@retval$1 float@0.0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void G_strval() {
    printf("#FUNCTION STRVAL\n\n");
    printf("LABEL func$strval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@$1\n");           // LF@$1
    printf("POPS LF@$1\n");             // Vrchol zasobniku -> LF@$1
    printf("DEFVAR LF@retval$1\n");
    printf("DEFVAR LF@type$var\"\n");
    printf("TYPE LF@type$var\" LF@$1\n");

    printf("JUMPIFEQ $strval$null nil@nil LF@$1\n");
    printf("MOVE LF@retval$1 LF@$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $strval$null\n");
    printf("MOVE LF@retval$1 string@""\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void G_strlen() {
    printf("#FUNCTION STRLEN\n\n");
    printf("LABEL func$strlen\n");
    printf("PUSHFRAME\n");

}

void G_substring() {
    printf("#FUNCTION SUBSTRING\n\n");
    printf("LABEL func$floatval\n");
    printf("PUSHFRAME\n");

}