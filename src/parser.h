/**
	file name:		parser.h
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	25.11.2018
	
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
//#include "psa.h" 			TODO: TEMPORARILY COMMENTED
#include "s_table.h"

void parserStart(TsymItem *, TsymItem *, FILE *f);
bool start(TToken **, FILE *, TsymItem *, TsymItem *);
bool stat(TToken **, FILE *, TsymItem *, TsymItem *);
bool fundef(TToken **, FILE *, TsymItem *, TsymItem *);
bool stlist(TToken **, FILE *, TsymItem *, TsymItem *);
bool plist(TToken **, FILE *, TsymItem *, TsymItem *);
bool term(TToken **, FILE *, TsymItem *, TsymItem *);
bool assorfun(TToken **, FILE *, TsymItem *, TsymItem *);
bool assign(TToken **, FILE *, TsymItem *, TsymItem *);
bool decideExprOrFunc(TToken **, FILE *, TsymItem *, TsymItem *);
bool pbody(TToken **, FILE *, TsymItem *, TsymItem *);
bool eol(TToken **, FILE *, TsymItem *, TsymItem *);
bool then(TToken **, FILE *, TsymItem *, TsymItem *);
bool telse(TToken **, FILE *, TsymItem *, TsymItem *);
bool end(TToken **, FILE *, TsymItem *, TsymItem *);
bool tdo(TToken **, FILE *, TsymItem *, TsymItem *);
bool id(TToken **, FILE *, TsymItem *, TsymItem *);
bool lbr(TToken **, FILE *, TsymItem *, TsymItem *);
bool rbr(TToken **, FILE *, TsymItem *, TsymItem *);

#endif //IFJ18_PARSER_H
/* end of parser.h */