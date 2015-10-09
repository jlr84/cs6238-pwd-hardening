#include <time.h>
#include <gmp.h>
#include "random.h"

//gcc gmp.c -lgmp -o gmp

int RandomNumber(mpz_t key_q)
{
  gmp_randstate_t grt;

  //Set random generation algorithm by default 
  gmp_randinit_default(grt);

  //Set a random seed with current time as the satement srand(time(NULL)) in C 
  gmp_randseed_ui(grt, time(NULL));

  //Initialize data type mpz_t (mpz can be applied only if it is initialized)
  mpz_init(key_q);

  //Generate a random number between 0~2^160-1
  mpz_urandomb(key_q, grt, 160);

  //Print the 16-bit prime in Decimal
//  gmp_printf("%Zd\n", key_q);

  return 0;
}

int RandomPrime(mpz_t key_q)  
{  
  gmp_randstate_t grt;

  //Set random generation algorithm by default 
  gmp_randinit_default(grt);

  //Set a random seed with current time as the satement srand(time(NULL)) in C 
  gmp_randseed_ui(grt, time(NULL));

  //Initialize data type mpz_t (mpz can be applied only if it is initialized)
  mpz_init(key_q);

  //Generate a random number between 0~2^160-1
  mpz_urandomb(key_q, grt, 160);
  
  //if the random number is even, then it is added by 1
  if(mpz_even_p(key_q))
  {
    mpz_add_ui(key_q, key_q, 1);
  }

  //Verify the prime with GMP function
  while(!mpz_probab_prime_p(key_q, 25) > 0)
  {
    mpz_add_ui(key_q, key_q, 2);
  }
    
  //Print the 16-bit prime in Hex
//  gmp_printf("%ZX\n", key_q);

  //Print the 16-bit prime in Decimal
//  gmp_printf("%Zd\n", key_q);  
  
  
  return 0;  
}

int InitialRandomNumber(mpz_t key_q)
{
  mpz_init(key_q);
  
  return 0;
}

void ModRandom (mpz_t result, mpz_t base, mpz_t num)  
{
  mpz_init(result);

  //result = base mod num
  mpz_mod(result, base, num);  
  
  gmp_printf("The mod result in ModRandom function is: %Zd\n", result);  
}

int CompareRandom (mpz_t operator1, mpz_t operator2)  
{
  //Operators need to be initialized before application
  int comparison = 0;
  comparison = mpz_cmp(operator1, operator2);
  
  if (comparison > 0)
  {
    gmp_printf("The Operator1 is bigger\n");
    return 0;
  }
  else
  {
    gmp_printf("The Operator1 is not bigger\n");
    return -1;
  }  
}
