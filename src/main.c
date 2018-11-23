#include <stdlib.h>
#include <stdio.h>
#include "stack_list.h"


int main( void ){
	tStackLPtr s = sLInit();
	sLDelete(s);
	return 0;
}
