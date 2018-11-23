#include "stack.h"

tStackPtr sInit( void ){
	tStackPtr stack;
	stack = (tStackPtr) malloc(sizeof(tStack));
	
	if(stack != NULL){	
		stack->buff = (char *) calloc(STACK_SIZE, sizeof(char*));
		if(stack->buff == NULL){
			ifjErrorPrint("stack ERROR in sInit: Given string is NULL. ERROR %d\n", ERR_RUNTIME);
			return NULL; 
		}
		stack->top = 0;	
	}
	
	else{
		ifjErrorPrint("stack ERROR in sInit: Given string is NULL. ERROR %d\n", ERR_RUNTIME);
		return NULL;
	}

	return stack;
}

bool sDelete( tStackPtr stack ){
	bool res = 0;
	free(stack->buff);
	
	if(stack->buff == NULL)
		res = 1;
		
	free(stack);
	
	if(stack == NULL)
		res &= 1;
	
	return res;
	
}

bool sEmpty( tStackPtr stack ){
	return ((stack->top == 0) || (stack->buff[0] == '$'));
}

char sTop( tStackPtr stack ){
	if((stack->top) > 0)
		return stack->buff[stack->top-1];

	return 0;
}

char sPop( tStackPtr stack ){
	if(stack->top > 0){
		stack->top--;
		return stack->buff[stack->top-1];
	}
	
	return 0;
}

			
				

void sPush( tStackPtr *stack, char item ){
	if( !((*stack)->top % STACK_SIZE) && ((*stack)->top != 0) ){
		(*stack)->buff = (char *) realloc((*stack)->buff, strlen((*stack)->buff) + STACK_SIZE + 1);
		
		if((*stack)->buff == NULL){
			ifjErrorPrint("stack ERROR in sPush: Given string is NULL.\n");
			return;
		}
		
		for(unsigned int i = strlen((*stack)->buff); i < strlen((*stack)->buff) +STACK_SIZE+1; i++)
			(*stack)->buff[i] = '\0';
			
		
		
		(*stack)->buff[(*stack)->top++] = item;
	}
	
	else{
		(*stack)->buff[(*stack)->top++] = item;
	}
	
}

