/**
	file name:		parser.h
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	26.11.2018
	
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

/* NON-TERMINALS */

void parserStart(FILE *, TsymItem *GT, TsymItem *LT);
bool start(TToken **);
bool stat(TToken **);
bool fundef(TToken **);
bool stlist(TToken **);
bool plist(TToken **);
bool term(TToken **);
bool assorfun(TToken **);
bool assign(TToken **);
bool decideExprOrFunc(TToken **);
bool pbody(TToken **);

/* TERMINALS */

bool eol(TToken **);
bool tif(TToken **);
bool then(TToken **);
bool telse(TToken **);
bool tWhile(TToken **);
bool tdo(TToken **);
bool def(TToken **);
bool end(TToken **);
bool id(TToken **);
bool tconst(TToken **);
bool nil(TToken **);
bool lbr(TToken **);
bool comma(TToken **);
bool rbr(TToken **);
bool eq(TToken **);

#endif //IFJ18_PARSER_H
/* end of parser.h */