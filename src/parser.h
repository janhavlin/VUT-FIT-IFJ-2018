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
bool eol(TToken **);
bool then(TToken **);
bool telse(TToken **);
bool end(TToken **);
bool tdo(TToken **);
bool id(TToken **);
bool lbr(TToken **);
bool rbr(TToken **);

#endif //IFJ18_PARSER_H
/* end of parser.h */