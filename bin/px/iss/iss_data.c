#include <oraccsys.h>
#include <tsv.h>
#include "../pxdefs.h"
#include "iss.h"
#include "../iso/iso.h"

static int iss_i_dump(Dbi_index *dp, Isp *ip, struct page *p, int i, int item, int page, int pitem, int zoom, int zpage, int zitem);
static int iss_p_dump(FILE *pfp, Tsv *tp, Isp *ip, struct page *p, int zoom, int page, int firsth, int firsti, int lasti);
static void iss_max(FILE *mfp, Dbi_index *mdp, int zoom, int max);

int
iss_data(Isp *ip, struct page *p)
{
  return iss_data_sub(ip, p, ip->cache.sort, ip->cache.tsv, ip->cache.max, ip->cache.mol);
}

int
iss_data_sub(Isp *ip, struct page *p, const char *sort, const char *tsv, const char *max, const char *mol)
{
  /*char *pframe[51], *zframe[27];*/
  int zoom = 0;  /* current zoom-N */
  int z0th = 1;  /* current page in zoom 0 */
  int zpth = 1;  /* current page in current zoom */
  int nlist = 0; /* current text index in total list */
  int nzoom = 0; /* current text index in zoom; used for zoom-max */
  int nz0th = 0; /* current text index in z0th */
  int nzpth = 0; /* current text index in zpth */
  int z0firsth = -1; /* p->p[index] of first header on current z0 page */
  int znfirsth = -1; /* p->p[index] of current zoom header */
  int pfirst = -1;/* p->p[index] of first text in current z0 page; -1 is a signal value*/
  int zfirst = -1;/* p->p[index] first text in current zN page */
  Tsv *tp;
  
  tp = tsv_init(tsv, NULL, NULL);
  FILE *pfp = fopen(tsv, "w");
  if (!pfp)
    {
      ip->err = px_err("failed to open %s\n", tsv);
      return 1;
    }
  FILE *mfp = fopen(max, "w");
  if (!mfp)
    {
      ip->err = px_err("failed to open %s\n", max);
      return 1;
    }
  
  Dbi_index *mdp = dbi_create("max", sort, 1024, 1, DBI_BALK);
  Dbi_index *idp = dbi_create("itm", sort, 1024, 1, DBI_BALK);
  
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
	      iss_p_dump(pfp, tp, ip, p, zoom, zpth, i, zfirst, iprev);
	    }

	  if (zoom)
	    iss_max(mfp, mdp, zoom, nzoom);

	  if (ip->iop)
	    iso_master_n(ip, nzoom);
	  iso_master_h(ip, (ucp)p->p[i]);

	  znfirsth= i;
	  /* This test means that the header is the first thing on a new page */
	  if (pfirst == -1)
	    {
	      z0firsth = i;
	      pfirst = i+1;
	    }
	  ++zoom;
	  zpth = 1;
	  nzoom = nzpth = 0;
	  zfirst = -1;
	}
      else
	{
	  /* increment index counters */
	  ++nlist;
	  ++nzoom;
	  ++nz0th;
	  ++nzpth;

	  if (pfirst == -1)
	    pfirst = i;
	  
	  /* unconditionally output item data */
	  iss_i_dump(idp, ip, p, i, nlist, z0th, nz0th, zoom, zpth, nzpth);
	  
	  /* dump z0 page if !nz0th%25; reset z0 page */
	  if (pfirst != -1 && (nz0th%25)==0)
	    {
	      iss_p_dump(pfp, tp, ip, p, 0, z0th, z0firsth, pfirst, i);
	      pfirst = -1;
	      nz0th = 0;
	      ++z0th;
	    }
	  
	  /* dump zN page if !nzpth%25; reset zN page */
	  if (zfirst != -1 && nzpth%25==0)
	    {
	      iss_p_dump(pfp, tp, ip, p, zoom, zpth, znfirsth, zfirst, i);
	      zfirst = -1;
	      nzpth = 0;
	      ++zpth;
	    }

	  if (nzpth == 1)
	    zfirst = i;
	}
    }

  /* i = p->used and we output up to and including i so it needs decrementing here */
  iss_p_dump(pfp, tp, ip, p, 0, z0th, z0firsth, pfirst, i-1);
  iss_p_dump(pfp, tp, ip, p, zoom, zpth, znfirsth, zfirst, i-1);

  /* set zoom-0 to the total number of items in the list */
  iss_max(mfp, mdp, 0, nlist);

  /* set z to the max zoom index */
  iss_max(mfp, mdp, -1, zoom);
  
  /* also set tmax if we are making a text list */
  if (p->texts)
    {
      char tmax[16];
      sprintf(tmax, "%d", nlist);
      dbi_add(idp, (ucp)"tmax", tmax, strlen(tmax)+1);
    }
  
  /* add the last zoom count to the master outline data */
  iso_master_n(ip, nzoom);
  iss_max(mfp, mdp, zoom, nzoom);

  dbi_flush(idp);
  dbi_flush(mdp);
  fclose(pfp);
  fclose(mfp);
  tsv_term(tp);

  if (iso_master(ip, mol))
    return 1;

  return 0;
}

static void
iss_max(FILE *mfp, Dbi_index*mdp, int zoom, int max)
{
  char mbuf[16];
  char zbuf[16];
  sprintf(mbuf, "%d", max);
  if (zoom == -1)
    strcpy(zbuf, "z");
  else
    sprintf(zbuf, "%d", zoom);
  dbi_add(mdp, (ucp)zbuf, mbuf, strlen(mbuf)+1);
  fprintf(mfp, "%s\t%s\n", zbuf, mbuf);
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
      if ((colon = strchr(prev, ':')))
	prev = ++colon;
    }

  int inext = i + 1;
  while (inext < p->used && (p->p[inext][0] == '#' || p->p[inext][0] == '+'))
    ++inext;

  if (inext < p->used)
    {
      next = p->p[inext];
      if ((colon = strchr(next, ':')))
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
iss_p_dump(FILE *pfp, Tsv *tp, Isp *ip, struct page *p, int zoom, int page, int firsth, int firsti, int lasti)
{
  int n = snprintf(NULL, 0, "z%dp%d", zoom, page);
  int suppress_space = 0;
  char key[n+1];
  sprintf(key, "z%dp%d", zoom, page);
  int j;
  fprintf(pfp, "%s\t", key);
  fflush(pfp);
  long tell = ftell(pfp);
  if (firsti < 0)
    firsti = firsth+1;
  fprintf(pfp, "%s\v%s", p->p[firsth], p->p[firsti]);
  for (j = firsti+1; j <= lasti; ++j)
    {
      if ('+' == p->p[j][0])
	continue;
      else if ('#' == p->p[j][0])
	{
	  fprintf(pfp, " %s\v", p->p[j]);
	  suppress_space = 1;
	}
      else if (suppress_space)
	{
	  fputs(p->p[j], pfp);
	  suppress_space = 0;
	}
      else	
	fprintf(pfp, " %s", p->p[j]);
    }
  fputc('\n', pfp);
  fflush(pfp);
  long len = ftell(pfp) - tell;
  tdb_add(tp, key, tell, (int)len, -1);
  return 0;
}

