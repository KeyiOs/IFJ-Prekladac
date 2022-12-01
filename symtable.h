/**
 * IFJ Projekt 2022
 * @author
 */
#ifndef SYM_CHECK
#define SYM_CHECK

#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/// -------------------------------------------------------- VARIABLE --------
void FreeItemV(_ITEMV_ *Item);

// Uvolni alokovanu pamat tabulka premennych
_ITEMV_ *FreeV(_ITEMV_ *Item);

// Inicializuje Premennu
_ITEMV_ *InitV(char *Name, Token_Type Type);

// Vlozi premennu do tabulky, vracia TRUE/FALSE ci sa podarilo
int InsertV(_ITEMV_ **ItemPtr, char *Name, Token_Type Type);

// Vrat premennu, ked nenajde NULL
_ITEMV_ *GetV(_ITEMV_ **ItemPtr, char *Name, Token_Type Type);

// Najdi a vrat premennu ak nenajde vytvori novu
_ITEMV_ *SearchV(_ITEMV_ **ItemPtr, char *Name);

_ITEMV_ *ReplaceItem(_ITEMV_ *ItemPtr);

// Zatial to je ze musis poslat odkaz &Item->Lokals
// Odstranenie premennej, pre funkcie to nemusi byt tie sa odstranovat nebudu
_ITEMV_ *DeleteV(_ITEMV_ **ItemPtr, char *Name);

/// -------------------------------------------------------- PARAM ----------
// Uvolni alokovanu pamat pre parametre
_PARAM_ *FreeParam(_PARAM_ *Params);

// Inicializuj Parameter funkcie
_PARAM_ *InitParam(char *Name, Token_Keyword Type);

int AddParam(char *Pname, Token_Keyword Type, _PARAM_ **ParamsPtr);

// Vloz parameter funkcie do zaznamu funkcie
int InsertParam(char *Fname, char *Pname, Token_Keyword Type, _ITEMF_ **ItemPtr);

/// ------------------------------------------------------- FUNCTION --------
void FreeItemF(_ITEMF_ *Item);

// Uvolni alokovanu pamat pre tabulku funkcii / celu symtable
_ITEMF_ *FreeST(_ITEMF_ *Item);

// Inicializuje funkciu v tabulke
_ITEMF_ *InitF(_ITEMF_ **Root, _ITEMF_ **ItemPtr, char *Name);

// Vlozi funkciu do tabulky, vracia TRUE/FALSE ci sa podarilo
int InsertF(_ITEMF_ **Root, _ITEMF_ **ItemPtr, char *Name);

// Najdi a vrat funkciu ak nenajde vytvori prvok
_ITEMF_ *SearchF(_ITEMF_ **ItemPtr, char *Name);

// Najdi funkciu a zapis navratovu hodnotu
int ReturnF(_ITEMF_ **ItemPtr, char *Name, Token_Keyword Type);

// TODO: Delete item
// TODO: Vyvazenost ?

#endif
