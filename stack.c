/**
 * IFJ Projekt 2021
 * @author <xkento00> Samuel Kentos
 */

#include "error_handler.h"
#include "skener.h"
#include "stack.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define __TOKEN__ struct Token_t

void Stack_Init ( __TOKEN__ *Stack ) {
    Stack->Key = 0;
    Stack->Next = NULL;
    Stack->Previous = NULL;
    Stack->Property.Keyword = 0;
    Stack->Property.String = NULL;
}

__TOKEN__ *Stack_Create ( ) {
    __TOKEN__ *Stack_New = malloc ( sizeof ( __TOKEN__ ));
    if ( Stack_New == NULL ) return NULL;
    Stack_Init ( Stack_New );
    return Stack_New;
}

__TOKEN__ *Stack_Push ( __TOKEN__ *Stack, __TOKEN__ *Token ) {
    if ( Stack->Key == 0 ) {
        Stack->Key = Token->Key;
        if ( Token->Key == TOKEN_TYPE_IDENTIFICATOR || Token->Key == TOKEN_TYPE_STRING ) Stack->Property.String = Token->Property.String;
        else if ( Token->Key == TOKEN_TYPE_INT ) Stack->Property.Integer = Token->Property.Integer;
        else if ( Token->Key == TOKEN_TYPE_NUMBER ) Stack->Property.Decimal = Token->Property.Decimal;
        else if ( Token->Key == TOKEN_TYPE_KEYWORD ) Stack->Property.Keyword = Token->Property.Keyword;
    } else {
        __TOKEN__ *Stack_New = Stack_Create ( );
        if ( !Stack_New) {
            Error_Handler ( 99 );
            return NULL;
        }
        Stack_New->Key = Stack->Key;
        if ( Stack->Key == TOKEN_TYPE_IDENTIFICATOR || Stack->Key == TOKEN_TYPE_STRING ) Stack_New->Property.String = Stack->Property.String;
        else if ( Stack->Key == TOKEN_TYPE_INT ) Stack_New->Property.Integer = Stack->Property.Integer;
        else if ( Stack->Key == TOKEN_TYPE_NUMBER ) Stack_New->Property.Decimal = Stack->Property.Decimal;
        else if ( Stack->Key == TOKEN_TYPE_KEYWORD ) Stack_New->Property.Keyword = Stack->Property.Keyword;
        Stack_New->Next = Stack->Next;
        Stack->Next = Stack_New;
        Stack->Key = Token->Key;
        if ( Token->Key == TOKEN_TYPE_IDENTIFICATOR || Token->Key == TOKEN_TYPE_STRING ) Stack->Property.String = Token->Property.String;
        else if ( Token->Key == TOKEN_TYPE_INT ) Stack->Property.Integer = Token->Property.Integer;
        else if ( Token->Key == TOKEN_TYPE_NUMBER ) Stack->Property.Decimal = Token->Property.Decimal;
        else if ( Token->Key == TOKEN_TYPE_KEYWORD ) Stack->Property.Keyword = Token->Property.Keyword;
    }
    return Stack;
}

__TOKEN__ *Stack_Pop ( __TOKEN__ *Stack ) {
    if ( Stack->Next == NULL ) {
        Stack_Init ( Stack );
    } else {
        __TOKEN__ *Stack_Tmp = Stack->Next;
        Stack->Key = Stack->Next->Key;
        if ( Stack->Next->Key == TOKEN_TYPE_IDENTIFICATOR || Stack->Next->Key == TOKEN_TYPE_STRING ) Stack->Property.String = Stack->Next->Property.String;
        else if ( Stack->Key == TOKEN_TYPE_INT ) Stack->Property.Integer = Stack->Next->Property.Integer;
        else if ( Stack->Key == TOKEN_TYPE_NUMBER ) Stack->Property.Decimal = Stack->Next->Property.Decimal;
        else if ( Stack->Key == TOKEN_TYPE_KEYWORD ) Stack->Property.Keyword = Stack->Next->Property.Keyword;
        if ( Stack->Next->Next ) Stack->Next = Stack->Next->Next;
        else Stack->Next = NULL;
        free ( Stack_Tmp );
        Stack_Tmp = NULL;
    }
    return Stack;
}