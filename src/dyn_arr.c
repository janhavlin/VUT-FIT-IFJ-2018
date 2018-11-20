#include "dyn_arr.h"
#include "ifj_error.h"

/*
* string_init()
* 	Allocate memory for string and check if allocation is succesful.
*/
string stringInit(unsigned int *err){				
	string arr = (string) calloc( ALLOC_SIZE + 1,  sizeof(char) );

	if(arr == NULL){
		ifjErrorPrint("dyn_Arr ERROR in stringInit: I can't initialise array.\n");
		*err = TRANSLATOR_ERR;
		return NULL;
	}

	return arr;
}



/*
 * char_put()
 * 	Check if given string is not NULL and if is not NULL, add char c into string.
 *	If string is full and user wants to add new char, than allocate new memory for him.
 */
void charPut(string *arr, char c, unsigned int *err){		

	if((*arr) != NULL){
			
		if( (strlen(*arr) != 0) && ((strlen(*arr) % (ALLOC_SIZE)) == 0) ){
			(*arr) = (string) realloc((*arr), strlen(*arr) + ALLOC_SIZE+1);

			if((*arr) == NULL){
				ifjErrorPrint("dyn_Arr ERROR in char_put: Can't allocate new place for string.\n");
				*err = TRANSLATOR_ERR;
				free(*arr);
				return;
			}
			
			for(int i = strlen(*arr); i < strlen(*arr)+ALLOC_SIZE+1; i++)
				(*arr)[i] = '\0';
				
			(*arr)[strlen(*arr)] = c;
		}
		
		else{
			(*arr)[strlen(*arr)] = c; 
		}
	}
	
	else{
		ifjErrorPrint("dyn_ARR ERROR in char_put: Given string is NULL.\n");
		*err = SEM_DEFINE_ERR;
		return;
	}
}



/*
 * strDelete()
 *	Free pasted string. If addr of string is NULL, nothing happens and value 1 is returned.
 *  If addr is not NULL, then free string and return 0.
 */
bool strDelete( string arr ){
	if( arr == NULL)
		return 1;
	
	else{
		free(arr);
		return 0;
	}

}

