#include <oraccsys.h>
#include "iso.h"

static const char *hclean(unsigned char *s);
static void iso_master_page(unsigned char *lp, struct isoz *iop);

int
iso_master(Isp *ip)
{
  static struct iso isos;

  memset(&isos, '\0', sizeof(struct iso));
  
  if (ip->cache.sort)
    {
      unsigned char *h1 = NULL, *h2 = NULL;
      int i;

      char buf[strlen(ip->cache.sort)+strlen(".mol0")];
      sprintf(buf, "%s.mol", ip->cache.sort);
      ip->cache.mol = (ccp)pool_copy((uccp)buf, ip->p);
      if (!access(ip->cache.mol, F_OK))
	{
	  if (access(ip->cache.mol, W_OK))
	    {
	      ip->err = PX_ERROR_START "master outline %s not writeable\n";
	      ip->errx = ip->cache.mol;
	      return 1;
	    }
	}

      FILE *fp;
      if (!(fp = fopen(ip->cache.mol, "w")))
	{
	  ip->err = PX_ERROR_START "unable to open master outline %s for write\n";
	  ip->errx = ip->cache.mol;
	  return 1;
	}	

      if (!(isos.zlines = px_loadfile_lines3((uccp)ip->cache.sort, &isos.zmax, NULL)))
	{
	  ip->err =(ccp) px_loadfile_error();
	  return 1;
	}
      
      fprintf(fp, "#%ld/%d\n", isos.zmax, isos.zlev);
      for (i = 0; i < isos.zmax; ++i)
	{
	  struct isoz iz = { NULL , NULL , NULL , NULL, 0 };
	  iso_master_page(isos.zlines[i], &iz);
	  if (h1 && !strcmp((ccp)iz.h1, (ccp)h1))
	    iz.h1 = (ucp)"";
	  else
	    {
	      h1 = iz.h1;
	      h2 = (ucp)"";
	    }
	  if (h2 && iz.h2 && !strcmp((ccp)iz.h2, (ccp)h2))
	    iz.h2 = (ucp)"";
	  else
	    h2 = iz.h2;
	  if (!iz.h3)
	    iz.h3 = (ucp)"";
	  /*iso_h_clean(&iz);*/
	  if (iz.h3 && *iz.h3)
	    {
	      if (iz.h1 && *iz.h1)
		fprintf(fp, "%s\n", hclean(iz.h1));
	      if (iz.h2 && *iz.h2)
		fprintf(fp, "\t%s\n", hclean(iz.h2));
	      fprintf(fp, "\t\t%s\t#%d\n", hclean(iz.h3), iz.count);
	    }
	  else if (iz.h2 && *iz.h2)
	    {
	      if (iz.h1 && *iz.h1)
		fprintf(fp, "%s\n", hclean(iz.h1));
	      fprintf(fp, "\t%s\t#%d\n", hclean(iz.h2), iz.count);
	    }
	  else
	    fprintf(fp, "%s\t#%d\n", hclean(iz.h1), iz.count);
	}
      fclose(fp);
      (void)hclean(NULL);
    }
  return 0;
}

static const char *
hclean(unsigned char *s)
{
  static char *p = NULL;
  if (!s && p)
    {
      free(p);
      return NULL;
    }

  p = realloc(p, strlen((ccp)s)+1);
  strcpy(p, (ccp)s);
  char *t = p;
  while (*t)
    if ('_' == *t)
      *t++ = ' ';
    else
      ++t;

  return p;
}

static void
iso_master_page(unsigned char *lp, struct isoz *iop)
{
  iop->h1 = ++lp;
  while (*lp && !isspace(*lp))
    ++lp;
  if ('\t' == *lp)
    {
      *lp++ = '\0';
      iop->h2 = lp;
      while (*lp && !isspace(*lp))
	++lp;
      if ('\t' == *lp)
	{
	  *lp++ = '\0';
	  iop->h3 = lp;
	  while (*lp && !isspace(*lp))
	    ++lp;
	}
    }
  if (' ' == *lp)
    {
      *lp++ = '\0';
      iop->items = lp;
      int i = 1;
      while (*lp)
	{
	  if (' ' == *lp)
	    ++i;
	  ++lp;
	}
      iop->count = i;
    }
}
