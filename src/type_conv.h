#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dyn_arr.h"
#include "scanner.h"


#ifndef TYPE_CONV
#define TYPE_CONV

int convStrToInt(string str);			// convert string to integer
string convIntToStr(int numb);			// convert integer to string
double convStrToDouble(string str);		// convert string to double
string tokToStr(TToken tok);

#endif //TYPE_CONV
