/**
	file name:		main.c
	project:		VUT-FIT-IFJ-2018
	created:		25.11.2018
	last modified:	01.12.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Main				
*/
#include<stdio.h>
#include "s_table.h"
#include "ifj_error.h"
#include "scanner.h"
#include "parser.h"
#include "code_gen_list.h"

int main() {
    errflg = PROGRAM_OK;    //extern from ifj_error.h
    FILE *f = stdin;
    TsymItem *GT = NULL;    //global symbol table
    TsymItem *LT = NULL;    //local symbol table
    TInstrList instructions;//instructions in 3AC interstep
    symTabInit(&GT); 
    symTabFillKwds(&GT); 
    symTabFillFuns(&GT); 
    if (errflg == ERR_RUNTIME) {    //allocation error in table
        symTabDispose(&GT);
        return errflg;
    }
    symTabInit(&LT);
    ILInit(&instructions);
    tokBuffInit();
    genPrgBegin(&instructions);
    int result = parse(f, GT, LT, &instructions);
    if (result == PROGRAM_OK)
        ILPrintAllInst(instructions);
        
    ILDisposeList(&instructions);
    symTabDispose(&LT);
    symTabDispose(&GT);
    return result;
}

/* end of main.c */