/*
 * file name:       translation.h
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   23.11.2018
 * 
 * created by:  Petr Bobčík, xbobci02@stud.fit.vutbr.cz
 * modification:
 * 
 * description: Header file with prototypes of functions for converting specific block of code into
 *              three address code. 
 */

#ifndef TRANSLATION_IFJ
#define TRANSLATION_IFJ

void ADDToAssembler();      // convert ADD into 3 address code
void SUBToAssembler();      // convert SUB into 3 address code
void MULToAssembler();      // convert MUL into 3 address code
void DIVToAssembler();      // convert DIV into 3 address code
void IFToAssembler();       // convert IF into 3 address code
void WHILEToAssembler();    // convert WHILE into 3 address code
void FUNToAssembler();      // convert FUN definition into 3 address code

#endif  // TRANSLATION_IFJ