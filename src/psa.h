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
#include<stdio.h>
#include "dyn_arr.h"
#include "scanner.h"



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

char **rules = {"E+E",
				"E-E",
				"+E",
				"-E",
				"E*E",
				"E/E",
				"E<E",
				"E>E",
				"E<=E",
				"E>=E",
				"E==E",
				"E!=E",
				"(E)",
				"i",
				"+E"};

#endif //IFJ18_PSA_H
/* end of psa.h */