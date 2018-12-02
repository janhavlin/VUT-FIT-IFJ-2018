/**
	file name:		type_conv.c
	project:		VUT-FIT-IFJ-2018
	created:		20.10.2018
	last modified:	20.10.2018
	
	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
	modifications:	
	
	description:	Converting types to different types
*/

#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "type_conv.h"
#include "ifj_error.h"



/*
* convStrToInt()
* 	Converts string to integer.
*/
int convStrToInt(string str){			
	int result = 0;
	int dec = 1;
	int shifter = 0;
	int type = 0;	// 0- decimal
					// 1- hexadecimal
					// 2- octal
	
	// decimal
	if(str[0] != '0' )
		type = 0;

	// hexadecimal
	else if((str[0] == '0') && ((str[1] == 'x')) )
		type = 1;

	// binary
	else if( (isdigit(str[0])) && (str[1] == 'b') )
		type = 2;
	

	// octal
	else if( (isdigit(str[0]) != 0) && (str[0] == '0'))	
		type = 3;



	for(int pos = strlen(str)-1; pos >= 0; pos--){
		if((pos == 0) && (str[pos] == '-')){
			result *= -1;
			break;
		}

		if(!type){
			result += (str[pos] - '0')* dec;
			dec *= 10;
		}

		else if(type == 1){
			
			if((pos == 0) || (pos == 1))
				continue;
			if(str[pos] >= 'a' && str[pos] <= 'f')
				result += (str[pos]-87) << 4*shifter;	  // to get value from 10 to 15

			else if(str[pos] >= 'A' && str[pos] <= 'F') 
				result += (str[pos]-55) << 4*shifter;	  // to get value from 10 to 15

			else
				result += (str[pos] - '0') << 4*shifter;  // to get value from 0 to 9
 			
			shifter++;	
		}

		else if(type == 2){
			if((pos == 0) || (pos == 1))
				continue;

			result += (str[pos] - '0') << shifter;
			shifter++;
		}
	
		else if(type == 3){
			result += (str[pos] - '0') << 3*shifter;
			shifter++;
		}
		
	}

	return result;
}


/*
* convIntToStr()
* 	Converts integer to string.
*/
char *convIntToStr(int numb){
	char *res = stringInit();
	char tmp;
	bool sign = false;
	
	if(numb < 0){
		sign = true;
		numb *= -1;			
	}
	
	while(numb > 0){
		charPut(&res, numb % 10 + '0');
		if (errflg){
			free(res);
			return NULL;
		}
		numb /= 10;
	}
	
	if( strlen(res) == 0){
		charPut(&res, '0');
		if (errflg){
			free(res);
			return NULL;
		}
		return res;
	}
		
	
	if(sign){
		charPut(&res, '-');
		if (errflg){
			free(res);
			return NULL;
		}
	}

	for(unsigned int i = 0; i < strlen(res)/2; i++){
		tmp = res[i];
		res[i] = res[strlen(res)-i-1];
		res[strlen(res)-i-1] = tmp;
	}

	return res;	
}


/*
* convStrToDouble()
* 	Converts string to float/double.
*/
double convStrToDouble(string str){
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

string tokToStr(TToken tok){
	
	switch(tok.type){
		case TOK_ID:
			return "TOK_ID";

		case TOK_ADD:
			return "TOK_ADD";
			
		case TOK_SUB:
			return "TOK_SUB";

		case TOK_MUL:
			return "TOK_MUL";

		case TOK_DIV:
			return "TOK_DIV";

		case TOK_LT:
			return "TOK_LT";

		case TOK_GT:
			return "TOK_GT";

		case TOK_LEQ:
			return "TOK_LEQ";
	
		case TOK_GEQ:
			return "TOK_GEQ";

		case TOK_EQ:
			return "TOK_EQ";

		case TOK_NEQ:
			return "TOK_NEQ";

		case TOK_ASSIGN:
			return "TOK_ASSIGN";

		case TOK_LBR:
			return "TOK_LBR";

		case TOK_RBR:
			return "TOK_RBR";

		case TOK_COMMA:
			return "TOK_COMMA";

		case TOK_EOL:
			return "TOK_EOL";

		case TOK_EOF:
			return "TOK_EOF";
		
		case TOK_INT:
			return "TOK_INT";

		case TOK_FLOAT:
			return "TOK_FLOAT";

		case TOK_STRING:
			return "TOK_STRING";
		
		case TOK_KEY:
			if(!strcmp(tok.data.s, "nil"))
				return "TOK_STRING";
						
			else
				return tok.data.s;

		default:
			return tok.data.s;
	}
}
