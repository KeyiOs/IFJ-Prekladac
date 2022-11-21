/**
 * IFJ Projekt 2021
 * @author <xhorac20> Andrej Horacek
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generator.h"

unsigned int ID = 0;
unsigned int elseCounter = 0;
StringList ListOfStrings;
StringList Vars;
StringList Exps;

int IntStack[1000];
int top = -1;

// _________________________________________Pomocne Funckie_____________________________________________________________

void my_push() {
    top = top + 1;
    IntStack[top] = ID;
    ID++;
}

void my_pop() {
    top = top - 1;
}

// _________________________________________Funckie na Generovanie______________________________________________________

void G_BigStart() {

    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("CALL $function$main\n");
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
}

void G_BigEnd() {
    printf("LABEL $$big_end\n");
    printf("EXIT int@0\n");
}

/*
void G_defvar(char* id,int scope, bool in_for){
    if(in_for){
        printf("MOVE LF@%s$%d nil@nil\n",id, scope);
    }else
    {
        printf("DEFVAR LF@%s$%d\n",id, scope);
    }

}

void gen_retvals(int number_of_return_values){
    for(int i = 1; i <= number_of_return_values; i++)
        printf("DEFVAR LF@retval$%d\n", i);
}

void gen_params(string* params){
    if(params->len <= 0)
        return;
    printf("DEFVAR LF@");
    for(int i = 0; i < params->len-1; i++){
        if(params->str[i] != '#'){
            printf("%c", params->str[i]);
        }else
        {
            printf("$0\nDEFVAR LF@");
        }
    }
    printf("$0\n");
    char tmpchar[params->len];
    int j = 0;
    for(int i = 0 ; i < params->len; i++){
        if(params->str[i] != '#'){
            tmpchar[j++] = params->str[i];
        }else
        {
            tmpchar[j] = '\0';
            printf("POPS LF@%s$0\n", tmpchar);
            j = 0;
        }
    }
}

void gen_call_params(token_t *last, st_stack_t *local_st) {
    token_t* prev = last;
    if (prev == NULL)
        return;
    while(prev->type != TOKEN_LBRACKET){
        gen_pushs_param(prev->type, &prev->actual_value, local_st);
        prev = prev->prev;
    }

}
void gen_pushs_param(token_type type, string *value, st_stack_t *local_st){
    st_item* item;
    switch (type)
    {
        case TOKEN_INTEGER:
            printf("PUSHS int@%s\n", value->str);
            break;

        case TOKEN_FLOAT:
            printf("PUSHS float@%a\n", Str_to_Float(value));
            break;
        case TOKEN_ID:
            item = stack_lookup(local_st, value);
            printf("PUSHS LF@%s$%d\n", value->str, item->data.scope);
            break;

        case TOKEN_STR:
            printf("PUSHS string@%s\n", value->str);
            break;

        default:
            break;
    }

}

void gen_add_to_vars(char *var_name, int scope) {
    char* tmp = malloc(strlen(var_name)+BLOCK_SIZE);
    if(tmp == NULL)
        exit(99);
    if(scope < 0)
        sprintf(tmp,"%s",var_name);
    else
        sprintf(tmp,"%s$%i",var_name,scope);
    InsertFirstString(&Vars, tmp);
}

void gen_add_to_exp(char *exp, bool in_for) {
    char* tmp = malloc(strlen(exp)+1);
    strcpy(tmp,exp);
    if(in_for)
        InsertFirstString(&Exps, tmp);
    else
        InsertLastString(&Exps, tmp);
}

void gen_for_assign(int NumberOfVariables){
    if(NumberOfVariables <= 0)
        return;
    printf("CREATEFRAME\n");
    for(int j = 0; j < NumberOfVariables; j++){
        printf("%s", Exps.First->data);
        DeleteFirstString(&Exps);
        printf("DEFVAR TF@$tmp$%d\n",ID);
        printf("POPS TF@$tmp$%d\n", ID);
        my_push();
    }
    for(int i = 0; i < NumberOfVariables; i++){
        if(strcmp(Vars.First->data, "_") == 0){
            DeleteFirstString(&Vars);
            continue;
        }
        printf("MOVE LF@%s TF@$tmp$%d\n",Vars.First->data, IntStack[top]);
        DeleteFirstString(&Vars);
        my_pop();
    }
}

void gen_assign(int NumberOfVariables, bool in_for) {
    int tmp_top;
    if(NumberOfVariables <= 0)
        return;
    printf("CREATEFRAME\n");
    for(int j = 0; j < NumberOfVariables; j++){
        printf("%s", Exps.First->data);
        DeleteFirstString(&Exps);
        printf("DEFVAR TF@$tmp$%d\n",ID);
        printf("POPS TF@$tmp$%d\n", ID);
        my_push();
    }
    tmp_top = top-NumberOfVariables+1;
    for(int i = 0; i < NumberOfVariables; i++){
        if(strcmp(Vars.First->data, "_") == 0){
            DeleteFirstString(&Vars);
            continue;
        }
        printf("MOVE LF@%s TF@$tmp$%d\n",Vars.First->data, IntStack[in_for?tmp_top++:top]);
        DeleteFirstString(&Vars);
        my_pop();
    }

}

void gen_assign_return(int NumberOfVariables){
    for(int i = NumberOfVariables; i > 0; i--){
        if(strcmp(Vars.First->data, "_") == 0){
            DeleteFirstString(&Vars);
            continue;
        }
        printf("MOVE LF@%s TF@retval$%i\n",Vars.First->data, i);
        DeleteFirstString(&Vars);
    }
}

void gen_set_retvals(int NumberOfReturns, bool in_for) {

    if (!in_for){
        for(int i = 1; i <= NumberOfReturns; i++){
            printf("%s", Exps.First->data);
            DeleteFirstString(&Exps);
            printf("POPS LF@retval$%i\n",i);
        }
    }else{
        for(int i = NumberOfReturns; i >= 1; i--){
            printf("%s", Exps.First->data);
            DeleteFirstString(&Exps);
            printf("POPS LF@retval$%i\n",i);
        }
    }
}
*/

void G_for_start(char *expression) {
    printf("LABEL CHECK$FOR$%d\n", ID);
    printf("%s", expression);
    G_for_jump();
}

void G_for_jump() {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS END$FOR$%d\n", ID);
    my_push();
}

void G_for_end() {
    StringElementPtr tmp = Vars.First;
    int count = 0;
    while (tmp != NULL) {
        tmp = tmp->ptr;
        count++;
    }
    printf("JUMP CHECK$FOR$%d\n", IntStack[top]);
    printf("LABEL END$FOR$%d\n", IntStack[top]);
    my_pop();
}

void G_call_start(char *function, int count_of_vars) {
    printf("CREATEFRAME\n");
    printf("CLEARS\n");
    if (strcmp(function, "print") == 0) {
        printf("DEFVAR TF@$0\n");
        printf("MOVE TF@$0 int@%d\n", count_of_vars);
    }
}

void G_call(char *function) {
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

void G_start_of_function(char *function) {
    printf("#Start of function\n");
    G_LABEL_start(function);
}

void G_end_of_function() {
    printf("#End of function\n");
    G_LABEL_end();
}


void G_if_start(char *expression) {
    printf("#IF $if$%d\n", ID);
    printf("%s", expression);
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $if$%d$else$%d\n", ID, elseCounter);
    my_push();
}

void G_else() {
    printf("JUMP $if$%d$end\n", IntStack[top]);
    printf("LABEL $if$%d$else$%d\n", IntStack[top], elseCounter);
}

void G_if_end() {
    printf("LABEL $if$%d$end\n", IntStack[top]);
    my_pop();
    elseCounter = 0;
}

// _________________________________________Vstavane funkcie IFJ22______________________________________________________

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

    printf("DEFVAR LF@retval$1\n");
    printf("POPS LF@retval$1\n");
    printf("STRLEN LF@retval$1 LF@retval$1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_substring() {
    printf("#FUNCTION SUBSTRING\n\n");
    printf("LABEL func$floatval\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@j\n");

    printf("POPS LF@string\n");         // Plnenie premennych zo steku
    printf("POPS LF@i\n");
    printf("POPS LF@j\n");

    printf("DEFVAR LF@retval$1\n");

    printf("DEFVAR LF@$bool\n");
    printf("DEFVAR LF@$len\n");
    printf("DEFVAR LF@$tmpstring\n");

    printf("STRLEN LF@$len LF@string\n");
    printf("MOVE LF@retval$1 nil@nil\n");

    printf("CLEARS\n");                 // vycistenie datoveho zasobniku
    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("GTS\n");                    // len > i ?
    printf("PUSHS LF@i\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");                    // i < 0
    printf("NOTS\n");
    printf("ANDS\n");

    printf("PUSHS LF@j\n");
    printf("PUSHS int@0\n");
    printf("LTS\n");                    // j < 0
    printf("NOTS\n");
    printf("ANDS\n");

    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$end LF@$bool bool@false\n");

    printf("PUSHS LF@$len\n");
    printf("PUSHS LF@i\n");
    printf("SUBS\n");                   // len - i
    printf("PUSHS LF@j\n");
    printf("LTS\n");                    // (len - i) < j

    printf("POPS LF@$bool\n");
    printf("ADD LF@j LF@j LF@i\n");
    printf("JUMPIFEQ $substring$lts bool@false LF@$bool\n");
    printf("MOVE LF@j LF@$len\n");
    printf("LABEL $substring$lts\n");


    printf("LABEL $substring$whilestart\n");                   // cyklus start
    printf("PUSHS LF@i\n");
    printf("PUSHS LF@j\n");
    printf("LTS\n");                    // i < j
    printf("POPS LF@$bool\n");
    printf("JUMPIFEQ $substring$end LF@$bool bool@false\n");

    printf("GETCHAR LF@$tmpstring LF@string LF@i\n");
    printf("CONCAT LF@retval$1 LF@retval$1 LF@$tmpstring\n");   // retval$1 = retval$1 + tmpstring

    printf("ADD LF@i LF@i int@1\n");    // i = i + 1
    printf("JUMP $substring$whilestart\n");                        // cyklus end

    printf("label $substring$end\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");
}

void G_ord() {
    printf("#FUNCTION ORD\n\n");
    printf("LABEL func$ord\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@ord$string\n");
    printf("DEFVAR LF@ord$int\n");
    printf("DEFVAR LF@retval$1\n");
    printf("MOVE LF@retval$1 int@0\n");

    printf("POPS LF@ord$string\n");
    printf("POPS LF@ord$int\n");
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
    printf("JUMPIFNEQS $END$ORD\n");
    printf("STRI2INT LF@retval$1 LF@ord$string LF@ord$int\n");

    printf("LABEL $END$ORD\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void G_chr() {
    printf("#FUNCTION CHR\n\n");
    printf("LABEL func$chr\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@chr$int\n");
    printf("DEFVAR LF@retval$1\n");
    printf("INT2CHAR LF@retval$1 LF@chr$int\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}