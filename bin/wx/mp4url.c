#include <oraccsys.h>
#include "p4url.h"

const char *docroot = "/home/oracc/www";

static void
print_p(P4url *p)
{
  fprintf(stderr, "p=%s; g=%s; id=%s", p->project, p->glossary, p->id);
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

int
main(int argc, char **argv)
{
  static P4url p;
  int res = 0;
  if (argv[1] && argv[2])
    res = p4url(&p, argv[1], argv[2]);
  else if (argv[1])
    res = p4url(&p, argv[1], NULL);
  else
    {
      fprintf(stderr, "%s: usage: p4url SCRIPT_URL [QUERY_STRING]\n", argv[0]);
      return 1;
    }

  if (!res)
    {
      if (argv[2])
	fprintf(stderr, "%s?%s => ", argv[1], argv[2]);
      else
	fprintf(stderr, "%s => ", argv[1]);

      print_p(&p);

      if (p.args[0].option)
	print_args(&p);

      fputc('\n', stderr);

      return 0;
    }
  else
    {
      fprintf(stderr, "%s: p4url failed: reported: %s\n", argv[0], p.err);
      return 1;
    }
}
