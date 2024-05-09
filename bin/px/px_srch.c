#include <oraccsys.h>
#include "px.h"

static int
px_run_srch(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/pxsrch.sh");
  list_add(args, " ");
  list_add(args, (void*)ip->project);
  list_add(args, " ");
  list_add(args, (void*)(ip->srchdata.idx ? ip->srchdata.idx : "#any"));
  list_add(args, " ");
  list_add(args, (void*)ip->srchdata.tmp);
  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "px_run_srch: %s\n", syscmd);

  if (system((ccp)syscmd))
    {
      ip->err = PX_ERROR_START "px_run_srch failed system call:\n\t%s\n";
      ip->errx = (ccp)syscmd;
      return 1;
    }
  
  return 0;
}

