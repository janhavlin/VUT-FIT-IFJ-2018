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

/**
 * @brief Creates a frame for a new function call
 * 
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallBegin(bool inWhile);

/**
 * @brief Defines a parameter of the function and moves it to temporary frame
 * 
 * @param var Name of the parameter
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallPar(char *var, bool inWhile);

/**
 * @brief Pushes the temporary frame and calls the function
 * 
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallEnd(char *fun, bool inWhile);

/**
 * @brief Creates 'begin' label of a while
 * 
 * @param whileCnt Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileBegin(unsigned whileCnt, bool inWhile);

/**
 * @brief Decides whether to skip body of the while or not
 * 
 * @param whileCnt Index of current while
 * @param psa Index of PSA that evaluated condition 
 * @param res Index of temporary variable of PSA containing result of the PSA
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileCond(unsigned whileCnt, unsigned psa, unsigned res, bool inWhile);

/**
 * @brief Jumps to the beginning of the while and creates label for ending
 * 
 * @param whileCnt  Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileEnd(unsigned whileCnt, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> i' rule
 * 
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @TODO: support constants (needs union)
 * @param var Name of the variable to be converted to E
 * @param inWhile Flag whether it's called from inside a while
 */
void genE(unsigned psa, unsigned res, char *var, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> E + E' rule
 * 
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @param var1 Index of the first 'E' operand
 * @param var2 Index of the second 'E' operand
 * @param inWhile Flag whether it's called from inside a while
 */
void genAdd(unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile);

/**
 * @brief Defines a new variable and assigns nil to it
 * 
 * @param var Name of the variable
 * @param inWhile Flag whether it's called from inside a while
 */
void genDefVar(char *var, bool inWhile);

#endif  // CODE_GEN_H
