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
