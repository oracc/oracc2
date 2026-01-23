#include <oraccsys.h>
#include <pool.h>

/* Exercise the indexed pool library routines */

int
main(int argc, char * const *argv)
{
  char buf[1024], *b;
  Pool *p = ipool_init();
  FILE *fp;
  if (argv[1])
    fp = fopen(argv[1], "r");
  else
    fp = stdin;
  while ((b = fgets(buf, 1023, fp)))
    {
      if (buf[strlen(buf)-1] == '\n')
	{
	  buf[strlen(buf)-1] = '\0';
	  size_t i = ipool_copy((uccp)b, p);
	  fprintf(stderr, "%s\t%ld\n", b, i);
	}
      else
	fprintf(stderr, "poolx: line too long\n");
    }
  fprintf(stderr, "poolx: pool for %ld strings follows\n", p->nstr);
  ipool_write(p, stdout);
}
