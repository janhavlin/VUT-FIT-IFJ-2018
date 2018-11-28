/*
*	error.h
*/

#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#ifndef IFJ_ERROR
#define IFJ_ERROR

extern int errflg;

#define PROGRAM_OK          0
// chyba v programu v ramci lexikalniho analyzatoru (chybna struktura aktualniho lexemu)
#define ERR_LEXICAL		    1	
// chyba v programu v ramci syntakticke analyzy (chybna syntaxe programu)
#define	ERR_SYNTAX		    2	
// semanticka chyba v programu (nedefinovana funkce/promenna, pokus o redefinovani funkce/promenne,...)
#define ERR_SEM_DEFINE		3	
// semanticka/behova chyba typove kompatibility v aritmetickych, retezovych a relacnich vyrazech 				
#define ERR_SEM_TYPE		4
// semanticka chyba v programu - spatny pocet parametru u volani funkce
#define ERR_SEM_PARAM		5
// ostatni semanticke chyby
#define ERR_SEM_OTHER		6
// behova chyba deleni nulou
#define ERR_DIV_ZERO		9
// interni chyba prekladace tj. neovlivnena vstupnim programem (napr. chyba alokace pameti, atd.)
#define ERR_RUNTIME		    99

// funkce pro vypis chtene chyby
void ifjErrorPrint( const char *fmt, ... );

#endif // IFJ_ERROR
