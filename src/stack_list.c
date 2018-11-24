#include "stack_list.h"
#include "scanner.h"

tStackLPtr sLInit( void ){
	tStackLPtr stack = (tStackLPtr) malloc(sizeof(tStackL));
	
	if(stack != NULL){
		
		stack->first = (tStackIPtr) malloc(sizeof(tStackI));
		
		if(stack->first != NULL){
			stack->first->pred 		= NULL;	
			stack->first->next 		= NULL;
			stack->first->type 		= (char *) calloc(4, sizeof(char));
			stack->first->IdName 	= (char *) calloc(2, sizeof(char));

			if(stack->first->IdName == NULL){
				free(stack->first);
				free(stack);
				ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate 'IdName' int stack. ERROR %d\n", ERR_RUNTIME);
				return NULL; 
			}
			
			if(stack->first->type == NULL){
				free(stack->first);
				free(stack);
				ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate string 'type' in stack. ERROR %d\n", ERR_RUNTIME);
				return NULL;
			}
			
			stack->first->IdName = strcpy(stack->first->IdName, "$\0");	
			stack->first->type   = strcpy(stack->first->type, "EOS\0");		//EOS End Of Stack
			stack->top 		 	 = stack->first;

			stack->top->next = NULL;
			stack->top->pred = NULL;	
			return stack;	
		}
		else{
			free(stack);
			ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate item 'first'. ERROR %d\n", ERR_RUNTIME);
			return NULL;
		}
	}
	else{
		ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate whole struct. ERROR %d\n", ERR_RUNTIME);
		return NULL;
	}
}

tStackIPtr sHighestTerminal( tStackLPtr stack ){
	//TODO:
}



bool sLDelete( tStackLPtr stack ){
	if(stack != NULL){
		tStackIPtr help = stack->first;
		tStackIPtr delete = NULL;
		
		while(help->next != NULL)	// the most right item of list
			help = help->next;
		
		while(help != stack->first){	
			delete = help;
			help = help->pred;
			
			if(delete->IdName != NULL)
				free(delete->IdName);

			if(stack->top->type != NULL)
				free(delete->type);
			
			free(delete);	
		}

		if(stack->first->IdName != NULL)
			free(stack->first->IdName);
		
		if(stack->first->type != NULL)
			free(stack->first->type);

		free(stack->first);

		stack->first = NULL;
		stack->top = NULL;
		
		free(stack);	
		return 1;
	}
	else
		return 0;
}


bool sLEmpty( tStackLPtr stack ){	// return 1 if stack is empty
	if(stack != NULL){
		if( !(strcmp(stack->top->type, "EOS")) || !(strcmp(stack->top->IdName, "$")) ){
			return 1;
		}
		
		if( (stack->top == NULL) || (stack->first == NULL) ){
			return 1;		
		}	
		
		else
			return 0;
	
	}
	return 1;
}


tStackIPtr sLTop( tStackLPtr stack ){
	if(stack != NULL)
		return stack->top;
	else
		return NULL;
}


tStackIPtr sLPop( tStackLPtr stack ){
	if( !sLEmpty(stack) ){
		tStackIPtr res = stack->top;
		stack->top = stack->top->pred;
		return res;
	}
	
	return NULL;
}

			
				

void sLPush( tStackLPtr stack, char *type, char *name ){
	if(stack != NULL){
		tStackIPtr New 	= (tStackIPtr) malloc(sizeof(tStackI));
		
		if(New == NULL){
				ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'New' in stack. ERROR %d\n", ERR_RUNTIME);
				return;
		}
		New->next = NULL;
		New->pred = NULL;
		
		New->IdName = (char *) calloc(strlen(name) + 1, sizeof(char));
		New->type 	= (char *) calloc(strlen(type) + 1, sizeof(char));		
		
		if(New->IdName == NULL){
			ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'IdName' in New. ERROR %d\n", ERR_RUNTIME);
			return;
		}

		if( New->type == NULL){
			ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'type' in New. ERROR %d\n", ERR_RUNTIME);
			return;
		}
		

		New->IdName = strcpy(New->IdName, name);
		New->type 	= strcpy(New->type, type);
		
		tStackIPtr Help;

		if(stack->top->next != NULL){
			Help = stack->top->next;
			stack->top->next = New;
			
			New->next = Help;
			Help->pred = New;

			New->pred = stack->top;
			stack->top = New;
		}

		else{	// top is really last item in list
			stack->top->next = New;
			New->pred = stack->top;

			stack->top = New;
		}
	}
}
