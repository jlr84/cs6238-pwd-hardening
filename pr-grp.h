/***********************************************************
 * File		: Random
 * Author	: Lei Zhang
 * Update	: 2015-10-10
*/
#ifndef _PR_H
#define _PR_H

#include <time.h>
#include <gmp.h>

/***********************************************************
 * hash: generate hash based on DJB Hash Function
*/
unsigned long hash(unsigned char *str, unsigned long hash);

/***********************************************************
 * PR: generate random number for function Pr(2i)
*/
int PR(mpz_t key_q, mpz_t r, unsigned long seed);

/***********************************************************
 * GRP: generate random number for function Grp(2i)
*/
int GRP(mpz_t key_q, mpz_t r, unsigned char *password, unsigned long seed);

#endif//_RANDOM_H
