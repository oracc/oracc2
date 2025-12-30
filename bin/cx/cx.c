#include <oraccsys.h>
#include <roco.h>
#include "cx.h"

static const char *arg_project;

Cx *
cx_init(void)
{
  Cx *cp = calloc(1, sizeof(Cx));
  cp->project = arg_project;
  cx_roco(cp);
  return cp;
}

int
cx_load(Cx *c, const char *cat)
{
  if (!access(cat, R_OK))
    {
      /* All catalogues must have fields in row one */
      if ((c->r = roco_load(cat, 1, "xmd-set", "xmd", NULL, NULL)))
	{
	  uccp f1 = c->r->rows[0][0];
	  if (!strcmp((ccp)f1, "id_text")
	      || !strcmp((ccp)f1, "id_composite")
	      || !strcmp((ccp)f1, "o:id"))
	    {
	      return 0;
	    }
	  else
	    {
	      fprintf(stderr, "cx: first field name must be id_text, id_composite, or o:id");
	      return 1;
	    }
	}
      else
	return 1;
    }
  else
    {
      fprintf(stderr, "cx: unable to read catalogue `%s'\n", cat);
      return 1;
    }
}

int
main(int argc, char * const *argv)
{
  options(argc, argv, "p:");
  Cx *c = cx_init();
  if (cx_load(c, argv[1]))
    {
      fprintf(stderr, "cx: error loading catalogue. Stop.\n");
      exit(1);
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'p':
      arg_project = arg;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  ;
}
