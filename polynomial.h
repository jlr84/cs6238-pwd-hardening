/********************************************************
 * File		: Polynomial
 * Author	: Lei Zhang
 * Update	: 2015-09-30
*/
#ifndef _POLYNOMIAL_H
#define _POLYNOMIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <gmp.h>

#define MaxDegree 100
#define NUM 100

/*Define Polynomial Structure*/
typedef struct
{
  long CoeffArray[MaxDegree + 1]; /*coeffecient*/
  int HighPower; /*the highest power*/
} Polynomial;


/********************************************************
 * ZeroPolynomial: initialize polynomial
*/
void ZeroPolynomial(Polynomial *Poly, int RealDegree);

/********************************************************
 * InputPolynomial: set value of each coeffecient for polynomial
*/
void InputPolynomial(Polynomial *Poly, int RealDegree);
/********************************************************
 * PrintPolynomial: print polynomial
*/
void PrintPolynomial(Polynomial Poly);
/********************************************************
 * AddPolynomial: Calculate the sum of specific polynomial
*/
int AddPolynomial(Polynomial Poly, mpz_t num, mpz_t sum);

#endif//_POLYNOMIAL_H
