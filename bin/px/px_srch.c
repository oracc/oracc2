#include <oraccsys.h>
#include "px.h"

#define SRCHTMP "s.XXXXXX"

static int
px_run_srch(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/pxsrch.sh");
  list_add(args, " ");
  list_add(args, ip->proj);
  list_add(args, " ");
  list_add(args, ip->srchdata.idx ? ip->srchdata.idx : "#any");
  list_add(args, " ");
  list_add(args, ip->srchdata.tmp;
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

int
px_srch(Isp *ip)
{
  char *s = CGI_decode_url(ip->srch);
  struct isp_srchdata *sdp = &ip->srchdata;
  
  char *d = (char*)pool_alloc(strlen(ip->cache.sub)+strlen(SRCHTMP)+2, ip->p);
  sprintf(d, "%s/%s", ip->cache.sub, SRCHTMP);

  if (!(sdp->tmp = mkdtemp(d)))
    {
      ip->err = "failed to make search tmpdir";
    }
  else
    {
      FILE *fp = NULL;

      fprintf(stderr, "p3: saving search '%s' to %s/search.bar\n", s, d);
      sdp->bar = (ccp)pool_alloc(strlen(d) + strlen("/search.bar") + 1, ip->p);
      sprintf((char*)sdp->bar, "%s/search.bar", d);
  
      if ((fp = fopen(sdp->bar, "w")))
	{
	  fputs(s,fp);
	  fclose(fp);
	}
      else
	{
	  fprintf(stderr, "p3: failed to open %s to write srch bar command\n", sdp->bar);
	  ip->err = "failed to write search.bar";
	}

      if (fp)
	{
	  sdp->txt = (ccp)pool_alloc(strlen(d) + strlen("/search.txt") + 1, ip->p);  
	  sprintf((char*)sdp->txt, "%s/search.txt", d);
	  if ((fp = fopen(sdp->txt, "w")))
	    {
	      while (isspace(*s))
		++s;
	      if (*s == '!')
		{
		  const char *tmp = ++s;
		  while (*s && !isspace(*s))
		    ++s;
		  if (*s)
		    {
		      int slen = (s - tmp);
		      char *idx = (char *)pool_alloc(slen+1, ip->p);
		      strncpy(idx, tmp, slen);
		      idx[slen] = '\0';
		      sdp->idx = idx;
		      while (isspace(*s))
			++s;
		    }
		}
	      fputs(s, fp);
	      fclose(fp);
	    }
	  else
	    {
	      fprintf(stderr, "p3: failed to open %s to write srch command\n", sdp->txt);
	      ip->err = "failed to write search.txt";
	    }
	}
    }
  free(s);

  if (!ip->err)
    return px_run_srch(ip);
  else
    return 1;
}
