/**
 *   file name:      ifj_error.h
 *   project:        VUT-FIT-IFJ-2018
 *   created:        17.10.2018
 *   last modified:  05.12.2018
 *
 *   created by:     Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *   modifications:  Jan Havlín  xhavli47@stud.fit.vutbr.cz
 *
 *   description:    There are names of errors and function to print error
*/

#include "ifj_error.h"


void ifjErrorPrint(const char *fmt, ...){   
	va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, "Line %d: ", linecnt);
	vfprintf(stderr, fmt, ap);
        va_end(ap);		
}	
