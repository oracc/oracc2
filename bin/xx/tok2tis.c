#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>

#include <tis.h>

struct tokloc
{
  const char *W;
};

struct tokloc tl;
Pool *p;

int
main(int argc, char **argv)
{
  size_t nbytes;
  unsigned char *lp;
  p = pool_init();
  Tisp t = tis_init();
  while ((lp = loadoneline(stdin,&nbytes)))
    {
      if (isupper(*lp))
	{
	  if ('W' == *lp)
	    {
	      unsigned char *wid = (ucp)strchr((ccp)lp,'\t');
	      if (wid)
		tl.W = (ccp)pool_copy(++wid,p);
	    }
	}
      else
	{
	  if (tl.W)
	    {
	      tis_add(t, (ccp)pool_copy(lp,p), tl.W);
	    }
	  else
	    {
	      fprintf(stderr, "no W set\n");
	      exit(1);
	    }
	}
    }
  tis_sort(t);
  tis_dump(stdout, t);
}
