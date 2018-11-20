#include "Tifj_error.h"

//error("ERROR in parser.c in func. main: allocation failed! Code=%d\n", ERR_RUNTIME);

// funkce pro vypis chyboveho hlaseni
void error(const char *fmt, ...){   
	va_list ap;
        va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
        va_end(ap);		
}	
