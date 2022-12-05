/**
 * IFJ Projekt 2022
 * @author <xhorac20> Andrej Horacek
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generator.h"

int IntStack[256];
int top = -1;

// _________________________________________Pomocne Funckie_____________________________________________________________

void my_push(int ID) {
    top = top + 1;
    IntStack[top] = ID;
}

void my_pop() {
    top = top - 1;
}

// _________________________________________Funckie na Generovanie______________________________________________________


void G_BigStart() {
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("CALL $$main\n");
    printf("JUMP $$big_end\n");

    G_reads();
    G_readi();
    G_readf();
    G_write();
    G_floatval();
    G_intval();
    G_strval();
    G_strlen();
    G_substring();
    G_ord();
    G_chr();

    printf("\n# main body\n");
    printf("LABEL $$main\n");
}

void G_BigEnd() {
    printf("LABEL $$big_end\n");
    printf("EXIT int@0\n");
}

void G_CallParam(_STACK_ *local_st, _ITEMF_ *Table) {
    if (local_st == NULL) return;
    while (local_st->Token.Type != T_TYPE_NULL) {
        G_PushParams(local_st, Table);
        local_st = Stack_Pop(local_st);
    }
}

void G_PushParams(_STACK_ *local_st, _ITEMF_ *Table) {
    switch (local_st->Token.Type) {
        case T_TYPE_INT_DATATYPE:
            printf("PUSHS int@%s\n", local_st->Token.String); // TODO:
            break;
        case T_TYPE_FLOAT_DATATYPE:
            printf("PUSHS float@%a\n", atof(local_st->Token.String)); // TODO:
            break;
        case T_TYPE_VARIABLE:{
            _ITEMV_ *ItemV = SearchV(&Table->Local, local_st->Token.String);
            printf("PUSHS LF@%s$%d\n", local_st->Token.String, ItemV->Dive);
            break;
        }
        case T_TYPE_STRING_DATATYPE:
            printf("PUSHS string@%s\n", local_st->Token.String); // TODO:
            break;
        default:
            break;
    }
}

void G_CallStart(char *function, int count_of_vars) {
    printf("CREATEFRAME\n");
    printf("CLEARS\n");
    if (strcmp(function, "write") == 0) {
        printf("DEFVAR TF@write$0\n");
        printf("MOVE TF@write$0 int@%d\n", count_of_vars);
    }
}

void G_Call(char *function) {
    printf("CALL func$%s\n", function);
}

void G_LABEL_start(char *label) {
    printf("LABEL func$%s\n", label);
    printf("PUSHFRAME\n");
}

void G_LABEL_end() {
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_StartFunction(char *function) {
    printf("\n#Start of function A %s\n", function);
    G_LABEL_start(function);
}

void G_EndFunction(char *function) {
    G_LABEL_end();
    printf("#End of function %s\n", function);
}

void G_Param(_PARAM_ *param) {
    if (param == NULL) return;
    while(param != NULL) {
        printf("DEFVAR LF@%s$1\n", param->Name);
        printf("POPS LF@%s$1\n", param->Name);
        param = param->Next;
    }
    printf("DEFVAR LF@retval$1\n");
}

void G_IfGen(int ID) {
    printf("#IF $if$%d\n", ID);
}

void G_IfStart(int ID) {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $if$%d$else$\n", ID);
    my_push(ID);
}

void G_Else() {
    printf("JUMP $if$%d$end\n", IntStack[top]);
    printf("LABEL $if$%d$else$\n", IntStack[top]);
}

void G_IfEnd() {
    printf("LABEL $if$%d$end\n", IntStack[top]);
    my_pop();
}

void G_WhileStart(int ID) {
    printf("LABEL WHILE$CHECK$%d\n", ID);
}

void G_WhileJump(int ID) {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS WHILE$END$%d\n", ID);
    my_push(ID);
}

void G_WhileEnd() {
    printf("JUMP WHILE$CHECK$%d\n", IntStack[top]);
    printf("LABEL WHILE$END$%d\n", IntStack[top]);
    my_pop();
}

void G_Return(){
    printf("POPFRAME\n");
    printf("RETURN\n");
}

/* void G_DefVar(char *Name, int Dive, bool in_for) {
    if (in_for) printf("MOVE LF@%s$%d nil@nil\n", Name, Dive);
    else printf("DEFVAR LF@%s$%d\n", Name, Dive);
} */

/* void G_AddToVariable(char *var_name, int scope) {
    char *tmp = malloc(strlen(var_name) + 1);
    if (tmp == NULL) exit(99);
    if (scope < 0) sprintf(tmp, "%s", var_name);
    else sprintf(tmp, "%s$%i", var_name, scope);
    InsertFirstString(&Vars, tmp);
} */

/* void G_AddToExponent(char *exp, bool in_for) {
    char *tmp = malloc(strlen(exp) + 1);
    strcpy(tmp, exp);
    if (in_for)
        InsertFirstString(&Exps, tmp);
    else
        InsertLastString(&Exps, tmp);
} */

/* void G_ForAss(int NumberOfVariables) {
    if (NumberOfVariables <= 0)
        return;
    printf("CREATEFRAME\n");
    for (int j = 0; j < NumberOfVariables; j++) {
        printf("%s", Exps.First->data);
        DeleteFirstString(&Exps);
        printf("DEFVAR TF@$tmp$%d\n", ID);
        printf("POPS TF@$tmp$%d\n", ID);
        my_push();
    }
    for (int i = 0; i < NumberOfVariables; i++) {
        if (strcmp(Vars.First->data, "_") == 0) {
            DeleteFirstString(&Vars);
            continue;
        }
        printf("MOVE LF@%s TF@$tmp$%d\n", Vars.First->data, IntStack[top]);
        DeleteFirstString(&Vars);
        my_pop();
    }
} */

// _________________________________________Vstavane funkcie IFJ22______________________________________________________

void G_reads() {
    printf("#FUNCTION READS\n\n");
    printf("LABEL func$reads\n");
    printf("PUSHFRAME\n");                           //  TF -> LF

    printf("DEFVAR LF@retval$1\n");                         //  LF@retval1
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
    printf("MOVE LF@retval$1 LF@param1\n");             // LF@retval$1 = LF@param1
    printf("POPFRAME\n");                               // LF -> TF
    printf("RETURN\n");                                 // RETURN na CALL s TF

    printf("LABEL $ERROR$READS\n");
    printf("MOVE LF@retval$1 nil@nil\n");               // LF@retval$1 = nil@nil
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_readi() {
    printf("#FUNCTION READI\n\n");
    printf("LABEL func$readi\n");
    printf("PUSHFRAME\n");                      //  TF -> LF

    printf("DEFVAR LF@retval$1\n");
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
    printf("DEFVAR LF@type$var\n");

    printf("LABEL $while$write\n");                              // Cyklus START
    printf("JUMPIFEQ while$end TF@write$0 int@0\n");             // TF@write$0 = 0 then jump
    printf("POPS LF@write$var\n");
    printf("TYPE LF@type$var LF@write$var\n");

    printf("JUMPIFEQ $write$int string@int LF@type$var\n");
    printf("JUMPIFEQ $write$float string@float LF@type$var\n");
    printf("JUMPIFEQ $write$null string@nil LF@type$var\n");

    printf("LABEL $write$ \n");                                 //  WRITE
    printf("WRITE LF@write$var\n");
    printf("SUB TF@write$0 TF@write$0 int@1\n");
    printf("JUMP $while$write\n");                              // Cyklus END

    printf("LABEL $write$int\n");       // TODO: int bude vytištěna pomocí '%d'
    printf("MOVE LF@write$var LF@write$var\n");
    printf("JUMP $write$\n");

    printf("LABEL $write$float\n");     // TODO float pak pomocí '%a'
    printf("MOVE LF@write$var LF@write$var\n");
    printf("JUMP $write$\n");

    printf("LABEL $write$null\n");
    printf("MOVE LF@write$var string@ \n");
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
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

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
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

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
    printf("MOVE LF@retval$1 float@0x0.0p+0\n");
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
    printf("DEFVAR LF@type$var\n");
    printf("TYPE LF@type$var LF@$1\n");

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

    printf("DEFVAR LF@retval$1\n");
    printf("POPS LF@retval$1\n");
    printf("STRLEN LF@retval$1 LF@retval$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_substring(){
    printf("LABEL func$substring\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@j\n");
    printf("DEFVAR LF@n\n");
    printf("DEFVAR LF@check\n");

    printf("DEFVAR LF@retval$1\n");

    printf("DEFVAR LF@$bool\n");
    printf("DEFVAR LF@$len\n");
    printf("DEFVAR LF@$tmpstring\n");

    printf("POPS LF@string\n");
    printf("POPS LF@i\n");
    printf("POPS LF@j\n");
    printf("SUB LF@n LF@j  LF@i\n");
    printf("ADD LF@n  LF@n int@1\n");

    printf("GT LF@check LF@i LF@j\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("STRLEN LF@$len LF@string\n");
    printf("GT LF@check LF@j LF@$len\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("EQ LF@check LF@i LF@$len\n");
    printf("JUMPIFEQ $substring$error LF@check bool@true\n");

    printf("MOVE LF@retval$1 string@\n");

    printf("CLEARS\n");
    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("GTS\n");
    printf("PUSHS LF@i\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("ANDS\n");

    printf("PUSHS LF@n\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("ANDS\n");

    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$error LF@$bool bool@false\n");

    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("SUBS\n");
    printf("PUSHS LF@n\n");
    printf("LTS\n");

    printf("POPS LF@$bool\n");
    printf("ADD LF@n LF@n LF@i #defaultvalue\n");
    printf("JUMPIFEQ $substring$lts bool@false LF@$bool\n");
    printf("MOVE LF@n LF@$len\n");
    printf("LABEL $substring$lts\n");


    printf("LABEL $substring$whilestart\n");
    printf("PUSHS LF@i\n");
    printf("PUSHS LF@n\n");
    printf("LTS\n");
    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$end LF@$bool bool@false\n");

    printf("GETCHAR LF@$tmpstring LF@string LF@i\n");
    printf("CONCAT LF@retval$1 LF@retval$1 LF@$tmpstring\n");

    printf("ADD LF@i LF@i int@1\n");
    printf("JUMP $substring$whilestart\n");

    printf("LABEL $substring$error\n");
    printf("MOVE LF@retval$1 nil@nil\n");

    printf("LABEL $substring$end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_ord(){
    printf("#FUNCTION ORD\n");
    printf("LABEL func$ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ord$string\n");
    printf("DEFVAR LF@ord$int\n");
    printf("DEFVAR LF@retval$1\n");
    printf("POPS LF@ord$string\n");
    printf("MOVE LF@ord$int int@0\n");
    printf("DEFVAR LF@error$check\n");
    printf("PUSHS LF@ord$int\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("DEFVAR LF@$lenght$ord\n");
    printf("STRLEN LF@$lenght$ord LF@ord$string\n");
    printf("PUSHS LF@ord$int\n");
    printf("PUSHS LF@$lenght$ord\n");
    printf("LTS\n");
    printf("ANDS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $ERROR$ORD\n");
    printf("STRI2INT LF@retval$1 LF@ord$string LF@ord$int\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $ERROR$ORD\n");
    printf("MOVE LF@retval$1 int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_chr() {
    printf("#FUNCTION CHR\n\n");
    printf("LABEL func$chr\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@chr$int\n");
    printf("DEFVAR LF@retval$1\n");
    printf("POPS LF@chr$int\n");

    printf("INT2CHAR LF@retval$1 LF@chr$int\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}