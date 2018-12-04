/**
	file name:	    dyn_arr.c
	project:	    VUT-FIT-IFJ-2018
	created:	    19.10.2018
	last modified:	19.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz

	modifications:	
	
	description:	Api for string, that automatically change his lenght if needed
*/

#include "dyn_arr.h"
#include "ifj_error.h"

int errflg = 0;

string stringInit(){				
	string arr = (string) calloc( ALLOC_SIZE + 1,  sizeof(char) );

	if(arr == NULL){
		ifjErrorPrint("dyn_Arr ERROR in stringInit: I can't initialise array.\n");
		errflg = ERR_RUNTIME;
		return NULL;
	}

	return arr;
}

void charPut(string *arr, char c){		

	if((*arr) != NULL){
			
		if( (strlen(*arr) != 0) && ((strlen(*arr) % (ALLOC_SIZE)) == 0) ){
			(*arr) = (string) realloc((*arr), strlen(*arr) + ALLOC_SIZE+1);

			if((*arr) == NULL){
				ifjErrorPrint("dyn_Arr ERROR in char_put: Can't allocate new place for string.\n");
				errflg = ERR_RUNTIME;
				free(*arr);
				return;
			}
			
			for(unsigned int i = strlen(*arr); i < strlen(*arr)+ALLOC_SIZE+1; i++)
				(*arr)[i] = '\0';
				
			(*arr)[strlen(*arr)] = c;
		}
		
		else{
			(*arr)[strlen(*arr)] = c; 
		}
	}
	
	else{
		ifjErrorPrint("dyn_ARR ERROR in char_put: Given string is NULL.\n");
		errflg = ERR_RUNTIME;
		return;
	}
}


bool strDelete( string arr ){
	if( arr == NULL)
		return 1;
	
	else{
		free(arr);
		return 0;
	}

}
