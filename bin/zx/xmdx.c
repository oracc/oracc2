#include <oraccsys.h>
#include <xmd.h>
int
main(int argc, char **argv)
{
  const char *p = argv[1];
  const char *t = argv[2];
  const char *f = argv[3];
  if (!f)
    f = "period";
  if (p && t)
    {
      xmd_init();
      Hash *hx = xmd_load(p, t);
      if (hx)
	{
	  const char *v = NULL;
	  v = hash_find(hx, (uccp)f);
	  if (v)
	    fputs(v, stdout);
	  else
	    fprintf(stderr, "xmdx: %s:%s has no value for %s\n", p, t, f);
	}
      else
	fprintf(stderr, "xmdx: %s:%s has no .xmd. Stop.\n", p, t);
    }
  else
    fprintf(stderr, "xmdx: must give PROJECT PQX-TEXT args. Stop.\n");
  xmd_term(); 
}
