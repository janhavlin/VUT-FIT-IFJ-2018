#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef DYN_ARR
#define DYN_ARR

#define ALLOC_SIZE 4		// Bytes for sting allocation

typedef char* string; 

string stringInit();
void charPut(string arr, char c);

#endif	//DYN_ARR
