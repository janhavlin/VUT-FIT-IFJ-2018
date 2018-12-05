/**
 *	file name:		ifj_error.h
 *	project:		VUT-FIT-IFJ-2018
 *	created:		17.10.2018
 *	last modified:	17.10.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *  modifications:  Jan Havlín  xhavli47@stud.fit.vutbr.cz
 *	
 *	description:	There are names of errors and function to print error
 */

#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#ifndef IFJ_ERROR
#define IFJ_ERROR
#define DEBUG 0

extern int errflg;
extern int linecnt;

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

/**
 * @brief Behaves as printf(), but used to print error to stderr
 */
void ifjErrorPrint( const char *fmt, ... );

#endif // IFJ_ERROR
