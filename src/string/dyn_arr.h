#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef DYNARR
#define DYNARR

#define ALLOC_SIZE 4		// Bytes for sting allocation

typedef char* string; 

string string_init();
void char_put(string arr, char c);
unsigned int string_size();

#endif	//DYNARR
