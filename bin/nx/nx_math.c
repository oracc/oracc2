#include <oraccsys.h>
#include "nx.h"

/* Add n2 into n1 */
void
nx_add_frac(struct nx_num *n1, struct nx_num *n2)
{
  if (n1->d == n2->d)
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
