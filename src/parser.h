/**
	file name:		parser.h
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	29.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Heart of the compiler, includes main
*/
#ifndef IFJ18_PARSER_H
#define IFJ18_PARSER_H

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include "scanner.h"
#include "ifj_error.h"
#include "dyn_arr.h"
#include "psa.h" 			//TODO: TEMPORARILY COMMENTED
#include "s_table.h"

/**
 * Instead of keeping global variables in parser.c
 * or passing million of arguments between functions
 * this wrapper structure carries all the data needed 
 * for semantic analysis.
 */ 
typedef struct {
	FILE *file;
	TsymItem *mainLT;
	TsymItem *currLT;
	TsymItem *currGT;
	unsigned whileCounter;
	unsigned ifCounter;
	unsigned psaCounter;
	unsigned paramCounter;	//set to 0 before every function call and function def
	bool inWhile;
	bool inFunCall;			//flag for parameters to determine if we read them for definition or call, false means inFunDef
} TWrapper;

/* NON-TERMINALS */

int parse(FILE *, TsymItem *GT, TsymItem *LT);
int start(TToken **, TWrapper *);
int stat(TToken **, TWrapper *);
bool fundef(TToken **, TWrapper *);
bool stlist(TToken **, TWrapper *);
bool plist(TToken **, TWrapper *);
bool term(TToken **, TWrapper *);
int assorfun(TToken **, TWrapper *, string);
int assign(TToken **, TWrapper *, string);
int decideExprOrFunc(TToken **, TWrapper *, string, string);
bool pbody(TToken **, TWrapper *);

/* TERMINALS */

bool eol(TToken **, TWrapper *);
bool tif(TToken **, TWrapper *);
bool then(TToken **, TWrapper *);
bool telse(TToken **, TWrapper *);
bool tWhile(TToken **, TWrapper *);
bool tdo(TToken **, TWrapper *);
bool def(TToken **, TWrapper *);
bool end(TToken **, TWrapper *);
bool id(TToken **, TWrapper *);
bool tconst(TToken **, TWrapper *);
bool nil(TToken **, TWrapper *);
bool lbr(TToken **, TWrapper *);
bool comma(TToken **, TWrapper *);
bool rbr(TToken **, TWrapper *);
bool eq(TToken **, TWrapper *);

#endif //IFJ18_PARSER_H
/* end of parser.h */