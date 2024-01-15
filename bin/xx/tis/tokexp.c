#include <oraccsys.h>
#include <tok.h>

/** Extract gsig data from .tok file in a form that can easily be used
    for diffing test results */
int
main(int argc, char **argv)
{
  char *l;
  int nth;
  Gsig *gp = malloc(sizeof(Gsig));
  
  while ((l = (char*)loadoneline(stdin,NULL)))
    {
      if ('W' == *l)
	{
	  nth = 0;
	}
      else if ('g' == *l)
	{
	  char **f = tab_split(l);
	  char *p = strdup((ccp)f[2]);
	  gp = gsig_parse(p, gp, NULL);
	  if (f[3] && gp->type != 'c')
	    printf("%s:%s[%d]\t%s\n", gp->lang, f[3], nth++, f[2]);
	  free(p);
	}
    }
}
