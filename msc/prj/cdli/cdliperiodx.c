#include <oraccsys.h>
#include "cdli.h"

/* Read a list of periods and output the period along with the Oracc version and sort code.
 *
 * If the period is not found, emit 'not' and sort=999.
 */

static struct periodstab notp = { .name="none" , .orac="not" , .sort=999 };

int
main(int argc, char **argv)
{
  char buf[1024], *lp;
  int lnum = 0;

  int with_orig = 0;
  
  while ((lp = fgets(buf, 1024, stdin)))
    {
      ++lnum;
      int len = strlen(lp);
      if (lp[len-1] != '\n')
	fprintf(stderr, "<stdin>:%d: line too long\n", lnum);
      else
	{
	  struct periodstab *p = NULL;
	  char *ep = strstr(lp, " (");
	  if (ep)
	    {
	      *ep = '\0';
	      while (ep > lp && isspace(ep[-1]))
		--ep;
	      if (*ep)
		*ep = '\0';
	    }
	  else
	    lp[len-1] = '\0';
	  p = periods(lp, strlen(lp)); /* len can have changed */
	  if (!p)
	    p = &notp;
	  if (with_orig)
	    printf("%s\t%s\t%d\n", lp, p->orac, p->sort);
	  else
	    printf("%s\t%d\n", p->orac, p->sort);
	}
    }
}
