/**
 * 	file name:		stack.h
 *	project:		VUT-FIT-IFJ-2018
 *	created:		22.10.2018
 *	last modified:	22.10.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *	modifications:	
 *	
 *	description:	Stack for string
*/

#ifndef STACK_PR
#define STACK_PR

#include <stdio.h>
#include <stdbool.h>
#include "dyn_arr.h"
#include "ifj_error.h"

#define STACK_SIZE 4

typedef struct tStack{
	unsigned int top;
	char *buff;
}tStack;

typedef tStack* tStackPtr;


tStackPtr sInit( void );
bool sDelete( tStackPtr stack );
bool sEmpty( tStackPtr stack );
bool sFull( tStackPtr stack );
char sTop( tStackPtr stack );
char sPop( tStackPtr stack );
void sPush( tStackPtr *stack, char item );

#endif // STACK_PR
