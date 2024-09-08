#include <oraccsys.h>
#include "nx.h"

static int nx_gcd(long n, long m);
int trace_math = 0;

/* Add n2 into n1 */
void
nx_add_frac(struct nx_num *n1, struct nx_num *n2)
{
  if (!n1 || !n2 || !n1->n || !n1->d || !n2->n || !n2->d)
    return;
  
  if (n1->d == 0)
    *n1 = *n2;
  else if (n1->d == n2->d)
    n1->n += n2->n;
  else if (n2->d > n1->d && (n2->d%n1->d) == 0)
    {
      n1->n *= n2->d/n1->d;
      n1->d = n2->d;
      n1->n += n2->n;
    }
  else if ((n1->d%n2->d) == 0)
    {
      n1->n += n2->n * (n1->d/n2->d);
    }
  else
    {
      int oden = n1->d;
      n1->d = n1->d * n2->d;
      n1->n = n1->n * (n1->d/oden);
      n1->n += n2->n * (n1->d/n2->d);
    }
}

void
nx_mul_frac(struct nx_num *n1, struct nx_num *n2)
{
  n1->n *= n2->n;
  n1->d *= n2->d;
}

nx_num
nx_div_num(nx_num divide, nx_num by)
{
  nx_num res;
  int d = by.d;
  by.d = (int)by.n;
  by.n = d;
  res.n = divide.n * by.n;
  res.d = divide.d * by.d;
  nx_simplify(&res);
  return res;
}

static int
nx_gcd(long n, long m)
{
  int remainder;
  while (n != 0)
    {
      remainder = m % n;
      m = n;
      n = remainder;
    }
  return m;
}

void
nx_simplify(struct nx_num *np)
{
  if (!np || !np->n || !np->d)
    return;

  int gcd = nx_gcd(np->n, np->d);
  if (trace_math)
    printf("nx_simplify: n = %llu/%d has gcd = %d\n", np->n, np->d, gcd);
  np->n /= gcd;
  np->d /= gcd;
  if (trace_math)
    printf("nx_simplify: result n = %llu/%d\n", np->n, np->d);
}
