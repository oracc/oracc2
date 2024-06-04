#include <oraccsys.h>
#include "p4url.h"

const char *docroot = "/home/oracc/www";

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
	fprintf(stderr, "%s?%s => \n", argv[1], argv[2]);
      else
	fprintf(stderr, "%s => p=%s; g=%s; id=%s\n", argv[1], p.project, p.glossary, p.id);
      return 0;
    }
  else
    {
      fprintf(stderr, "%s: p4url failed: reported: %s\n", argv[0], p.err);
      return 1;
    }
}
