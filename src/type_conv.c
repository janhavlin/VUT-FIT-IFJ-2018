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
		if((pos == 0) && (str[pos] == '-')){
			result *= -1;
			break;
		}
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
	bool sign = false;
	
	if(numb < 0){
		sign = true;
		numb *= -1;			
	}
	
	while(numb > 0){
		charPut(res, numb % 10 + '0');
		numb /= 10;
	}

	if(sign)
		charPut(res, '-');
	for(unsigned int i = 0; i < strlen(res)/2; i++){
		tmp = res[i];
		res[i] = res[strlen(res)-i-1];
		res[strlen(res)-i-1] = tmp;
	}

		
	return res;	
}


/*
* ConvStrToFloat()
* 	Converts string to float/double.
*/
double ConvStrToFloat(char *str){
	int dot = strlen(str)-1;
	int expon = 0;	
	bool dotFound = false;
	bool something = true;
	
	for(int pos = strlen(str)-1 ;pos >= 0; pos--){
		if(str[pos] == '.')
			dot = pos;
	}
	
	double res = 0.0;
	
	for(int pos = strlen(str)-1; pos >= 0; pos--){
		if( (pos == 0) && str[pos] == '-')
			res *= -1;
		else if( something && (pos > 0) && ((str[pos] == '+') || (str[pos] == '-') || (str[pos] == 'e') || (str[pos] == 'E')) ){
			expon = res*pow(10,strlen(str)-dot-1);
			something = false;
			
			if(str[pos] == '-')
				expon*=-1;
				
			res = 0.0;
		}		
	
		else if( (pos != dot) ){
			if((str[pos] != 'E') && (str[pos] != 'e'))
				res += (str[pos] - '0') * pow(0.1, pos-dot+dotFound);
		}
				
		
		else
			dotFound = true;
	}

	if(expon != 0)
		res *= pow(10, expon);

	return res;
}




