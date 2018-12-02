/**
	file name:	    dyn_arr.h
	project:	    VUT-FIT-IFJ-2018
	created:	    19.10.2018
	last modified:	19.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
	modifications:	
	
	description:	File with error numbers and error printing function
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef DYN_ARR
#define DYN_ARR

#define ALLOC_SIZE 4		// Bytes for sting allocation

typedef char* string; 

string stringInit();
void charPut(string *arr, char c);
bool strDelete( string arr );

#endif	//DYN_ARR
