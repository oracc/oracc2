#include <oraccsys.h>
#include "../../px/px.h"
#include "../../px/pxdefs.h"
#include "qx.h"

/* search engine interface for when it's called from wx */
extern enum result_granularity res_gran;
struct qxdata qd;
int p4status = 0;
struct sdata sdata;

static const char *
cache_sub(struct qxdata *qp)
{
  const char *cache = getenv("ORACC_P4_CACHE");
  if (!cache)
    cache = "";
  int len = strlen(cache)+strlen(oracc_builds())+strlen("/www/p4.d/0")+strlen(qp->project);
#if 0
  if (qp->glos)
    len += strlen(qp->glos)+strlen("//");
#endif
  char *p = (char*)pool_alloc(len, qp->p);
  if (cache && *cache)
    sprintf(p, "%s/%s", cache, qp->project);
  else
    sprintf(p, "%s/www/p4.d/%s", oracc_builds(), qp->project);
#if 0
  if (qp->glos)
    {
      strcat(p, "/");
      strcat(p, qp->glos);
    }
#endif
  return p;
}	  

int
qx_tmpdir(struct qxdata *qp)
{
  char *s = CGI_decode_url(qp->srch);
  struct sdata *sdp = &sdata;

  sdp->sub = cache_sub(qp);
  
  char *d = (char*)pool_alloc(strlen(sdp->sub)+strlen(SRCHTMP)+2, sdp->p);

  if (qp->argtmp)
    {
      struct stat sb;
      if (stat(qp->argtmp, &sb) != 0 || !S_ISDIR(sb.st_mode))
	{
	  sdp->err = (ccp)pool_copy((ucp)qx_err("argument tmpdir %s does not exist or is not a directory",
						qp->argtmp), sdp->p);
	  return 1;

	}
      else
	{
	  sdp->tmp = qp->argtmp;
	  sprintf(d, "%s", sdp->tmp);
	}
    }
  else
    {
      sprintf(d, "%s/%s", sdp->sub, SRCHTMP);
      if (!(sdp->tmp = mkdtemp(d)))
	{
	  sdp->err = (ccp)pool_copy((ucp)qx_err("failed to make search tmpdir %s: %s",
						d, strerror(errno)), sdp->p);
	  return 1;
	}
    }

  FILE *fp = NULL;

  char cbdidx[1024];
  if (qp->glos)
    sprintf(cbdidx, "!cbd/%s ", qp->glos);
  else
    *cbdidx = '\0';
	   
  if (*cbdidx)
    fprintf(stderr, "qx: saving search '%s%s' to %s/search.bar\n", cbdidx, s, d);
  else
    fprintf(stderr, "qx: saving search '%s' to %s/search.bar\n", s, d);
  sdp->bar = (ccp)pool_alloc(strlen(d) + strlen("/search.bar") + 1, sdp->p);
  sprintf((char*)sdp->bar, "%s/search.bar", d);
  
  if ((fp = fopen(sdp->bar, "w")))
    {
      fprintf(fp, "%s%s", cbdidx, s);
      fclose(fp);
    }
  else
    {
      fprintf(stderr, "qx: failed to open %s to write srch bar command\n", sdp->bar);
      sdp->err = (ccp)pool_copy((ucp)qx_err("failed to write search.bar"), sdp->p);
    }
  free(s);

  return sdp->err ? 1 : 0;
}

static const char *
qx_idx_check(struct qxdata *qp, struct sdata *sdp, const char *bar)
{
  unsigned char u203c[] = { 0xE2 , 0x80 , 0xBC };
  const char *idx = NULL;
  Pool *p = sdata.p;
  if ('!' == *bar)
    idx = bar+1;
  else if (bar[0] == u203c[0] && bar[1] == u203c[1] && bar[2] == u203c[2])
    idx = bar+3;
  if (idx)
    {
      char *sp = (char*)idx;
      while (*sp && !isspace(*sp))
	++sp;
      if (*sp)
	{
	  int len = sp - idx;
	  if (len > 20)
	    {
	      sdata.err = (ccp)pool_copy((ucp)qx_err("impossibly long index size (max is 20, e.g., cbd/akk-x-stdbab-939)"),sdp->p);
	      goto wrapup;
	    }
	  char buf[len+1];
	  strncpy(buf, idx, len);
	  buf[len] = '\0';
	  sdata.idx = (ccp)pool_copy((ucp)buf, p);
	  sdata.dir = (ccp)pool_alloc(strlen(sdata.pub)+strlen(sdata.idx)+2, p);
	  sprintf((char*)sdata.dir, "%s/%s", sdata.pub, sdata.idx);
	  /* test for dir */
	  struct stat sb;
	  if (stat(sdata.dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
	    {
	      sdata.err = (ccp)pool_copy((ucp)qx_err("unknown index %s", sdata.dir), p);
	      goto wrapup;
	    }

	  if (len > 3 && !strncmp(idx, "cbd/", 4))
	    sdata.nam = "cbd";
	  else
	    sdata.nam = sdata.idx;

	  sdata.dbi = (ccp)pool_alloc(strlen(sdata.dir)+strlen(sdata.nam)+strlen("/.dbi0"), p);
	  sprintf((char*)sdata.dbi, "%s/%s.dbi", sdata.dir, sdata.nam);
	  if (access(sdata.dbi, R_OK))
	    {
	      sdata.err = (ccp)pool_copy((ucp)qx_err("bad index: %s absent or unreadable", sdata.dbi), p);
	      goto wrapup;
	    }
	}
      else
	{
	  sdata.err = (ccp)pool_copy((ucp)qx_err("search.bar contains an index but no search terms"),p);
	  goto wrapup;
	}
    }
  else
    any_index = qp->any_index = 1;
 wrapup:
  return sdata.err;
}

int
qx(struct qxdata *qp)
{
  Pool *p = qp->p;
  struct stat sb;

  if (!qp->srch || '\0' == *qp->srch)
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("empty search"), p);
      return 1;
    }
  
  sdata.prj = (ccp)pool_alloc(strlen(oracc_builds())+strlen(qp->project)+2, p);
  sprintf((char*)sdata.prj, "%s/%s", oracc_builds(), qp->project);
  if (stat(sdata.prj, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("unknown project %s", sdata.prj), p);
      goto wrapup;
    }

  sdata.pub = (ccp)pool_alloc(strlen(oracc_builds())+strlen(sdata.prj)+strlen("/02pub0"), p);
  sprintf((char*)sdata.pub, "%s/02pub", sdata.prj); 
  if (stat(sdata.pub, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("project %s has no 02pub directory", sdata.prj), p);
      goto wrapup;
    }

  if (qx_tmpdir(qp))
    goto wrapup;
  
  if (stat(sdata.tmp, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("unknown or inaccessible tmpdir %s", sdata.tmp), p);
      goto wrapup;
    } 

  sdata.bar = (ccp)pool_alloc(strlen(sdata.tmp)+strlen("/search.bar0"), p);
  sprintf((char*)sdata.bar, "%s/search.bar", sdata.tmp);
  if (access(sdata.bar, R_OK))
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("unknown or inaccessible search.bar %s", sdata.bar), p);
      goto wrapup;
    }

  char *bar = (char*)slurp("qx-px", sdata.bar, NULL);
  if (!bar)
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("failed to read search.bar %s", sdata.bar), p);
      goto wrapup;
    }
  else
    {
      qp->bar = (ccp)pool_copy((ucp)bar, qp->p);
      if (bar[strlen(bar)-1] == '\n')
	bar[strlen(bar)-1] = '\0';
    }

  if ((sdata.err = qx_idx_check(qp, &sdata, bar)))
    goto wrapup;

  sdata.lst = (ccp)pool_alloc(strlen(sdata.tmp)+strlen("/list0"), p);
  sprintf((char*)sdata.lst, "%s/list", sdata.tmp);
  if (!(qp->listfp = fopen(sdata.lst, "w")))
    {
      sdata.err = (ccp)pool_copy((ucp)qx_err("failed to open list file %s\n", sdata.lst), p);
      return 1;
    }
  
  if (se_run_qx(qp, &sdata))
    {
      if (!sdata.err)
	sdata.err = (ccp)pool_copy((ucp)qx_err("execution of search returned error\n"), p);
      return 1;
    }
  
  if (!sdata.err)
    {
      sdata.count = (int)result_count();
    }

 wrapup:
  mesg_term();
  /*pool_term(sdata.p);*//*can't do this here*/

  if (sdata.err)
    return 1;

  if (px_exec(qp, &sdata))
    {
      if (!qp->noexec)
	{
	  sdata.err = "qx: error: failed to exec px\n";
	  return 1;
	}
    }
  return 0;
}

int
qx_count_file(struct sdata *sdp)
{
  const char *countfn = (ccp)pool_alloc(strlen(sdp->tmp)+strlen("/count00"), sdp->p);
  sprintf((char*)countfn, "%s/count", sdp->tmp);
  FILE *countfp = fopen(countfn, "w");
  if (countfp)
    {
#if 1
      fprintf(countfp, "%d", sdp->count);
#else
      const char *gran = "trfwg";
      fprintf(countfp, "%d%c", sdp->count, gran[res_gran]);
#endif
      fclose(countfp);
      sprintf((char*)countfn, "%s/qx.new", sdp->tmp);
      countfp = fopen(countfn, "w");
      fclose(countfp);
      return 0;
    }
  else
    {
      sdp->err = (ccp)pool_copy((ucp)qx_err("failed to open count file %s: %s\n",
					     countfn, strerror(errno)), sdp->p);
      return 1;
    }
}
