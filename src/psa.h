/**
	file name:		psa.h
	project:		VUT-FIT-IFJ-2018
	created:		23.11.2018
	last modified:	23.11.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
					
	modifications:	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	
	description:	Precedence syntax analysis for expressions
*/
#ifndef IFJ18_PSA_H
#define IFJ18_PSA_H
#include <stdio.h>
#include "dyn_arr.h"
#include "scanner.h"
#include "stack_list.h"
#include "s_table.h"

#define MAX_RULE_LENGTH	5
#define AMOUNT_OF_RULES	15
#define RULE_NOT_FOUND	-1
#define NO_E_NONTERM	0

#define ADD_RULE		0
#define SUB_RULE		1
#define PLUSE_RULE		2
#define MINUSE_RULE		3
#define MUL_RULE		4
#define DIV_RULE		5
#define LT_RULE			6
#define GT_RULE			7
#define LEQ_RULE		8
#define GEQ_RULE		9
#define EQ_RULE			10
#define NEQ_RULE		11
#define LBR_E_RBR_TOK	12
#define ID_RULE			13


/**
 * handles expressions
 * 
 * after reading "then", "do" or "eol" function MUST return it to the scanner with return!
 * 
 * @param string represents token expected after expression ("then", "do" or "eol")
 * 
 * @returns true if amount of nonterminal E, when it is 0, it is ERROR 
 * (parser is currently based on boolean evaluating, but I don't know if we won't need to return 
 * value for generating condition in the future)
 */
unsigned int processExpression(FILE *, string, TsymItem *STG, TsymItem *STL);

char lookInPrecedenceTable(TToken stackTopTok, TToken newTok);
int getIndex(TToken token);

char *rules[AMOUNT_OF_RULES] = {
	{"ETOK_ADDE\0"},		// rule 0 
	{"ETOK_SUBE\0"},		// rule 1
	{"TOK_ADDE\0"},		// rule 2
	{"TOK_SUBE\0"},		// rule 3
	{"ETOK_MULE\0"},		// rule 4
	{"ETOK_DIVE\0"},		// rule 5
	{"ETOK_LTE\0"},		// rule 6
	{"ETOK_GTE\0"},		// rule 7
	{"ETOK_LEQE\0"},		// rule 8
	{"ETOK_GEQE\0"},		// rule 9
	{"ETOK_EQE\0"},		// rule 10
	{"ETOK_NEQE\0"},		// rule 11
	{"TOK_LBRETOK_RBR\0"},// rule 12
	{"TOK_ID\0"}			// rule 13
};

TToken highestTerminal( tStackLPtr stack );
int findRule( string readRule );


#endif //IFJ18_PSA_H
