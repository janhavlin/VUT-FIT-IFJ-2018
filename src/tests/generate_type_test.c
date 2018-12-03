#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>



#define OP_AMOUNT		10
#define OP_LENG			3
#define ITEM_AMOUNT		8
#define ITME_LENG		10
#define CONSTR_AMOUNT 	3
#define CONSTR_LENG		10	

int convStrToInt(char *str);
double convStrToDouble(char *str);

#define IS_FLOAT(item)  ( (!strcmp(item, "vf"))   || (!strcmp(item, "1.68")) )
#define IS_INT(item)    ( (!strcmp(item, "vi"))   || (!strcmp(item, "3")) )
#define IS_NIL(item)    ( (!strcmp(item, "vnil")) || (!strcmp(item, "nil")) )
#define IS_STRING(item) ( (!strcmp(item, "vs"))   || (!strcmp(item, "\"text\"")) )

#define ONLY_FLOAT(item1, item2){\
									( ( (!strcmp(item1, "vf")) || (!strcmp(item1, "1.68")) ) && \
								 	( (!strcmp(item2,  "vf")) || (!strcmp(item2, "1.68")) ) )  \
								}	
								
#define ONLY_INT(item1, item2){\
									( ( ( !strcmp(item1, "vi") ) || (!strcmp(item1, "3")) ) && \
					 				( (!strcmp(item2,  "vi") )   || (!strcmp(item2, "3")) ) )  \
								}								 	
	
#define ONLY_STRING(item1, item2){\
									( ( ( !strcmp(item1, "vs") ) || (!strcmp(item1, "\"text\"")) ) && \
				 					( (!strcmp(item2,  "vs") )   || (!strcmp(item2, "\"text\"")) ) )  \
								}
								
#define ONLY_NIL(item1, item2){\
									( ( (!strcmp(item1, "vnil") ) || (!strcmp(item1, "nil")) ) && \
				 					( (!strcmp(item2,  "vnil") ) || (!strcmp(item2, "nil")) ) )  \
								}	
														
#define ONLY_NUMB(item1, item2){\
									( (IS_FLOAT(item1)) || (IS_INT(item1)) ) && \
									( (IS_FLOAT(item2)) || (IS_INT(item2)) )    \
								}


#define ONE_FLOAT(item1, item2){\
									(((!strcmp(item1, "vf")) || (!strcmp(item1, "1.68"))) && \
								 	((!strcmp(item2,  "vf")) || (!strcmp(item2, "1.68"))))  \
								}
								
#define ONE_INT(item1, item2){\
									(((!strcmp(item1, "vi")) || (!strcmp(item1, "3"))) && \
								 	((!strcmp(item2,  "vi")) || (!strcmp(item2, "3"))))  \
								}

#define ONE_STRING(item1, item2){\
									(((!strcmp(item1, "vs")) || (!strcmp(item1, "\"text\""))) && \
								 	((!strcmp(item2,  "vs")) || (!strcmp(item2, "\"text\""))))  \
								}
								
#define ONE_NIL(item1, item2){\
									(((!strcmp(item1, "vnil")) || (!strcmp(item1, "nil"))) && \
								 	((!strcmp(item2,  "vnil")) || (!strcmp(item2, "nil"))))  \
								}																								
							
			
#define STRING_OPER_TEST(item1, op, item2) 	{\
													 	if(!strcmp(op, "+")){\
															printf("$ 0 texttext\n\n");\
														}\
														else if(!strcmp(op, "<")){\
															printf("$ 0 0\n\n");\
														}\
														else if(!strcmp(op, ">")){\
															printf("$ 0 0\n\n");\
														}\
														else if(!strcmp(op, "<=")){\
															printf("$ 0 1\n\n");\
														}\
														else if(!strcmp(op, ">=")){\
															printf("$ 0 1\n\n");\
														}\
														else if(!strcmp(op, "==")){\
															printf("$ 0 1\n\n");\
														}\
														else if(!strcmp(op, "!=")){\
															printf("$ 0 0\n\n");\
														}\
													}
													
#define NUMB_OPER_TEST(item1, op, item2) 	{\
													 	if(!strcmp(op, "+")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 %a\n\n", 1.68+1.68);\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 0\n\n", 6);\
															else\
																printf("$ 0 %a\n\n", 3+1.68);\
														}\
													 	if(!strcmp(op, "-")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 %a\n\n", 0);\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 %a\n\n", 1.68-3);\
															else\
																printf("$ 0 %a\n\n", 3-1.68);\
														}\
														else if(!strcmp(op, "<")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 1\n\n");\
															else\
																printf("$ 0 0\n\n");\
														}\
														else if(!strcmp(op, ">")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 0\n\n");\
															else\
																printf("$ 0 1\n\n");\
														}\
														else if(!strcmp(op, "<=")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 1\n\n");\
															else\
																printf("$ 0 0\n\n");\
														}\
														else if(!strcmp(op, ">=")){\
												 			if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 0\n\n");\
															else\
																printf("$ 0 1\n\n");\
														}\
														else if(!strcmp(op, "==")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 1\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 0\n\n");\
															else\
																printf("$ 0 0\n\n");\
														}\
														else if(!strcmp(op, "!=")){\
													 		if(ONLY_FLOAT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if(ONLY_INT(item1, item2))\
																printf("$ 0 0\n\n");\
															else if( IS_FLOAT(item1) )\
																printf("$ 0 1\n\n");\
															else\
																printf("$ 0 1\n\n");\
														}\
												}

bool sameType(char *item1, char *item2){
									return ((ONLY_FLOAT(item1, item2))   || (ONLY_INT(item1, item2)) ||
									(ONLY_STRING(item1, item2)) || (ONLY_NIL(item1, item2)));
								}

int convStrToInt(char *str){			
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

double convStrToDouble(char *str){
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



char constr[CONSTR_AMOUNT][CONSTR_LENG] = {
											{"while"},
											{"if"}
										  };

char op[OP_AMOUNT][OP_LENG] = {
								{"+"},
								{"-"},
								{"*"},
								{"/"},
								{"<"},
								{"<="},
								{">"},
								{">="},
								{"=="},
								{"!="}
							  };
	
char item[ITEM_AMOUNT][ITME_LENG] = {
										{"3"},
										{"1.68"},
										{"\"text\""},								
										{"nil"},
										{"vi"},
										{"vf"},
										{"vs"},
										{"vnil"}
									};


void result(char *item1, char *op, char *item2){	// type: 0 - bool, 1 - int, 2 - float, 3 - string 
	// not same type
	if( !sameType(item1, item2) ){
		
		int x = ONE_STRING(item1, item2);
		if( x ){

			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					if(!strcmp(op, "=="))
						printf("$ 0 0\n\n");	// second 0 == false
					else
						printf("$ 0 1\n\n");
					return;
			}
					
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		x = ONE_NIL(item1, item2 );
		if( x ){
			if(!strcmp(op, "==") || !strcmp(op, "!=")){
				if(!strcmp(op, "=="))
					printf("$ 0 0\n\n");
					
				else
					printf("$ 0 1\n\n");
				return;	
			}
					
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		
		else if( ONLY_NUMB(item1, item2) ){
			if( ONLY_FLOAT(item1, item2) ){
		  	  	printf("$ 0 %a\n\n", 1.68 + 1.68);
		  	  	return;
		  	}
		  	else if( ONLY_INT(item1, item) ){
				printf("$ 0 %d\n\n", 6);
				return;
		  	}
			return;
		}
	}
	
	// same type
	else{
		if( ONLY_NIL(item1, item2) ){
			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					if(!strcmp(op, "=="))
						printf("$ 0 1\n\n");
						
					else
						printf("$ 0 0\n\n");
	
					return;
			}
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		else if(ONLY_STRING(item1, item2)){
			if(!strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/")){
				printf("$ 4\n\n");
				return;
			}
			else{
			
				STRING_OPER_TEST(item1, op, item2);
				return;	
			}
		}
			
	  	else if( ONLY_NUMB(item1, item2) ){
		  	NUMB_OPER_TEST(item1, op, item2);
			return;
		}
		
	}
}
						
						
						
						
						
int main(){
	int cntr = 0;
	for(int c = 0; c < 2; c++){
		for(int i = 0; i < ITEM_AMOUNT; i++){
			for(int x = 0; x < OP_AMOUNT; x++ ){
				for(int y = 0; y < ITEM_AMOUNT; y++){
					printf("vi = 3\nvf = 1.68\nvs = \"text\"\nvnil=nil\n");
					
					if(c % 2)
						printf("if( 5 ) then\n");

					printf("c = %s %s %s\n", item[i], op[x], item[y]);
					if(cntr++ % 2)
						printf("print c\n");	
					else
						printf("print(c)\n");			
					result(item[i], op[x], item[y]);
					if(c % 2)
						printf("end\n");
				}
			}
		}
	}
	return 0;
}
