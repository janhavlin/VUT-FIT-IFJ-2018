#include "ifj_error.h"

// funkce pro vypis chyboveho hlaseni
void error(const char *fmt, ...){   
	va_list ap;
        va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
        va_end(ap);		
}	
