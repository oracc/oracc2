#include <oraccsys.h>
#include "cx.h"

static const char *arg_project;
int merge_fields = 0;
int remap_only = 0;
int sortinfo_only = 0;
int verbose = 0;

Cx *
cx_init(void)
{
  mesg_init();
  Cx *cp = calloc(1, sizeof(Cx));
  cp->project = arg_project;
  cp->msort = memo_init(sizeof(Fsort), 1024);
  cx_roco(cp);
  cp->p = pool_init();
  cp->cfg = xpd_init(cp->project, cp->p);
  return cp;
}

int
cx_load(Cx *c, const char *cat)
{
  if (!access(cat, R_OK))
    {
      /* All catalogues must have fields in row one */
      if ((c->r = roco_load(cat, 1, "xmd-set", NULL, NULL, NULL)))
	{
	  c->r->user = c;
	  int rt = cx_roco_id_index(c);
	  if (rt == 0)
	    return 0;
	  else if (rt > 0)
	    {
	      roco_reorder(c->r, 0, rt);
	      return 0;
	    }
	  else
	    {
	      fprintf(stderr, "cx: no id_text, id_composite, or o:id in row 0\n");
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
  options(argc, argv, "mp:rsvt:");
  if (!arg_project)
    {
      fprintf(stderr, "cx: must give project with -p [PROJECT]. Stop.\n");
      return 1;
    }
  ff_verbose = verbose;
  Cx *c = cx_init();
  if (cx_load(c, argv[optind]))
    {
      fprintf(stderr, "cx: error loading catalogue. Stop.\n");
      return 1;
    }
  else
    {
      roco_field_indexes(c->r);
      cx_keydata(c);
      if (remap_only)
	return cx_remap(c);
      else
	{
	  if (merge_fields || !strcmp(xpd_option(c->cfg, "cat-merge-periods"), "yes"))
	    cx_merge_periods(c);
	  cx_sortinfo(c);
	  if (!sortinfo_only)
	    roco_write_xml(stdout, c->r);
	}
    }
  mesg_print(stderr);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'm':
      merge_fields = 1;
    case 'p':
      arg_project = arg;
      break;
    case 'r':
      remap_only = 1;
      break;
    case 's':
      sortinfo_only = 1;
      break;
    case 'v':
      ++verbose;
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
