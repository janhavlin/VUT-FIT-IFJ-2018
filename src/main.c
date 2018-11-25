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
    TsymItem *globalSymTable = NULL;
    TsymItem *localSymTable = NULL;
    symTabInit(&globalSymTable); //TODO: can throw error
    fillTabWithKwds(&globalSymTable); //TODO: can throw error
    symTabInit(&localSymTable); //TODO: can throw error
    File *f = stdin;
    
    parserStart(globalSymTable, localSymTable, f); //TODO: can throw error
	
    return errflg;
}

/* end of main.c */