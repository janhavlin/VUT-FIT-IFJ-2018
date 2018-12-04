/**
 *	file name:		type_conv.h
 *	project:		VUT-FIT-IFJ-2018
 *	created:		19.10.2018
 *	last modified:	19.10.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *	modifications:	
 *	
 *	description:	Converting types to different types
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dyn_arr.h"
#include "scanner.h"


#ifndef TYPE_CONV
#define TYPE_CONV

/**
 * @brief Converts String into Integer
 * 
 * @param str String that needs to be converted into Integer
 * @return Integer that was converted from String
 */
int convStrToInt(string str);			

/**
 * @brief Converts Integer into String
 * 
 * @param numb Number that needs to be converted
 * @return String that was converted from Integer
 */
string convIntToStr(int numb);		

/**
 * @brief Converts structure TToken into string with name of token
 * 
 * @param tok Structure with token data
 * @return Name of token that was converted from TToken structure
 */
string tokToStr(TToken tok);

/**
 * @brief Converts String into Double
 * 
 * @param str String that needs to be converted into  Double
 * @return Double that was converted from String
 */
double convStrToDouble(string str);		


#endif //TYPE_CONV
