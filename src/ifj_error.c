/**
        file name:	ifj_error.c
        project:	VUT-FIT-IFJ-2018
        created:	22.10.2018
        last modified:	22.10.2018

        created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
        modifications:	

        description:	File with error numbers and error printing function
*/

#include "ifj_error.h"

// funkce pro vypis chyboveho hlaseni
void ifjErrorPrint(const char *fmt, ...){   
	va_list ap;
        va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
        va_end(ap);		
}	
