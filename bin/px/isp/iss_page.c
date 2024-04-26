#include <oraccsys.h>
#include "isp.h"

struct page *pages;
struct page *currpage;
int currpage_used;
int npage = 0;
int pagesize = 15;
extern int quick;

struct pgtell
{
  long ptell;
  int plen;
  long htell;
  int hlen;
};

struct pgtell *
pg_tell(Isp *ip, long ht, int hl, long pstart, FILE *fp)
{
  struct pgtell *pt = memo_new(ip->tmem);
  pt->htell = ht;
  pt->hlen = hl;
  pt->ptell = pstart;
  fflush(fp);
  pt->plen = ftell(fp)-pstart;
  return pt;
}

/* zmax is maximum value of zoom for full page; zimx is maximum value
   of item for zoomed page */
int
pg_map_one(Isp *ip, FILE *fp, int zmax, int zimx, List *lmap)
{
#if 1
  /* Reinitialize before dumping pmp for z0 */
  if (!ip)
    {
      return 0;
    }
  struct pgtell *pt;
  for (pt = list_first(lmap); pt; pt = list_next(lmap))
    {
#if 0
      long start = (ptlast ? ptlast->ptell+1 : 0L);
      if (start < pt->htell+pt->hlen)
	start = pt->htell+pt->hlen;
#endif
      fprintf(fp, "%d/%d/%ld/%d/%ld/%d\n", zmax, zimx, pt->htell, pt->hlen, pt->ptell, pt->plen);
    }
#else  
  int nth = 0;
  List_node *np = lmap->first;
  while (nth < list_len(lmap))
    {
      struct pgtell *pt = np->data;
      long start = (nth ? ((struct pgtell*)(np->prev->data))->ptell+1 : 0L);
      fprintf(fp, "%d/%d/%ld/%d/%ld/%ld\n", zmax, zimx, pt->htell, pt->hlen, start, pt->ptell);
      np = np->next;
      ++nth;
    }
#endif
  return 0;
}

int
pg_zmaps(Isp *ip, List *zmaps, List *z)
{
  int znth = 0;
  int zcounts[list_len(z)+1];
  List_node *np = z->first;
  while (znth < list_len(z))
    {
      zcounts[znth++] = (uintptr_t)np->data;
      np = np->next;
    }

#if 0
  char zfn[strlen(ip->cache.sort)+strlen(".zmp0")];
  sprintf(zfn, "%s.zmp", ip->cache.sort);
#endif

  znth = 0;
  List *lp;
  for (lp = list_first(zmaps); lp; lp = list_next(zmaps))
    {
      char pfn[strlen(ip->cache.sort)+strlen("-z12340")];
      sprintf(pfn, "%s-z%d.pmp", ip->cache.sort, znth+1); /* should rewrite to use snprintf */
      FILE *fp = fopen(pfn, "w");
      pg_map_one(ip, fp, list_len(z), zcounts[znth], lp);
      fclose(fp);
      ++znth;
    }
  return 0;
}

char *
fmthdr(short sic_index)
{
  static char buf[1024], *s;
  int i;
  strcpy(buf,"#");
  for (i = 0; i < nheadfields; ++i)
    {
      u4*pindex = sip->pindex + (sicache[sic_index]->codes - sip->scodes);
      u4 poff = pindex[headfields[i]];
      sprintf(buf+strlen(buf),"%s\t", &sip->pool[poff]);
    }
  buf[strlen(buf)-1] = '\0';
  for (s = buf; *s; ++s)
    if (' ' == *s)
      *s = '_';
  s = malloc(strlen(buf)+1);
  strcpy(s,buf);
  return s;
}

static char *
plussed(unsigned char *qpq)
{
  char *colon = strchr((ccp)qpq, ':');
  if (colon)
    {
      *colon = '+';
      return colon;
    }
  else
    return (char*)qpq;
}

/* The pg2 version of this routine split the pages up; this one
   creates a single list */
struct page *
pg_page(Isp *ip, struct item **pitems, int nitems)
{
  static int last_i = 0, i;

  npage = 1;
  pages = malloc(npage * sizeof(struct page));
  pages[i].used = 0;
  pages[i].p = malloc((nitems*2) * sizeof(char *));
  currpage = &pages[0];

  if (nheadfields)
    {
      int x = 0;
      while (pitems[x]->grp < 0)
	++x;

      currpage->p[0] = fmthdr(pitems[x]->grp);
      currpage->used = 1;
    }
  else
    currpage->used = 0;

  struct outline *ipop = ip->op;
  
  if (ipop)
    ipop->page = 1;

  for (i = 0; i < nitems; ++i)
    {
      if (pitems[i]->grp < 0)
	continue;

      if (i && pitems[i]->grp != pitems[i-1]->grp && pitems[i-1]->grp != -1)
	{
         if (ipop)
            ipop->count = i - last_i;
	  last_i = i;
	  if (nheadfields)
	    currpage->p[currpage->used++] = fmthdr(pitems[i]->grp);
          if (ipop)
            {
              ++ipop;
              ipop->page = 1 + currpage - pages;
            }
	}
      if (i && !strcmp((ccp)pitems[i-1]->qpq, (ccp)pitems[i]->qpq)
	  && pitems[i-1]->lkey == pitems[i]->lkey)
	currpage->p[currpage->used++] = plussed(pitems[i]->s);
      else
	currpage->p[currpage->used++] = (char *)pitems[i]->s;
    }

 if (ipop)
    ipop->count = i - last_i;

  return pages;
}

/* The pg2 version of this routine printed page-size groups on a line
   with headings interspersed among the IDs.

   This version prints each zoom-group on its own line.
 */
#if 1
int
pg_page_dump(Isp *ip, struct page *p)
{
  FILE *fpag = NULL;
  int zmax = 0, imax = 0;
  if (ip->cache.sort)
    {
      if (!(fpag = fopen(ip->cache.sort, "w")))
	{
	  ip->err = "unable to open output for sort";
	  return 1;
	}
      else if (ip->verbose)
	fprintf(stderr, "isp: ispsort writing %s\n", ip->cache.sort);
    }
  else
    fpag = stdout;
  int i;
  for (i = 0; i < p->used; ++i)
    {
      if ('#' == p->p[i][0])
	{
	  ++zmax;
	  if (i)
	    fputc('\n',fpag);
	  fputs(p->p[i], fpag);
	}
      else if ('+' != p->p[i][0])
	{
	  ++imax;
	  fputc(' ', fpag);
	  fputs(p->p[i], fpag);
	}
    }
  fputc('\n',fpag);
  if (ip)
    {
      fclose(fpag);
      unsigned char *f = loadfile((uccp)ip->cache.sort, NULL);
      ispmp_zooms(ip, f, zmax);
      ispmp_pages(ip, f, imax);
      if (ispo_master(ip))
	return 1;
    }
  
  return 0;
}
#else
int
pg_page_dump(Isp *ip, struct page *p)
{
  int p_count = 0; /* count of IDs in the one-big-page */
  int z_count = 0; /* count of IDs in the current zoom */
  int z_index = 0; /* index of current zoom into zooms */
  List *z = list_create(LIST_SINGLE); /* list of number of IDs in each zoom */
  List *pmap = list_create(LIST_DOUBLE);
  List *zmap = list_create(LIST_DOUBLE);
  List *zmaps = list_create(LIST_SINGLE);
  long htell = 0L, pstart = 0L, zstart = 0L, phtell = -1L, ppstart = -1L;
  int hlen, phlen;

  FILE *fpag = NULL;
  ip->tmem = memo_init(sizeof(struct pgtell), 128);
  if (ip->cache.sort)
    {
      if (!(fpag = fopen(ip->cache.sort, "w")))
	{
	  ip->err = "unable to open output for sort";
	  return 1;
	}
      else if (ip->verbose)
	fprintf(stderr, "isp: ispsort writing %s\n", ip->cache.sort);
    }
  else
    fpag = stdout;
  int i;
  for (i = 0; i < p->used; ++i)
    {
      if ('#' == p->p[i][0])
	{
	  if (i)
	    {
	      fputc('\n',fpag);
	      list_add(z, (void*)(uintptr_t)z_count);
	      list_add(zmap, pg_tell(ip, htell, hlen, pstart, fpag));
	      list_add(zmaps,zmap);
	    }
	  fflush(fpag);
	  htell = ftell(fpag);
	  hlen = strlen(p->p[i]);
	  if (phtell < 0L)
	    {
	      phtell = htell;
	      phlen = hlen;
	    }
	  fputs(p->p[i], fpag);
	  fflush(fpag);
	  pstart = ftell(fpag)+1;
	  if (ppstart < 0L)
	    ppstart = pstart;
	  zmap = list_create(LIST_DOUBLE);
	  ++z_index;
	  z_count = 0;
	}
      else if ('+' != p->p[i][0])
	{
	  fputc(' ', fpag);
	  ++z_count;
	  if (!(z_count%25))
	    {
	      list_add(zmap, pg_tell(ip, htell, hlen, zstart, fpag));
	      fflush(fpag);
	      zstart = ftell(fpag);
	    }
	  ++p_count;
	  if (!(p_count%25))
	    {
	      list_add(pmap, pg_tell(ip, phtell, phlen, ppstart, fpag));
	      ppstart = phtell = -1L;
	      fflush(fpag);
	      pstart = ftell(fpag);
	    }
	  fputs(p->p[i], fpag);
	}
    }
  fputc('\n',fpag);
  list_add(z, (void*)(uintptr_t)z_count);
  list_add(zmap, pg_tell(ip, htell, hlen, pstart, fpag));
  list_add(zmaps, zmap);
  if (p_count)
    list_add(pmap, pg_tell(ip, phtell, phlen, ppstart, fpag));

  /* This is zoom=0 */
  if (ip)
    {
      fclose(fpag);
      pg_zmaps(ip, zmaps, z);
      char pfn[strlen(ip->cache.sort)+strlen("-z12340")];
      sprintf(pfn, "%s.pmp", ip->cache.sort);
      pg_map_one(NULL, NULL, 0, 0, NULL);
      FILE *fp = fopen(pfn, "w");
      pg_map_one(ip, fp, list_len(z), p_count, pmap);
      fclose(fp);
      if (ispo_master(ip))
	return 1;
    }
  list_free(z, NULL);
  list_free(zmap, NULL);
  list_free(pmap, NULL);
  
  return 0;
}
#endif
