#include "dyn_arr.h"


/*
* string_init()
* 	Allocate memory for string and check if allocation is succesful.
*/
string stringInit(){				
	string arr = (string) calloc( ALLOC_SIZE,  sizeof(string) );
	
	if(arr == NULL){
		printf("dyn_Arr ERROR in stringInit: I can't initialise array.\n");
		return NULL;
	}

	return arr;
}

/*
* char_put()
* 	Check if given string is not NULL and if is not NULL, add char c into string.
*	If string is full and user wants to add new char, than allocate new memory for him.
*/
void charPut(string arr, char c){		
	if(arr != NULL){
		if( (strlen(arr) != 0) && (strlen(arr) % ALLOC_SIZE == 0) ){
			
			string newArr = (string) malloc(sizeof(char) * strlen(arr) + 1 + ALLOC_SIZE);

			if(newArr == NULL){
				printf("dyn_Arr ERROR in char_put: Can't allocate new place for string.\n");
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
		printf("dyn_ARR ERROR in char_put: Given string is NULL.\n");
		return;
	}
}

