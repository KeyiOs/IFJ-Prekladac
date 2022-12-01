/**
 * IFJ Projekt 2022
 * @author Jakub Brciak
 */

#include "symtable.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAMES 5
#define NAME_LEN 10
#define STR_LEN 20

/**
 295 parser.c -> Zistujes ci je to priradenie, ked je tak mozes vlozit premennu do symtab

 Ked chces zapisat Variable do SymTable musis jej dat aj datovy typ, kludne aj na surovo T_TYPE_NULL_DATATYP
**/

/// ------------------------------------------------------------------------------ VARIABLE ----------------------------

void FreeItemV(_ITEMV_ *Item) {      // Vyuzije sa aj pri odstranovani prvku
    free(Item->Name);
    free(Item);
    Item = NULL;
}

// Uvolni alokovanu pamat tabulka premennych
_ITEMV_ *FreeV(_ITEMV_ *Item) {
    if(Item != NULL) {
        FreeV(Item->Left);
        FreeV(Item->Right);
        FreeItemV(Item);
        Item = NULL;
    }
    return NULL;
}

// Inicializuje Premennu
_ITEMV_ *InitV(char *Name, Token_Type Type) {
    _ITEMV_ *Item = malloc(sizeof(_ITEMV_));
    if (Item != NULL) {
        if((Item->Name = (char *) malloc(strlen(Name) + 1)) == NULL) return NULL;
        strcpy(Item->Name, Name);
        Item->Type = Type;
        Item->Init = 0;
        Item->Dive = 0;
        Item->Left = NULL;
        Item->Right = NULL;
    } else return NULL;
    return Item;
}

// Vlozi premennu do tabulky, vracia TRUE/FALSE ci sa podarilo
int InsertV(_ITEMV_ **ItemPtr, char *Name, Token_Type Type) {
    _ITEMV_ *Item = *ItemPtr;
    if (Item == NULL) {
        if(((*ItemPtr) = InitV(Name, Type)) == NULL) return 99;
        else return 0;
    } else if (strcmp(Name, Item->Name) < 0){
        return InsertV(&(Item->Left), Name, Type);
    } else if (strcmp(Name, Item->Name) > 0){
        return InsertV(&(Item->Right), Name, Type);
    } else return 1;
}

// Vrat premennu, ak nenajde vytvori novu
_ITEMV_ *GetV(_ITEMV_ **ItemPtr, char *Name, Token_Type Type) {
    _ITEMV_ *Item = *ItemPtr;
    if(Item == NULL) {
        if(((*ItemPtr) = InitV(Name, Type)) == NULL) return NULL;
        else return (*ItemPtr);
    }

    if(!strcmp(Name, Item->Name)) {
        return Item;
    } else if (strcmp(Name, Item->Name) < 0) {
        return GetV((&Item->Left), Name, Type);
    } else if (strcmp(Name, Item->Name) > 0) {
        return GetV(&(Item->Right), Name, Type);
    } else return NULL;
}

// Najdi a vrat premennu, ked nenajde NULL
_ITEMV_ *SearchV(_ITEMV_ **ItemPtr, char *Name) {
    _ITEMV_ *Item = *ItemPtr;
    if(Item == NULL) return NULL;

    int CMP = strcmp(Name, Item->Name);
    if(CMP == 0) {
        return Item;
    } else if (CMP < 0) {
        return SearchV((&Item->Left), Name);
    } else if (CMP > 0) {
        return SearchV(&(Item->Right), Name);
    } else return NULL;
}

_ITEMV_ *ReplaceItem(_ITEMV_ *ItemPtr){
    _ITEMV_ *Item = ItemPtr;
    while(Item && Item->Left != NULL) Item = Item->Left;
    return Item;
}

// Zatial to je ze musis poslat odkaz &Item->Lokal
// Odstranenie premennej, pre funkcie to nemusi byt tie sa odstranovat nebudu
// Toto je este v procese ale funguje
_ITEMV_ *DeleteV(_ITEMV_ **ItemPtr, char *Name) {
    _ITEMV_ *Item = *ItemPtr;
    if(Item == NULL) return NULL;
    if (strcmp(Name, Item->Name) < 0){
        return DeleteV((&Item->Left), Name);
    }
    else if (strcmp(Name, Item->Name) > 0){
        return DeleteV(&(Item->Right), Name);
    } else {
        if(Item->Left == NULL){
            _ITEMV_ *Del = Item->Right;
            FreeItemV(Item);
            Item = NULL;
            return Del;
        }
        else if(Item->Right == NULL){
            _ITEMV_ *Del = Item->Left;
            FreeItemV(Item);
            Item = NULL;
            return Del;
        }
        _ITEMV_ *Del = ReplaceItem(Item->Right);

        // Okopiruj data z prenasaneho prvku
        Item->Type = Del->Type;
        strcpy(Item->Name, Del->Name);
        Item->Dive = Del->Dive;
        Item->Init = Del->Init;

        Item->Right = DeleteV(&Item->Right, Del->Name);
    }
    return Item;
}

/// ------------------------------------------------------------------------------ VARIABLE ----------------------------

/// ------------------------------------------------------------------------------- FUNKCIA ----------------------------

/// -------------------------------------------------- PARAMS --------------------- FUNKCIA ----------------------------

// Uvolni alokovanu pamat pre parametre
_PARAM_ *FreeParam(_PARAM_ *Params) {
    if(Params != NULL) {
        FreeParam(Params->Next);
        free(Params->Name);
        free(Params);
        Params = NULL;
    }
    return NULL;
}

// Inicializuj Parameter funkcie
_PARAM_ *InitParam(char *Name, Token_Keyword Type) {
    _PARAM_ *Params = malloc(sizeof(_PARAM_));
    if(Params != NULL) {
        if((Params->Name = (char *) malloc(strlen(Name) + 1)) == NULL) return NULL;
        strcpy(Params->Name, Name);
        Params->Type = Type;
        Params->Next = NULL;
    } else return NULL;
    return Params;
}

int AddParam(char *Pname, Token_Keyword Type, _PARAM_ **ParamsPtr) {
    if(*ParamsPtr == NULL) {
        if((*ParamsPtr = InitParam(Pname, Type)) == NULL) return 99;
    }
    else AddParam(Pname, Type, &(*ParamsPtr)->Next);
    return 0;
}

// Vloz parameter funkcie do zaznamu funkcie
int InsertParam(char *Fname, char *Pname, Token_Keyword Type, _ITEMF_ **ItemPtr) {
    _ITEMF_ *Item = SearchF(ItemPtr, Fname);
    if(Item == NULL) return 99;
    int ERR = AddParam(Pname, Type, &Item->Params);
    if(ERR != 0) return 99;
    if(SearchV(&Item->Local, Pname) != NULL) return 8;
    InsertV(&Item->Local, Pname, Type - 16);
    return 0;
}

/// -------------------------------------------------- PARAMS --------------------- FUNKCIA ----------------------------

void FreeItemF(_ITEMF_ *Item) {
    free(Item->Name);
    if(Item->Local != NULL) Item->Local = FreeV(Item->Local);
    if(Item->Params != NULL) Item->Params = FreeParam(Item->Params);
    free(Item);
    Item = NULL;
}

// Uvolni alokovanu pamat pre tabulku funkcii / celu symtable
_ITEMF_ *FreeST(_ITEMF_ *Item) {
    if(Item != NULL) {
        FreeST(Item->Left);
        FreeST(Item->Right);
        FreeItemF(Item);
        Item = NULL;
    }
    return NULL;
}

// Inicializuje funkciu v tabulke
_ITEMF_ *InitF(_ITEMF_ **Root, _ITEMF_ **ItemPtr, char *Name) {
    _ITEMF_ *Item = malloc(sizeof(_ITEMF_));
    if (Item != NULL) {
        if((Item->Name = (char *) malloc(strlen(Name) + 1)) == NULL) return NULL;
        strcpy(Item->Name, Name);
        Item->Local = NULL;
        Item->Params = NULL;
        Item->Type = T_KEYWORD_EMPTY;
        Item->Root = *Root;
        Item->Left = NULL;
        Item->Right = NULL;
    } else return NULL;
    return Item;
}

// Vlozi funkciu do tabulky, vracia TRUE/FALSE ci sa podarilo
int InsertF(_ITEMF_ **Root, _ITEMF_ **ItemPtr, char *Name) {
    _ITEMF_ *Item = *ItemPtr;
    if (Item == NULL) {
        if(((*ItemPtr) = InitF(Root, ItemPtr, Name)) == NULL) return 99;
        return 0;
    } else if (strcmp(Name, Item->Name) < 0){
        return InsertF(Root, &(Item->Left), Name);
    } else if (strcmp(Name, Item->Name) > 0){
        return InsertF(Root, &(Item->Right), Name);
    } else return 1;
}

// Najdi a vrat funkciu, ked nenajde NULL 
_ITEMF_ *SearchF(_ITEMF_ **ItemPtr, char *Name) {
    _ITEMF_ *Item = *ItemPtr;
    if(Item == NULL) return NULL;

    int CMP = strcmp(Name, Item->Name);
    if(CMP == 0){
        return Item;
    } else if (CMP < 0){
        return SearchF((&Item->Left), Name);
    } else if (CMP > 0){
        return SearchF(&(Item->Right), Name);
    } else return NULL;
}

// Najdi funkciu a zapis navratovu hodnotu
int ReturnF(_ITEMF_ **ItemPtr, char *Name, Token_Keyword Type) {
    _ITEMF_ *Item = SearchF(ItemPtr, Name);
    if(Item == NULL) return 1;
    Item->Type = Type;
    return 0;
}