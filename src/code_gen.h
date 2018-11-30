/*
 * file name:       code_gen.h
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   25.11.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modification:
 * 
 * description: Functions for converting specific block of code into three address code. 
 */

#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>

typedef struct tDLElem {
    char *inst;
    struct tDLElem *lptr;
    struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct {
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
} tDLList;

void ADDToAssembler();      // convert ADD into 3 address code
void SUBToAssembler();      // convert SUB into 3 address code
void MULToAssembler();      // convert MUL into 3 address code
void DIVToAssembler();      // convert DIV into 3 address code
void IFToAssembler();       // convert IF into 3 address code
void WHILEToAssembler();    // convert WHILE into 3 address code
void FUNToAssembler();      // convert FUN definition into 3 address code

#endif  // CODE_GEN_H
