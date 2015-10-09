/********************************************************
 * File		: Lagrange
 * Author	: Lei Zhang
 * Update	: 2015-10-04
*/
#ifndef _LAGRANGE_H
#define _LAGRANGE_H

#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
/********************************************************
 * Lagrange: generate result when x=0 (the Hpwd=a0)
 * XNum is the number of unknown variable X in f(x)
 * TotalDataCount is the amount of known data groups (x,f(x))
 * DataX is the array for variable X
 * DataY is the array for variable Y
*/
int Lagrange(mpf_t result,int TotalDataCount,mpf_t* DataX,mpf_t* DataY);

long Xround (mpf_t result);

#endif//_LAGRANGE_H
