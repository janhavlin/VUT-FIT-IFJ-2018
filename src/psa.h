/**
	file name:		psa.h
	project:		VUT-FIT-IFJ-2018
	created:		23.11.2018
	last modified:	23.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Precedence syntax analysis for expressions
*/
#ifndef IFJ18_PSA_H
#define IFJ18_PSA_H
#include <stdio.h>
#include "dyn_arr.h"
#include "scanner.h"
#include "stack_list.h"

#define MAX_RULE_LENGTH	5
#define AMOUNT_OF_RULES	15
#define RULE_NOT_FOUND	-1

/**
 * handles expressions
 * 
 * after reading "then", "do" or "eol" function MUST return it to the scanner with return!
 * 
 * @param string represents token expected after expression ("then", "do" or "eol")
 * 
 * @returns true if successfull, false othervise 
 * (parser is currently based on boolean evaluating, but I don't know if we won't need to return 
 * value for generating condition in the future)
 */
bool processExpression(FILE *, string);

char * lookInPrecedenceTable(TToken stackTopTok, TToken newTok);
int getIndex(TToken token);

char rules[AMOUNT_OF_RULES][MAX_RULE_LENGTH] = {
												{"E+E"},// rule 0 
												{"E-E"},// rule 1
												{"+E"},	// rule 2
												{"-E"},	// rule 3
												{"E*E"},// rule 4
												{"E/E"},// rule 5
												{"E<E"},// rule 6
												{"E>E"},// rule 7
												{"E<=E"},// rule 8
												{"E>=E"},// rule 9
												{"E==E"},// rule 10
												{"E!=E"},// rule 11
												{"(E)"},// rule 12
												{"i"},	// rule 13
												{"+E"}	// rule 14
											};	

tStackIPtr HighestTerminal( tStackLPtr stack );
int FindRule( string readRule );


#endif //IFJ18_PSA_H
