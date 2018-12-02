#include <stdio.h>
#include <string.h>


#define OP_AMOUNT	10
#define OP_LENG		3
#define ITEM_AMOUNT	8
#define ITME_LENG	10

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
										{"1"},
										{"1.68"},
										{"\"text\""},								
										{"nil"},
										{"vi"},
										{"vf"},
										{"vs"},
										{"vnil"}
									};
									
									
void isCorrect(char *item1, char *op, char *item2){
	if( strcmp(item1, item2) ){
		
		// oba stringy
		if( !strcmp(item1, "\"text\"") && !strcmp(item2, "vs") ){
			if(!strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/")){
				printf("$ 4\n\n");
				return;
			}
			printf("$ 0\n\n");
			return;
		}
		
		if( !strcmp(item2, "\"text\"") && !strcmp(item1, "vs")){
			if(!strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/")){
				printf("$ 4\n\n");
				return;
			}
			printf("$ 0\n\n");
			return;
		}
		
		// neco je string
		if((!strcmp(item1, "\"text\"")) || (!strcmp(item1, "vs"))){

			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					printf("$ 0\n\n");
					return;
			}
					
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		if((!strcmp(item2, "\"text\"")) || (!strcmp(item2, "vs"))){
			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					printf("$ 0\n\n");
					return;
			}
					
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		// neco je nil
		if((!strcmp(item1, "nil")) || (!strcmp(item1, "vnil"))
		  ||(!strcmp(item2, "nil")) || (!strcmp(item2, "vnil"))){
			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					printf("$ 0\n\n");
					return;
			}
					
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		
		else{
			printf("$ 0\n\n");
			return;
		}
	}
	
	else{
		if((!strcmp(item1, "nil")) || (!strcmp(item1, "vnil"))
		  ||(!strcmp(item2, "nil")) || (!strcmp(item2, "vnil"))){
			if(!strcmp(op, "==") || !strcmp(op, "!=")){
					printf("$ 0\n\n");
					return;
			}
			else{
				printf("$ 4\n\n");
				return;
			}
		}
		
		if((!strcmp(item1, "\"text\"")) || (!strcmp(item1, "vs"))){
			if(!strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/")){
				printf("$ 4\n\n");
				return;
			}
			else{
				printf("$ 0\n\n");
				return;
			}
		}
			
		else{
			printf("$ 0\n\n");
			return;
		}
	}

}
						
int main(){
int cntr = 0;
	for(int i = 0; i < ITEM_AMOUNT; i++){
		for(int x = 0; x < OP_AMOUNT; x++ ){
			for(int y = 0; y < ITEM_AMOUNT; y++){
				printf("vi = 3\nvf = 9.1\nvs = \"text\"\nvnil=nil\n");
				printf("c = %s %s %s\n", item[i], op[x], item[y]);
				isCorrect(item[i], op[x], item[y]);				
			}
		}
	}
	return 0;
}
