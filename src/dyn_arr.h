/**
	file name:	    dyn_arr.h
	project:	    VUT-FIT-IFJ-2018
	created:	    19.10.2018
	last modified:	19.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
	
	modifications:	
	
	description:	Api for string, that automatically change his lenght if needed
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef DYN_ARR
#define DYN_ARR

#define ALLOC_SIZE 4		// Bytes for sting allocation

typedef char* string; 

/**
 * @brief Inicialising string
 * 
 * @return Inicialised string
 */
string stringInit();

/**
 * @brief Put char into string
 * 		Check if given string is not NULL and add char c into string.
 *		If string is full and user wants to add new char, than allocate new memory for him.
 * 
 * @param arr Pointer to string, where we want to put data
 * @param c Char data, we want put into string
 * @return Specific rule - Shift == 's', Reduce == 'r', Empty == 'X', Equal == 'e'
 */
void charPut(string *arr, char c);

/**
 * @brief Delete all string
 * 
 * @param arr String that we want to delete 
 * @return True the string is succesfully deleted rule, otherwise returns False
 */
bool strDelete( string arr );

#endif	//DYN_ARR
