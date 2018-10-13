#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef TYPE_CONV
#define TYPE_CONV

int ConvStrToInt(char *str);		// convert string to integer
char *ConvIntToStr(int numb);		// convert integer to string
double ConvStrToFloat(char *str);	// convert string to float

#endif //TYPE_CONV
