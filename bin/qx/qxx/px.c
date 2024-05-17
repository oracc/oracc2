#include <oraccsys.h>
#include "../../px/px.h"
#include "qx.h"

const char *px_exe = "/home/oracc/bin/px";

static const char *
px_cgi_arg(const char *arg, const char *val)
{
  char *c = malloc(strlen(arg)+strlen(val)+2);
  sprintf(c, "%s=%s", arg, CGI_decode_url(val));
  return c;
}

#if 0
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
#endif

int
px_exec(struct qxdata *qp, struct sdata *sdp)
{
  const char *vec[6];
  vec[0] = px_exe;
  vec[1] = px_cgi_arg("web","1");
  vec[2] = px_cgi_arg("project",qp->project);
  vec[3] = px_cgi_arg("glos",qp->glos?qp->glos:"");
  vec[4] = px_cgi_arg("s.d",sdp->tmp);
  vec[5] = NULL;
  if (qp->noexec)
    {
      int i;
      for (i = 0; vec[i]; ++i)
	printf("%s ", vec[i]);
      putchar('\n');
      exit(0);
    }
  else
    {
      execv(px_exe, (char*const*)vec);
      fprintf(stderr, "execv %s failed\n", px_exe);
      exit(1);
    }
}
