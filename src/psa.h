/**
 *	file name:		psa.h
 *	project:		VUT-FIT-IFJ-2018
 *	created:		23.11.2018
 *	last modified:	23.11.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *					
 *	modifications:	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
 *	
 *	description:	Precedence syntax analysis for expressions
 */

#ifndef IFJ18_PSA_H
#define IFJ18_PSA_H
#include <stdio.h>
#include "dyn_arr.h"
#include "scanner.h"
#include "stack_list.h"
#include "s_table.h"
#include "type_conv.h"
#include "code_gen.h"

#define MAX_RULE_LENGTH	17
#define AMOUNT_OF_RULES	18
#define RULE_NOT_FOUND	-1
#define NO_E_NONTERM	0

#define ADD_RULE			0
#define SUB_RULE			1
#define PLUSE_RULE			2
#define NEG_RULE			3
#define MUL_RULE			4
#define DIV_RULE			5
#define LT_RULE				6
#define GT_RULE				7
#define LEQ_RULE			8
#define GEQ_RULE			9
#define EQ_RULE				10
#define NEQ_RULE			11
#define LBR_E_RBR_RULE		12
#define ID_RULE				13
#define INT_RULE			14
#define FLOAT_RULE			15
#define STRING_RULE			16

char rules[AMOUNT_OF_RULES][MAX_RULE_LENGTH];	// all rules used for precedence S.A.

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
unsigned int processExpression(FILE *f, string followingToken, TsymItem *STG, TsymItem *STL, TInstrList *instrList, bool inWhile, bool inFunDef);

/**
 * @brief Find corresponding rule to apply (shift, reduce, empty, equal) for precedence S.A.
 * 
 * @param stackTopTok Token on the top of stack
 * @param newTok Token that comes from scanner 
 * @return Specific rule - Shift == 's', Reduce == 'r', Empty == 'X', Equal == 'e'
 */
char lookInPrecedenceTable(TToken stackTopTok, TToken newTok);

/**
 * @brief Help function, that get index into precedence table
 * 
 * @param token Token which index we want to get
 * @return Index to precedence table
 */
int getIndex(TToken token);

/**
 * @brief Goes from Top to First item of stack until terminal is found
 * 
 * @param stack Pointer to stack
 * @return Highest terminal in stack
 */
TToken highestTerminal( tStackLPtr stack );

/**
 * @brief Goes through all rules untiol the rule is found
 * 
 * @param readRule String with tokens to reduce (rule)
 * @return Number of rule or -1 when rule is not found
 */
int findRule( string readRule );

/**
 * @brief Help function, to store data from scannere structure TToken to structure for generator 
 * 
 * @param get Token that we need to store into generator structure
 * @return Generator structure
 */
TAdr idValGet(TToken get);

#endif //IFJ18_PSA_H
