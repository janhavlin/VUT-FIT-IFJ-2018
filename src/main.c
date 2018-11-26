/**
	file name:		main.c
	project:		VUT-FIT-IFJ-2018
	created:		25.11.2018
	last modified:	
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Main				
*/
#include "main.h"
#define DEBUG 0

int main() {
    //symbol tables initialization
    TsymItem *GT = NULL;
    TsymItem *LT = NULL;
    symTabInit(&GT); //TODO: can throw error
    symTabAddKwds(&GT); //TODO: can throw error
    symTabAddFuns(&GT); //TODO: can throw error
    symTabInit(&LT); //TODO: can throw error
    FILE *f = stdin;
    
    parserStart(f, GT, LT); //TODO: can throw error
	
    return errflg;
}

/* end of main.c */