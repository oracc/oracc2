#include <oraccsys.h>
#include "px.h"
#include "p4url.h"

static void
print_p(P4url *p)
{
  fprintf(stderr, "proj=%s; glos=%s; oxid=%s; pxid=%s",
	  p->project, p->glossary, p->oxid, p->pxid);
}

static void
print_args(P4url *p)
{
  int i = 0;
  while (p->args[i].option)
    {
      if (i)
	fputs("; ", stderr);
      else
	fputc(' ', stderr);
      fprintf(stderr, "%s=%s", p->args[i].option, p->args[i].value);
      ++i;
    }
}

static void
p4url_show(P4url *p, const char *u, const char *q)
{
  if (q)
    fprintf(stderr, "%s?%s => ", u, q);
  else
    fprintf(stderr, "%s => ", u);

  print_p(p);
      
  if (p->args && p->args[0].option)
    print_args(p);

  fputc('\n', stderr);
}

int
p4url_eval(const char *u, const char *q)
{
  extern int verbose;
  static P4url p;
  int res = 0;

  /* there should be a way of knowing if the syntax is OK but the
     values are wrong or if the syntax doesn't match p4url */
  if ((res = p4url(&p, u, q)))
    return 0;
  
  const char **v = p4url_vec(&p);

  if (verbose)
    {
      p4url_show(&p, u, q);
      dump_vec("px", v);
    }

  execv(px_exe, (char*const*)v);

  /* exec failed */
  fprintf(stderr, "p4url_eval: exec failed:\n\t");
  dump_vec("px", v);
  return 1;
}
