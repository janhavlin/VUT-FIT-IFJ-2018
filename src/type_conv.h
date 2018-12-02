/**
	file name:		type_conv.h
	project:		VUT-FIT-IFJ-2018
	created:		20.10.2018
	last modified:	20.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
	modifications:	
	
	description:	Converting types to different types
*/

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
