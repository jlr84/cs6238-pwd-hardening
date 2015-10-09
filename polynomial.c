#include <gmp.h>
#include "polynomial.h"

/*Initialize Polynomial*/
void ZeroPolynomial(Polynomial *Poly, int RealDegree)
{
  if (RealDegree > MaxDegree)
  {
    printf("The Maxmum Degree is %d", MaxDegree);
  }
  else
  {
    int i;
    for (i = 0; i <= RealDegree; i++){
      Poly->CoeffArray[i] = 0;
      Poly->HighPower = 0;
    }
  }
}


/*多项式输入*/
void InputPolynomial(Polynomial *Poly, int RealDegree)
{
  int degree = 0;
  ZeroPolynomial(Poly, RealDegree); /*多项式初始化为0*/
  
  Poly->HighPower = RealDegree;
  srand( (unsigned)time( NULL ) );
  while (degree <= RealDegree)
  {
    Poly->CoeffArray[degree] = rand()%2000000-1000000;
    degree++;
  }
}


/*打印多项式*/
void PrintPolynomial(Polynomial Poly)
{
  int degree = 0;
  int RealDegree = Poly.HighPower;
  long coefficient = 0;
  printf ("The Polynimial is:\n");
  while (degree <= RealDegree)
  {
    if (degree == 0)
    {
      coefficient = Poly.CoeffArray[degree];
      printf ("%ld ",coefficient);
      degree++;
    }
    else
    {
      coefficient = Poly.CoeffArray[degree];
      printf ("%ldX^%d ",coefficient, degree);
      degree++;
    }
    
  }
  printf ("\n");
}

int AddPolynomial(Polynomial Poly, mpz_t num, mpz_t sum)
{
  mpz_init(sum);
  mpz_t Xcoefficient;
  mpz_init(Xcoefficient);
  mpz_t power;
  mpz_init(power);
  mpz_t multi;
  mpz_init(multi);
  int degree = 0;
  int RealDegree = Poly.HighPower;
  long coefficient = 0;
  while (degree <= RealDegree)
  {
    if (degree == 0)
    {
      coefficient = Poly.CoeffArray[degree];
      mpz_set_si(Xcoefficient, coefficient);
      mpz_add(sum, sum, Xcoefficient);
      degree++;
    }
    else
    {
      coefficient = Poly.CoeffArray[degree];
      mpz_set_si(Xcoefficient, coefficient);
      mpz_pow_ui(power, num, degree);
      mpz_mul(multi, Xcoefficient, power);
      mpz_add(sum, sum, multi);
      //printf ("The sum of the polynomial is %ld \n",sum);
      degree++;
    }
  }
  gmp_printf("The total sum of the polynomial is %Zd\n", sum);
  return 0;
}
