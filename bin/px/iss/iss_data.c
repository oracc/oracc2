#include <oraccsys.h>
#include "../pxdefs.h"
#include "iss.h"

static int iss_i_dump(Dbi_index *dp, Isp *ip, struct page *p, int i, int item, int page, int pitem, int zoom, int zpage, int zitem);
static int iss_p_dump(FILE *pfp,Isp *ip, struct page *p, int zoom, int page, int firsth, int firsti, int lasti);

int
iss_data(Isp *ip, struct page *p)
{
  /*char *pframe[51], *zframe[27];*/
  int zoom = 0;  /* current zoom-N */
  int z0th = 1;  /* current page in zoom 0 */
  int zpth = 1;  /* current page in current zoom */
  int nlist = 0; /* current text index in total list */
  int nz0th = 0; /* current text index in z0th */
  int nzpth = 0; /* current text index in zpth */
  int z0firsth = -1; /* p->p[index] of first header on current z0 page */
  int znfirsth = -1; /* p->p[index] of current zoom header */
  int pfirst = -1;/* p->p[index] of first text in current z0 page */
  int zfirst = -1;/* p->p[index] first text in current zN page */

  Dbi_index *idp = dbi_create("itm", ".", 1024, 1, DBI_BALK);
  FILE *pfp = fopen("pag.tsv", "w");
  
  Unsigned32 i;
  for (i = 0; i < p->used; ++i)
    {
      if ('+' == p->p[i][0])
	continue;
      else if ('#' == p->p[i][0])
	{
	  /* Print the tail end of the last zoom only if there are leftover texts */
	  if (i && zfirst != -1)
	    {
	      int iprev = i - 1;
	      while (iprev > 0 && (p->p[iprev][0] == '#' || p->p[iprev][0] == '+'))
		--iprev;	  
	      iss_p_dump(pfp, ip, p, zoom, zpth, i, zfirst, iprev);
	    }

	  znfirsth= i;
	  /* This test means that the header is the first thing on a new page */
	  if (pfirst == -1)
	    z0firsth = i;
	  ++zoom;
	  zpth = 1;
	  nzpth = 0;
	  zfirst = -1;
	}
      else
	{
	  /* increment index counters */
	  ++nlist;
	  ++nz0th;
	  ++nzpth;

	  /* unconditionally output item data */
	  iss_i_dump(idp, ip, p, i, nlist, z0th, nz0th, zoom, zpth, nzpth);
	  
	  /* dump z0 page if !nz0th%25; reset z0 page */
	  if (pfirst != -1 && nz0th%25==0)
	    {
	      iss_p_dump(pfp, ip, p, 0, z0th, z0firsth, pfirst, i);
	      pfirst = -1;
	      nz0th = 0;
	      ++z0th;
	    }
	  
	  /* dump zN page if !nzpth%25; reset zN page */
	  if (zfirst != -1 && nzpth%25==0)
	    {
	      iss_p_dump(pfp, ip, p, zoom, zpth, znfirsth, zfirst, i);
	      zfirst = -1;
	      nzpth = 0;
	      ++zpth;
	    }

	  if (nz0th == 1)
	    pfirst = i;
	  if (nzpth == 1)
	    zfirst = i;
	}
    }

  /* i = p->used and we output up to and including i so it needs decrementing here */
  iss_p_dump(pfp, ip, p, 0, z0th, z0firsth, zfirst, i-1);
  iss_p_dump(pfp, ip, p, zoom, zpth, znfirsth, zfirst, i-1);

  dbi_flush(idp);
  fclose(pfp);
  
  return 0;
}

static int
iss_i_dump(Dbi_index *dp, Isp *ip, struct page *p, int i, int item, int page, int pitem, int zoom, int zpage, int zitem)
{
  char *prev = "#", *next = "#";
  char *colon = NULL;

  /* p->p[0] can be the stopper here because it is always a header */
  int iprev = i - 1;
  while (iprev > 0 && (p->p[iprev][0] == '#' || p->p[iprev][0] == '+'))
    --iprev;
  if (iprev > 0)
    {
      prev = p->p[iprev];
      colon = strchr(prev, ':');
      prev = ++colon;
    }

  int inext = i + 1;
  while (inext < p->used && (p->p[inext][0] == '#' || p->p[inext][0] == '+'))
    ++inext;

  if (inext < p->used)
    {
      next = p->p[inext];
      colon = strchr(next, ':');
      next = ++colon;
    }
  
  char projbuf[strlen(p->p[i])];
  char *proj = projbuf, *pqx = NULL;
  strcpy(proj, p->p[i]);
  colon = strchr(proj, ':');
  if (colon)
    {
      *colon++ = '\0';
      pqx = colon;
    }
  else
    {
      pqx = proj;
      proj = (char*)ip->project;
    }  

  int n = snprintf(NULL, 0, "%d\t%d\t%d\t%d\t%d\t%d%s\t%s\t%s", item, page, pitem, zoom, zpage, zitem, prev, next, proj);
  char data[n+1];
  sprintf(data, "%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s", item, page, pitem, zoom, zpage, zitem, prev, next, proj);

  dbi_add(dp, (ucp)pqx, data, strlen(data)+1);

  return 0;
}

static int
iss_p_dump(FILE *pfp, Isp *ip, struct page *p, int zoom, int page, int firsth, int firsti, int lasti)
{
  int n = snprintf(NULL, 0, "z%dp%d", zoom, page);
  char key[n+1];
  sprintf(key, "z%dp%d", zoom, page);
  int j;
  fprintf(pfp, "%s\t%s %s", key, p->p[firsth], p->p[firsti]);
  for (j = firsti+1; j <= lasti; ++j)
    {
      if ('+' == p->p[j][0])
	continue;
      else if ('#' == p->p[j][0])
	fprintf(pfp, " %s", p->p[j]);
      else
	fprintf(pfp, " %s", p->p[j]);
    }
  fputc('\n', pfp);
  fflush(pfp);
  return 0;
}

