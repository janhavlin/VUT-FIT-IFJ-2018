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

/**
 * @brief Useless function
 * 
 * @param L Pointer to a list of instructions
 */
void genPrgEnd(TInstrList *L);

/**
 * @brief Creates a label for a new function definition
 * 
 * @param L Pointer to a list of instructions
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genFunDefBegin(TInstrList *L, char *fun, bool inWhile, bool inFunDef);

/**
 * @brief Creates RETURN instruction
 * 
 * @param L Pointer to a list of instructions
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genFunDefEnd(TInstrList *L, bool inWhile, bool inFunDef);

/**
 * @brief Creates a frame for a new function call
 * 
 * @param L Pointer to a list of instructions
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genFunCallBegin(TInstrList *L, char *fun, bool inWhile, bool inFunDef);

/**
 * @brief Defines a parameter of the function and moves it to temporary frame
 * 
 * @param L Pointer to a list of instructions
 * @param fun Name of the function
 * @param parNum Number of the parameter passed in this function call
 * @param var Structure that holds data (either name of a variable or a constant)
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genFunCallPar(TInstrList *L, char *fun, unsigned parNum, TAdr var, unsigned funParOrder, bool inWhile, bool inFunDef);

/**
 * @brief Pushes the temporary frame and calls the function
 * 
 * @param L Pointer to a list of instructions
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genFunCallEnd(TInstrList *L, char *fun, bool inWhile, bool inFunDef);

/**
 * @brief Creates 'begin' label of a while
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt Index of current while
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genWhileBegin(TInstrList *L, unsigned whileCnt, bool inWhile, bool inFunDef);

/**
 * @brief Decides whether to skip body of the while or not
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt Index of current while
 * @param psa Index of PSA that evaluated condition expression
 * @param psaResultE Index of temporary variable of PSA containing result of the PSA
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genWhileCond(TInstrList *L, unsigned whileCnt, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef);

/**
 * @brief Jumps to the beginning of the while and creates label for ending
 * 
 * @param L Pointer to a list of instructions
 * @param whileCnt  Index of current while
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genWhileEnd(TInstrList *L, unsigned whileCnt, bool inWhile, bool inFunDef);

/**
 * @brief Decides whether to jump into else branch or then branch
 * 
 * @param L Pointer to a list of instructions
 * @param ifCnt Index of current if statement
 * @param psa Index of PSA that evaluated condition expression
 * @param psaResultE Index of temporary variable of PSA containing result of the PSA
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genIfCond(TInstrList *L, unsigned ifCnt, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef);

/**
 * @brief Creates a label for else branch
 * 
 * @param L Pointer to a list of instructions
 * @param ifCnt Index of current if statement
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genIfElse(TInstrList *L, unsigned ifCnt, bool inWhile, bool inFunDef);

/**
 * @brief Creates a label at the end of the if
 * 
 * @param L Pointer to a list of instructions
 * @param ifCnt Index of current if statement
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genIfEnd(TInstrList *L, unsigned ifCnt, bool inWhile, bool inFunDef);

/**
 * @brief Function for evaluating PSA 'E -> i' rule
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param resultE Index of 'E' to store the result
 * @param var Structure that holds data (either name of a variable or a constant)
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genE(TInstrList *L, unsigned psa, unsigned resultE, TAdr var, bool inWhile, bool inFunDef);

/**
 * @brief Defines a new variable and assigns nil to it
 * 
 * @param L Pointer to a list of instructions
 * @param var Name of the variable
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genDefVar(TInstrList *L, char *var, bool inWhile, bool inFunDef);

/**
 * @brief 
 * 
 * @param L Pointer to a list of instructions
 * @param var Name of a variable to which will be assigned
 * @param funParOrder
 * @param psa Index of PSA that evaluated the expression
 * @param E Index of the E variable that holds the value we want to assign
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genAssign(TInstrList *L, char *var, unsigned funParOrder, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef);

/**
 * @brief 
 * 
 * @param L 
 * @param var 
 * @param funParOrder 
 * @param inWhile 
 * @param inFunDef 
 */
void genAssignRetval(TInstrList *L, char *var, unsigned funParOrder, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and adds them if they are compatible
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genAdd(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and subtracts them if they are compatible
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genSub(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and multiplies them if they are compatible
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genMul(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and divides them if they are compatible, second operand can't be zero
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genDiv(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates less than operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genLT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates greater than operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genGT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates less or equal than operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genLEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates greater or equal than operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genGEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates equal operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);

/**
 * @brief Compares types of two operands and evaluates not equal than operation
 * 
 * @param L Pointer to a list of instructions
 * @param psa Index of current PSA
 * @param res Index of E that will store a result of the operation
 * @param var1 Index of E that stores first operand
 * @param var2 Index of E that stores second operand
 * @param inWhile Flag whether it's called from inside a while
 * @param inFunDef Flag whether it's called from inside a function definition
 */
void genNEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef);
#endif  // CODE_GEN_H
