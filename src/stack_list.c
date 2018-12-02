/**
	file name:		stack_list.c
	project:		VUT-FIT-IFJ-2018
	created:		27.10.2018
	last modified:	27.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
	modifications:	
	
	description:	Stack based on list, used in precedence syn. analyse 
*/

#include "stack_list.h"

/*
 *	sLInit()
 *		Inicialise all stack + add first item with $.
 *		Return pointer to inicialise stack.
 */
tStackLPtr sLInit( int bottomStakTokType, string bottomStakTokName ){	//string bottomStakTokType
	tStackLPtr stack = (tStackLPtr) malloc(sizeof(tStackL));
	
	if(stack != NULL){
		stack->first = (tStackIPtr) malloc(sizeof(tStackI));
		
		if(stack->first != NULL){
			stack->first->pred 	 = NULL;	
			stack->first->next 	 = NULL;
			//stack->first->type 	 = (char *) calloc(strlen(bottomStakTokType)+1, sizeof(char));
			stack->first->type = 0;
			stack->first->IdName = (char *) calloc(strlen(bottomStakTokName)+1, sizeof(char));

			if(stack->first->IdName == NULL){
				free(stack->first);
				free(stack);
				ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate 'IdName' int stack. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return NULL; 
			}
			
			/*if(stack->first->type == NULL){
				free(stack->first);
				free(stack);
				ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate string 'type' in stack. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return NULL;
			}*/
			
			stack->first->IdName = strcpy(stack->first->IdName, bottomStakTokName);	
			stack->first->type   = bottomStakTokType;	//strcpy(stack->first->type, bottomStakTokType);		//EOS End Of Stack
			stack->top 		 	 = stack->first;
			
			stack->top->next = NULL;
			stack->top->pred = NULL;	
			return stack;	
		}
		else{
			free(stack);
			ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate item 'first'. ERROR %d\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return NULL;
		}
	}
	else{
		ifjErrorPrint("stack_list ERROR in sLInit: Can't allocate whole struct. ERROR %d\n", ERR_RUNTIME);
		errflg = ERR_RUNTIME;
		return NULL;
	}
}


/*
 *	sLDelete()
 *		Delete whole stack.
*/
bool sLDelete( tStackLPtr stack ){
	if(stack != NULL){
		tStackIPtr help = stack->first;
		tStackIPtr delete = NULL;
		
		if(help == NULL)
			return false;
			
		while(help->next != NULL)	// the most right item of list
			help = help->next;
		
		while(help != stack->first){	
			delete = help;
			help = help->pred;
			
			if(delete->IdName != NULL)
				free(delete->IdName);
				
			free(delete);	
		}

		if(stack->first->IdName != NULL)
			free(stack->first->IdName);

		free(stack->first);

		stack->first = NULL;
		stack->top = NULL;
		
		free(stack);	
		return true;
	}
	else
		return false;
}


/*
 *	sLEmpty()
 *		Check if stack is empty. If yes, return 1, otherwise return 0.
*/
bool sLEmpty( tStackLPtr stack ){	// return 1 if stack is empty
	if(stack != NULL){
		/*if( !(strcmp(stack->top->IdName, "$")) || !(strcmp(stack->top->type, "EOS")) ){
			return 1;
		}*/
		
		if( (stack->top == NULL) || (stack->first == NULL) ){
			return 1;		
		}	
		
		else
			return 0;
	
	}
	return 1;
}


/*
 *	sLTop()
 *		Return item at the top of stack. It will not delete it.
*/
tStackIPtr sLTop( tStackLPtr stack ){
	if(stack != NULL)
		return stack->top;
	else
		return NULL;
}


/*
 *	sLPop()
 *		Return item at the top of stack. Decrement top value.
 */
tStackIPtr sLPop( tStackLPtr stack ){
	if( !sLEmpty(stack) ){
		tStackIPtr res = stack->top;
		stack->top = stack->top->pred;
		return res;
	}
	
	return NULL;
}


/*
 *	sGetExprToReduce()
 *		When reduce is asserted, this function find string from top of stack to symbol 's' == '<'
 *		and return this string (without 's').
 */
string sGetExprToReduce( tStackLPtr stack ){	// for example sE+E
	if(stack != NULL){
		unsigned long amount = 0;
		tStackIPtr item = stack->top;

		while( strcmp(item->IdName, "s") != 0 ){
			amount += strlen(item->IdName);
			item = item->pred;
			if( (item == NULL) || (item->IdName == NULL) || (item == stack->first) )
				return NULL;
		}

		if((item = item->next) == NULL){	// move one item next to shift char 's'
			return NULL;
		}
		
		string exp = (string) calloc(amount+1, sizeof(char));
		
		if(exp == NULL){
			ifjErrorPrint("stack_list ERROR in sGetExprToReduce: Can't allocate item 'exp' in stack. ERROR %d\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return NULL;		
		}
		
		while ( item != stack->top->next ){
			strcat(exp, item->IdName);	//, strlen(item->IdName)
			item = item->next;
		}

		return exp;
	}
	else
		return NULL;
}


				
/*
 *	sPreAdd()
 *		Paste item before given item.
*/
void sPreAdd( tStackLPtr stack, tStackIPtr before, char *name, int type ){ //char *type
	if(stack != NULL){
		if( (before != NULL) && (name != NULL) ){ // (type != NULL) &&
			tStackIPtr New 	= (tStackIPtr) malloc(sizeof(tStackI));
		
			if(New == NULL){
					ifjErrorPrint("stack_list ERROR in sPreAdd: Can't allocate item 'New' in stack. ERROR %d\n", ERR_RUNTIME);
					errflg = ERR_RUNTIME;
					return;
			}
			
			New->next = NULL;
			New->pred = NULL;
		
			New->IdName = (char *) calloc(strlen(name) + 1, sizeof(char));
			/*New->type 	= (char *) calloc(strlen(type) + 1, sizeof(char));*/		
		
			if(New->IdName == NULL){
				ifjErrorPrint("stack_list ERROR in sPreAdd: Can't allocate item 'IdName' in New. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return;
			}

			/*if( New->type == NULL){
				ifjErrorPrint("stack_list ERROR in sPreAdd: Can't allocate item 'type' in New. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return;
			}*/
		
		
			New->IdName = strcpy(New->IdName, name);
			New->type 	= type;//strcpy(New->type, type);
			
			tStackIPtr help = before->pred;
			
			help->next = New;
			New->pred = help;
			
			New->next = before;
			before->pred = New;
		}
	}
}
				
/*
 *	sLPush()
 *		Create new stack list, fill it with 'type' and 'name' and then put it into stack.
*/
void sLPush( tStackLPtr stack, char *name, int type  ){	//char *type
	if(stack != NULL){
		tStackIPtr New 	= (tStackIPtr) malloc(sizeof(tStackI));
		
		if(New == NULL){
				ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'New' in stack. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return;
		}
		New->next = NULL;
		New->pred = NULL;
		
		New->IdName = (char *) calloc(strlen(name) + 1, sizeof(char));
		//New->type 	= (char *) calloc(strlen(type) + 1, sizeof(char));		
		
		if(New->IdName == NULL){
			ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'IdName' in New. ERROR %d\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return;
		}

		/*if( New->type == NULL){
			ifjErrorPrint("stack_list ERROR in sLPush: Can't allocate item 'type' in New. ERROR %d\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return;
		}*/
		

		New->IdName = memcpy(New->IdName, name, strlen(name));
		New->type 	= type;//strcpy(New->type, type);
		
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

void sPlaceShiftChar( tStackLPtr s ){
	tStackIPtr tmp = s->top;
	tStackIPtr help;

	tStackIPtr New = (tStackIPtr) malloc(sizeof(tStackI));
	if(New == NULL){
		ifjErrorPrint("stack_list ERROR in sPlaceShiftChar: Can't allocate item 'New' in stack. ERROR %d\n", ERR_RUNTIME);
		errflg = ERR_RUNTIME;
		return;
	}

	New->IdName = (string) calloc(3, sizeof(char));
	if(New->IdName == NULL){
		free(New);
		ifjErrorPrint("stack_list ERROR in sPlaceShiftChar: Can't allocate item 'New' in stack. ERROR %d\n", ERR_RUNTIME);
		errflg = ERR_RUNTIME;
		return;
	}
	
	memcpy(New->IdName, "s", 1);
	New->next = NULL;
	New->pred = NULL;
	New->type = 15;
	New->numberOfE = 0;

	while( !strcmp( tmp->IdName, "E") ){
            tmp = tmp->pred;
	}
	 

	if(tmp != s->top){
		help = tmp->next;
		
		tmp->next = New;
		New->pred = tmp;
		
		New->next = help;
		help->pred = New;
	}

	else{
		help = tmp->next;

		tmp->next = New;
		New->pred = tmp;
		s->top = New;

		New->next = help;
		if(help != NULL)
			help->pred = New;
	}

}

void sPrintStack( tStackLPtr s){
	if(s != NULL){	
		tStackIPtr item = s->first;
		while(item != s->top){
			printf("[Name: %s, Type: %d]-", item->IdName, item->type);
			if(item->next == NULL){
				printf("-/\n\n");
			}
			else
				printf("->");
			item = item->next;
		}
		printf("[Name: %s, Type: %d]-/\n", item->IdName, item->type);
	}
}




