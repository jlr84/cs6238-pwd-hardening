#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "lagrange.h"

int Lagrange(mpf_t result,int TotalDataCount,mpf_t* DataX,mpf_t* DataY)
{
 int i,j;
 mpf_t fz;
 mpf_t fm;
 mpf_t XNum;
 mpf_t Xfz;
 mpf_t Xfm;
 mpf_t Yfz;
 mpf_t Yfm;
 mpf_init_set_d(fz,0);
 mpf_init_set_d(fm,0);
 mpf_init_set_d(XNum,0);
 mpf_init_set_d(Xfz,0);
 mpf_init_set_d(Xfm,0);
 mpf_init_set_d(Yfz,0);
 mpf_init_set_d(Yfm,0);
 for(i=0;i<TotalDataCount;i++)
 {
  mpf_set_d(fz, 1);
  mpf_set_d(fm, 1);
  for(j=0;j<TotalDataCount;j++)
   if(j!=i)
    {
     mpf_sub(Xfz, XNum, DataX[j]);
     mpf_mul(fz, fz, Xfz);
     mpf_sub(Xfm, DataX[i], DataX[j]);
     mpf_mul(fm, fm, Xfm);
    }
  mpf_mul(Yfz, DataY[i], fz);
  mpf_div(Yfm, Yfz, fm);
  mpf_add(result, result, Yfm);
  }
 return 0;
}

long Xround (mpf_t result)
{
  long fun;
  int comp;
  mpf_t operand;
  mpf_init_set_d(operand, 0.5);
  mpf_t comparison;
  mpf_init_set_d(comparison, 0);
  comp = mpf_cmp(result, comparison);

  if (comp >= 0)
  {
    mpf_add(result, result, operand);
  }
  else
  {
    mpf_sub(result, result, operand);
  }
  
  fun = mpf_get_si(result);

  mpf_clear(operand);
  mpf_clear(comparison);

  return fun;
}
