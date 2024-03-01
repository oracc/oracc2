#include <oraccsys.h>
#include "ispsort.h"

struct page *pages;
struct page *currpage;
int currpage_used;
int npage = 0;
int pagesize = 15;
extern int quick;

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

struct page *
pg_page(struct item **pitems, int nitems, int *npages, struct outline *outlinep)
{
  static int last_i = 0, i;

  /* page the results */
  npage = (nitems / pagesize) + (nitems % pagesize ? 1 : 0);
  pages = malloc(npage * sizeof(struct page));
  for (i = 0; i < npage; ++i)
    {
      pages[i].used = 0;
      pages[i].p = malloc((pagesize*2) * sizeof(char *));
    }
  currpage = pages;
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

  if (outlinep)
    outlinep->page = 1;
  
  for (i = 0; i < nitems; ++i)
    {
      if (pitems[i]->grp < 0)
	continue;

      if (i && pitems[i]->grp != pitems[i-1]->grp && pitems[i-1]->grp != -1)
	{
	  if (outlinep)
	    outlinep->count = i - last_i;
	  last_i = i;
	  /* 	  if (currpage->used < pagesize) */
	  if (nheadfields)
	    currpage->p[currpage->used++] = fmthdr(pitems[i]->grp);
	  if (outlinep)
	    {
	      ++outlinep;
	      outlinep->page = 1 + currpage - pages;
	    }
	}
      if (currpage_used == pagesize)
	{
	  if (*currpage->p[currpage->used-1] == '#') {
	    --currpage->used;
	    if (outlinep)
	      (outlinep-1)->page++;
	  }
	  ++currpage;
	  currpage_used = 0;
	  if (nheadfields)
	    {
	      currpage->p[0] = fmthdr(pitems[i]->grp);
	      currpage->used = 1;
	    }
	  else
	    currpage->used = 0;
	}
      currpage->p[currpage->used++] = (char *)pitems[i]->s;
      ++currpage_used;
    }
  if (outlinep)
    outlinep->count = i - last_i;

  *npages = npage;
  return pages;
}

void
pg_page_dump_all(FILE *fp, struct page *pages, int npages)
{
  int i;
  for (i = 0; i < npages; ++i)
    pg_page_dump_one(fp, &pages[i]);
}

void
pg_page_dump_one(FILE *fp, struct page *p)
{
  int i;
  for (i = 0; i < p->used; ++i)
    fprintf(fp,"%s ", p->p[i]);
  fputc('\n',fp);
}
