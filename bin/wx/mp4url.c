#include <oraccsys.h>
#include "px.h"
#include "p4url.h"

const char *docroot = "/home/oracc/www";
const char *project = NULL;
const char *query_string = NULL;

int verbose;

int
main(int argc, char **argv)
{
  const char *u, *q;
  
  if (argv[1] && argv[2])
    {
      u = argv[1];
      q = argv[2];
    }
  else if (argv[1])
    u = argv[1];
  else
    {
      fprintf(stderr, "%s: usage: p4url SCRIPT_URL [QUERY_STRING]\n", argv[0]);
      return 1;
    }

  p4url_eval(u, q);
}
