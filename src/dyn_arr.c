#include "dyn_arr.h"
#include "ifj_error.h"

/*
* string_init()
* 	Allocate memory for string and check if allocation is succesful.
*/
string stringInit(unsigned int *err){				
	string arr = (string) calloc( ALLOC_SIZE,  sizeof(string) );
	
	if(arr == NULL){
		fprintf(stderr,"dyn_Arr ERROR in stringInit: I can't initialise array.\n");
		*err = ERR_RUNTIME;
		return NULL;
	}

	return arr;
}

/*
* char_put()
* 	Check if given string is not NULL and if is not NULL, add char c into string.
*	If string is full and user wants to add new char, than allocate new memory for him.
*/
void charPut(string arr, char c, unsigned int *err){		
	if(arr != NULL){
		if( (strlen(arr) != 0) && (strlen(arr) % ALLOC_SIZE == 0) ){
			
			string newArr = (string) malloc(sizeof(char) * strlen(arr) + 1 + ALLOC_SIZE);

			if(newArr == NULL){
				error("dyn_Arr ERROR in char_put: Can't allocate new place for string.\n");
				*err = ERR_RUNTIME;
				return;
			}

			memcpy(newArr, arr, strlen(arr)-1);
			arr[strlen(arr)] = c;

			free(newArr);
		}
		
		else
			arr[strlen(arr)] = c; 
	}
	else{
		error("dyn_ARR ERROR in char_put: Given string is NULL.\n");
		*err = ERR_SEM_DEFINE;
		return;
	}
}

