/**
	file name:		parser.h
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	01.12.2018
	
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
#include "code_gen.h"
#include "code_gen_list.h"

int parse(FILE *, TsymItem *GT, TsymItem *LT, TInstrList *instructions);

#endif //IFJ18_PARSER_H
/* end of parser.h */