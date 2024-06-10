#include <oraccsys.h>
#include "iso.h"

static const char *hclean(unsigned char *s);

int
iso_master(Isp *ip)
{
  static struct iso isos;

  memset(&isos, '\0', sizeof(struct iso));
  
  if (ip->cache.sort)
    {
      unsigned char *h1 = NULL, *h2 = NULL;

      char buf[strlen(ip->cache.sort)+strlen("/zoom.mol0")];
      sprintf(buf, "%s/zoom.mol", ip->cache.sort);
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

      int zlev = 3;
      if (!ip->iop->h3)
	{
	  if (!ip->iop->h2)
	    zlev = 1;
	  else
	    zlev = 2;
	}
	  
      fprintf(fp, "#%d/%d\n", ip->zmax, zlev);
      struct isoz *iz;
      for (iz = ip->iop; iz; iz = iz->next)
	{
	  if (h1 && !strcmp((ccp)iz->h1, (ccp)h1))
	    iz->h1 = (ucp)"";
	  else
	    {
	      h1 = iz->h1;
	      h2 = (ucp)"";
	    }
	  if (h2 && iz->h2 && !strcmp((ccp)iz->h2, (ccp)h2))
	    iz->h2 = (ucp)"";
	  else
	    h2 = iz->h2;
	  if (!iz->h3)
	    iz->h3 = (ucp)"";
	  /*iso_h_clean(&iz);*/
	  if (iz->h3 && *iz->h3)
	    {
	      if (iz->h1 && *iz->h1)
		fprintf(fp, "%s\n", hclean(iz->h1));
	      if (iz->h2 && *iz->h2)
		fprintf(fp, "\t%s\n", hclean(iz->h2));
	      fprintf(fp, "\t\t%s\t#%d\n", hclean(iz->h3), iz->count);
	    }
	  else if (iz->h2 && *iz->h2)
	    {
	      if (iz->h1 && *iz->h1)
		fprintf(fp, "%s\n", hclean(iz->h1));
	      fprintf(fp, "\t%s\t#%d\n", hclean(iz->h2), iz->count);
	    }
	  else
	    fprintf(fp, "%s\t#%d\n", hclean(iz->h1), iz->count);
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

void
iso_master_h(Isp *ip, unsigned char *h)
{
  if (!ip->isozmem)
    {
      ip->isozmem = memo_init(sizeof(struct isoz), 64);
      ip->iop = memo_new(ip->isozmem);
      ip->iop->last = ip->iop;
    }
  else
    {
      ip->iop->last->next = memo_new(ip->isozmem);
      ip->iop->last = ip->iop->last->next;
    }
  ++ip->zmax;
  ip->iop->last->h1 = pool_copy(++h, ip->p);
  h = ip->iop->last->h1;
  while (*h && !isspace(*h))
    ++h;
  if ('\t' == *h)
    {
      *h++ = '\0';
      ip->iop->last->h2 = h;
      while (*h && !isspace(*h))
	++h;
      if ('\t' == *h)
	{
	  *h++ = '\0';
	  ip->iop->last->h3 = h;
	  while (*h && !isspace(*h))
	    ++h;
	}
    }
}

void
iso_master_n(Isp *ip, int n)
{
  ip->iop->last->count = n;
}
