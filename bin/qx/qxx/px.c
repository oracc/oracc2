#include <oraccsys.h>
#include "../../px/px.h"
#include "../../px/pxdefs.h"

struct sdata
{
  const char *prj;
  const char *pub;
  const char *tmp;
  const char *bar;
  const char *idx;
  const char *dir; /* idx dir */
  const char *nam; /* idx nam */
} sdata;

int p4status = 0;

static const char *
qx_idx_check(struct sdata *sdp, const char *bar)
{
  unsigned char u203c[] = { 0xE2 , 0x80 , 0xBC };
  const char *idx = NULL;
  if ('!' == *bar)
    idx = bar+1;
  else if (bar[0] == u203c[0] && bar[1] == u203c[1] && bar[2] == u203c[2])
    idx = bar+3;
  if (idx)
    {
      char *sp = idx;
      while (*sp && !isspace(*sp))
	++s;
      if (*sp)
	{
	  int len = sp - idx;
	  if (len > 20)
	    {
	      return "impossibly long index size (max is 20, e.g., cbd/akk-x-stdbab-939)";
	    }
	  char buf[len+1];
	  strncpy(buf, idx, len);
	  buf[len] = '\0';
	  sdata.idx = pool_copy(buf, p);
	  sdata.dir = pool_alloc(strlen(sdata.pub)+strlen(sdata.idx, p)+2);
	  sprintf(sdata.dir, "%s/%s", sdata.pub, sdata.idx);
	  /* test for dir */
	  if (stat(sdata.dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
	    {
	      err = (ccp)pool_copy((ucp)qx_err("unknown index %s", sdata.dir), p);
	      goto wrapup;
	    }

	  if (len > 3 && !strncmp(idx, "cbd/", 4))
	    sdata.nam = "cbd";
	  else
	    sdata.nam = idx;

	  sdata.dbi = pool_alloc(strlen(sdata.dir)+strlen(sdata.nam)+strlen("/.dbi0"));
	  sprintf(sdata.dbi, "%s/%s/%s.dbi", sdata.dir, sdata.nam);
	  if (access(sdata.dbi, R_OK))
	    {
	      err = (ccp)pool_copy((ucp)qx_err("bad index: %s absent or unreadable", sdata.dbi), p);
	      goto wrapup;
	    }
	  
	}
      else
	{
	  return "search.bar seems to contain an index but no search terms";
	}
    }
}

void
px(const char *project, const char *tmpdir)
{
  Pool *p = pool_init();
  const char *err = NULL;
  struct stat sb;

  mesg_init();

  sdata.prj = (ccp)pool_alloc(strlen(oracc_builds())+strlen(project)+2, p);
  sprintf((char*)sdata.prj, "%s/%s", oracc_builds(), project);
  if (stat(sdata.prj, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      err = (ccp)pool_copy((ucp)qx_err("unknown project %s", sdata.prj), p);
      goto wrapup;
    }

  sdata.pub = (ccp)pool_alloc(strlen(oracc_builds())+strlen(project)+strlen("/02pub0", p));
  sprintf((char*)sdata.pub, "%s/02pub", sdata.prj); 
  if (stat(sdata.pub, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      err = (ccp)pool_copy((ucp)qx_err("project %s has no 02pub directory", sdata.prj), p);
      goto wrapup;
    }

  sdata.tmp = tmpdir;
  if (stat(sdata.tmp, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      err = (ccp)pool_copy((ucp)qx_err("unknown or inaccessible tmpdir %s", sdata.tmp), p);
      goto wrapup;
    } 

  sdata.bar = (ccp)pool_alloc(strlen(tmpdir)+strlen("/search.bar0"), p);
  sprintf((char*)sdata.bar, "%s/search.bar", tmpdir);
  if (access(sdata.bar, R_OK))
    {
      err = (ccp)pool_copy((ucp)qx_err("unknown or inaccessible search.bar %s", sdata.bar), p);
      goto wrapup;
    }

  char *bar = (char*)slurp("qx-px", sdata.bar, NULL);
  if (!bar)
    {
      err = (ccp)pool_copy((ucp)qx_err("failed to read search.bar %s", sdata.bar), p);
      goto wrapup;
    }

  if ((err = qx_idx_check(&sdata, bar)))
    goto wrapup;    

 wrapup:
  if (err)
    {
      fputs(err, stderr);
      p4status = 1;
    }

  mesg_term();
  pool_term(p);
  return;
}
