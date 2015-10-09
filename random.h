/********************************************************
 * File		: Random
 * Author	: Lei Zhang
 * Update	: 2015-10-03
*/
#ifndef _RANDOM_H
#define _RANDOM_H

#include <time.h>
#include <gmp.h>

/********************************************************
 * RandomNumber: generate random number, not necessarily
   a prime... just a random number
*/
int RandomNumber(mpz_t key_q);

/********************************************************
 * RandomPrime: generate random number as a prime
*/
int RandomPrime(mpz_t key_q);

/********************************************************
 * InitialRandomNumber: Initial a random number
*/
int InitialRandomNumber(mpz_t key_q);

/********************************************************
 * ModRandom: Mod operation on random number
 * result = base mod num
*/
void ModRandom (mpz_t result, mpz_t base, mpz_t num);

/********************************************************
 * CompareRandom: Compare random numbers
 * Return 0 when operator1 is bigger
 * Return -1 when operator1 is not bigger
*/
int CompareRandom (mpz_t operator1, mpz_t operator2);

#endif//_RANDOM_H
