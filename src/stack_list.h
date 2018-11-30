/*
	stack.h
		API for stack.	
*/

#ifndef STACK_LIST_PR
#define STACK_LIST_PR

#include <stdio.h>
#include <stdbool.h>
#include "dyn_arr.h"
#include "ifj_error.h"
#include "dyn_arr.h"


typedef struct tStackItem{	// struct for item in stack
	struct tStackItem *next;
	struct tStackItem *pred;
	char *IdName;
	int type;
	unsigned int numberOfE;
}tStackI;


typedef struct tStackList{	// struct for whole stack 
	tStackI *top;
	tStackI *first;	
}tStackL;


typedef tStackI* tStackIPtr;	// item ptr
typedef tStackL* tStackLPtr;	// list ptr


tStackLPtr sLInit( int bottomStakTokType, string bottomStakTokName ); //string bottomStakTokType
tStackIPtr sLTop( tStackLPtr stack );
tStackIPtr sLPop( tStackLPtr stack );
string sGetExprToReduce( tStackLPtr stack );
bool sLDelete( tStackLPtr stack );
bool sLEmpty( tStackLPtr stack );
bool sLFull( tStackLPtr stack );
void sLPush( tStackLPtr stack, char *name, int type); //char *type
void sPlaceShiftChar( tStackLPtr s );

#endif // STACK_PR
