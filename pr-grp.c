#include <time.h>
#include <gmp.h>
#include "pr-grp.h"

// DJB Hash Function
unsigned long hash(unsigned char *str, unsigned long hash)
{
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}


int PR(mpz_t key_q, mpz_t r, unsigned long seed)  
{  
  gmp_randstate_t grt;

  //Set random generation algorithm by default 
  gmp_randinit_default(grt);

  //Set a random seed with current time as the satement srand(time(NULL)) in C 
  gmp_randseed_ui(grt, seed);

  //Initialize data type mpz_t (mpz can be applied only if it is initialized)
  mpz_init(key_q);

  //Generate a random number between 0~2^160-1
  mpz_urandomm(key_q, grt, r); 
  
  
  return 0;  
}

int GRP(mpz_t key_q, mpz_t r, unsigned char *password, unsigned long seed)
{
  seed = hash(password, seed);
  PR(key_q, r, seed);
  return 0;
}

