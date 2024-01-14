#include <oraccsys.h>
#include <tok.h>

int
main(int argc, char **argv)
{
  char *lp;
  char buf[1024];
  int K = 0;

  tloc_init(0);
  while ((lp = fgets(buf,1024,stdin)))
    {
      if (isupper(*lp))
	{
	  if (tloc_line(lp))
	    {
	      if ('K' != *lp)
		{
		  if ('L' == *lp && !K++)
		    tloc_show('K');
		  if ('T' == *lp)
		    K = 0;
		  tloc_show(*lp);
		}
	    }
	  else
	    fprintf(stderr, "tokx: tloc_line failed: %s\n", lp);
	}
    }
  tloc_term();
}
