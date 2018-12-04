/**
 *	file name:		stack_list.h
 *	project:		VUT-FIT-IFJ-2018
 *	created:		19.10.2018
 *	last modified:	19.10.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *	modifications:	
 *	
 *	description:	Stack reaslied by list
*/

#ifndef STACK_LIST_PR
#define STACK_LIST_PR

#include <stdio.h>
#include <stdbool.h>
#include "dyn_arr.h"
#include "ifj_error.h"
#include "dyn_arr.h"
#include "scanner.h"

// struct for item in stack
typedef struct tStackItem{	
	struct tStackItem *next;
	struct tStackItem *pred;
	char *IdName;
	int type;
	unsigned int numberOfE;
}tStackI;

// struct for stack itself
typedef struct tStackList{	 
	tStackI *top;
	tStackI *first;	
}tStackL;


typedef tStackI* tStackIPtr;	// ptr to item 
typedef tStackL* tStackLPtr;	// ptr to list

/**
 * @brief Inicialise all stack - alloc memory for it
 * 
 * @param bottomStakTokType Type of token (konstant, identifier,...) that is needed for stack bottom
 * @param bottomStakTokName Name of token that is needed for stack bottom
 * @return Pointer to allocated and inicialized stack
 */
tStackLPtr sLInit( int bottomStakTokType, string bottomStakTokName );

/**
 * @brief Return the most top item in stack
 * 
 * @param stack Pointer to stack
 * @return Item at the top of stack
 */
tStackIPtr sLTop( tStackLPtr stack );

/**
 * @brief Return the most top item in stack and pop it
 * 
 * @param stack Pointer to stack
 * @return Item at the top of stack
 */
tStackIPtr sLPop( tStackLPtr stack );

/**
 * @brief Take item all items till shift sign and create String form it, that is important for precedence S.A.
 * 
 * @param stack Pointer to stack
 * @return Expression for precedence syntactic analyse
 */
string sGetExprToReduce( tStackLPtr stack );

/**
 * @brief Delete allstack
 * 
 * @param stack Pointer to stack
 * @return True if the stack was succesfully deleted, otherwise return False
 */
bool sLDelete( tStackLPtr stack );

/**
 * @brief Check if the stack is empty or not
 * 
 * @param stack Pointer to stack
 * @return True if the stack is empty, otherwise return False
 */
bool sLEmpty( tStackLPtr stack );

/**
 * @brief Check if the stack is full or not
 * 
 * @param stack Pointer to stack
 * @return True if the stack is full, otherwise return False
 */
bool sLFull( tStackLPtr stack );

/**
 * @brief Push item into stack
 * 
 * @param stack Pointer to stack
 * @param name Name of token
 * @param type Type of token 
 */
void sLPush( tStackLPtr stack, char *name, int type);

/**
 * @brief Place shift char, used for precedence S.A., into stack
 * 
 * @param stack Pointer to stack
 */
void sPlaceShiftChar( tStackLPtr s );

/**
 * @brief Helping function to print all stack from first item to the top item
 * 
 * @param stack Pointer to stack
 */
void sPrintStack( tStackLPtr s);
#endif // STACK_PR
