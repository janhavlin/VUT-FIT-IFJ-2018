/**
 *   file name:      ifj_error.h
 *   project:        VUT-FIT-IFJ-2018
 *   created:        17.10.2018
 *   last modified:  17.10.2018
 *
 *   created by:     Petr Bobčík xbobci02@stud.fit.vutbr.cz
 * 
 *   modifications:	
 *
 *   description:    There are names of errors and function to print error
*/

#include "ifj_error.h"


void ifjErrorPrint(const char *fmt, ...){   
	va_list ap;
        va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
        va_end(ap);		
}	
