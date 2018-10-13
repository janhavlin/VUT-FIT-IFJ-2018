#include <stdbool.h>
#include <math.h>
#include "type_conv.h"
#include "dyn_arr.h"


/*
* ConvStrToInt()
* 	Converts string to integer.
*/
int ConvStrToInt(char *str){		
	int result = 0;
	int dec = 1;

	for(int pos = strlen(str)-1; pos >= 0; pos--){
		result += (str[pos] - '0')* dec;
		dec *= 10;
	}

	return result;
}


/*
* ConvIntToStr()
* 	Converts integer to string.
*/
char *ConvIntToStr(int numb){		
	char *res = stringInit();
	char tmp;
	
	while(numb > 0){
		charPut(res, numb % 10 + '0');
		numb /= 10;
	}

	for(unsigned int i = 0; i < strlen(res)/2; i++){
		tmp = res[i];
		res[i] = res[strlen(res)-i-1];
		res[strlen(res)-i-1] = tmp;
	}

	return ahoj;	
}


/*
* ConvStrToFloat()
* 	Converts string to float/double.
*/
double ConvStrToFloat(char *str){
	int dot = strlen(str)-1;
	bool dotFound = false;
	
	for(int pos = strlen(str)-1 ;pos >= 0; pos--){
		if(str[pos] == '.'){
			dot = pos;
			break;
		}
	}
	
	double res = 0.0;
	
	for(int pos = strlen(str)-1; pos >= 0; pos--){
		if(pos != dot)
			res += (str[pos] - '0') * pow(0.1, pos-dot+dotFound);
		else
			dotFound = true;	
	}
	
	return res;
}




