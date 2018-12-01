/*
 * file name:       code_gen.h
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   30.11.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modification:
 * 
 * description: Functions for converting specific block of code into three address code. 
 */

#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "code_gen_list.h"
#include "ifj_error.h"
#include "type_conv.h"

/**
 * @brief Creates label for main body of the program
 * 
 * @param L Pointer to a list of instructions 
 */
void genPrgBegin(TInstrList *L);

void genPrgEnd(TInstrList *L);
/**
 * @brief Creates a frame for a new function call
 * 
 * @param L Pointer to a list of instructions
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallBegin(TInstrList *L, bool inWhile);

/**
 * @brief Defines a parameter of the function and moves it to temporary frame
 * 
 * @param L Pointer to a list of instructions
 * @param parNum Number of the parameter passed in this function call
 * @param var Structure that holds data (either name of a variable or a constant)
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallPar(TInstrList *L, unsigned parNum, TAdr var, bool inWhile);

/**
 * @brief Pushes the temporary frame and calls the function
 * 
 * @param L Pointer to a list of instructions
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallEnd(TInstrList *L, char *fun, bool inWhile);

/**
 * @brief Creates 'begin' label of a while
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileBegin(TInstrList *L, unsigned whileCnt, bool inWhile);

/**
 * @brief Decides whether to skip body of the while or not
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt Index of current while
 * @param psa Index of PSA that evaluated condition 
 * @param res Index of temporary variable of PSA containing result of the PSA
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileCond(TInstrList *L, unsigned whileCnt, unsigned psa, unsigned res, bool inWhile);

/**
 * @brief Jumps to the beginning of the while and creates label for ending
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt  Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileEnd(TInstrList *L, unsigned whileCnt, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> i' rule
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @param var Structure that holds data (either name of a variable or a constant)
 * @param inWhile Flag whether it's called from inside a while
 */
void genE(TInstrList *L, unsigned psa, unsigned res, TAdr var, bool inWhile);

/**
 * @brief Defines a new variable and assigns nil to it
 * 
 * @param L Pointer to a list of instructions
 * @param var Name of the variable
 * @param inWhile Flag whether it's called from inside a while
 */
void genDefVar(TInstrList *L, char *var, bool inWhile);

/**
 * @brief 
 * 
 * @param L Pointer to a list of instructions
 * @param var Name of a variable to which will be assigned
 * @param psa Index of PSA that evaluated the expression
 * @param E Index of the E variable that holds the value we want to assign
 * @param inWhile Flag whether it's called from inside a while
 */
void genAssign(TInstrList *L, char *var, unsigned psa, unsigned E, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> E + E' rule
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @param var1 Index of the first 'E' operand
 * @param var2 Index of the second 'E' operand
 * @param inWhile Flag whether it's called from inside a while
 */
void genAdd(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile);

#endif  // CODE_GEN_H
