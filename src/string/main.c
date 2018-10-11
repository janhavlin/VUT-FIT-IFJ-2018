#include <stdlib.h>
#include <stdio.h>
#include "dyn_arr.h"

int main( void ){
	string a;

	if((a = string_init()) == NULL){
		return 1;	
	}
	
	char_put(a, 'a');
	char_put(a, 'h');
	char_put(a, 'o');
	char_put(a, 'j');
	char_put(a, ' ');
	char_put(a, 'j');
	char_put(a, 'a');
	char_put(a, 'k');
	char_put(a, ' ');
	char_put(a, 's');
	char_put(a, 'e');
	char_put(a, ' ');
	char_put(a, 'm');
	char_put(a, 'a');
	char_put(a, 's');
	char_put(a, '?');

	printf("Vysledek: ");

	printf("%s <------text\n",a);

	free(a);

	return 0;
}
