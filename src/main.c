/**
	file name:		main.c
	project:		VUT-FIT-IFJ-2018
	created:		25.11.2018
	last modified:	27.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Main				
*/
#include "main.h"
#define DEBUG 0

int main() {
    errflg = PROGRAM_OK;    //extern from ifj_error.h
    TsymItem *GT = NULL;    //global symbol table
    TsymItem *LT = NULL;    // local symbol table
    symTabInit(&GT); 
    symTabAddKwds(&GT); 
    symTabAddFuns(&GT); 
    if (errflg == ERR_RUNTIME) {    //allocation error in table
        symTabDispose(&GT);
        return errflg;
    }
    symTabInit(&LT);
    FILE *f = stdin;
    
    int result = parserStart(f, GT, LT);
    symTabDispose(&LT);
    symTabDispose(&GT);
    return result;
}

/* end of main.c */