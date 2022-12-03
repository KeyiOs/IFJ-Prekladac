/**
 * IFJ Projekt 2022
 * @author <xkento00> Samuel Kentoš
 */

#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

#define _TOKEN_ struct Token

#ifndef SKENER_CHECK
#define SKENER_CHECK

int Prolog(_WRAP_ *Wrap);
int Comment(_WRAP_ *Wrap);
int Strings(_WRAP_ *Wrap, char *String, int Type);
int Scan(_WRAP_ *Wrap);

#endif