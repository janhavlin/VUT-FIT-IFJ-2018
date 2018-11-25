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
#include "psa.h"
#include "s_table.h"

bool parserStart(TsymItem *global, TsymItem *local, FILE *;
bool start(TToken **, FILE *);
bool stat(TToken **, FILE *);
bool fundef(TToken **, FILE *);
bool stlist(TToken **, FILE *);
bool plist(TToken **, FILE *);
bool term(TToken **, FILE *);
bool assorfun(TToken **, FILE *);
bool assign(TToken **, FILE *);
bool pbody(TToken **, FILE *);
bool eol(TToken **, FILE *);
bool then(TToken **, FILE *);
bool telse(TToken **, FILE *);
bool end(TToken **, FILE *);
bool tdo(TToken **, FILE *);
bool id(TToken **, FILE *);
bool lbr(TToken **, FILE *);
bool rbr(TToken **, FILE *);

#endif //IFJ18_PARSER_H
/* end of parser.h */