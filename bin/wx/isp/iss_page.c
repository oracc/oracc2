#include <oraccsys.h>
#include "iss.h"

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

static char *
plussed(unsigned char *qpq)
{
  char *colon = strchr(qpq, ':');
  if (colon)
    return colon;
  else
    return (char*)qpq;
}

/* The pg2 version of this routine split the pages up; this one
   creates a single list */
struct page *
pg_page(struct item **pitems, int nitems, int *npages)
{
  static int last_i = 0, i;
  Unsigned32 last_lkey = 0;
  
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

  for (i = 0; i < nitems; ++i)
    {
      if (pitems[i]->grp < 0)
	continue;

      if (i && pitems[i]->grp != pitems[i-1]->grp && pitems[i-1]->grp != -1)
	{
	  last_i = i;
	  if (nheadfields)
	    {
	      currpage->p[currpage->used++] = fmthdr(pitems[i]->grp);
	      last_lkey = 0;
	    }
	}
      if (i && !strcmp(pitems[i-1]->qpq, pitems[i]->qpq)
	  && pitems[i-1]->lkey == pitems[i]->lkey)
	currpage->p[currpage->used++] = plussed(pitems[i]->s);
      else
	currpage->p[currpage->used++] = (char *)pitems[i]->s;
    }

  *npages = 1;
  return pages;
}

void
pg_page_dump_all(FILE *fp, struct page *pages, int npages)
{
  int i;
  for (i = 0; i < npages; ++i)
    pg_page_dump_one(fp, &pages[i]);
}

/* The pg2 version of this routine printed page-size groups on a line
   with headings interspersed among the IDs.

   This version prints each zoom-group on its own line.
 */
void
pg_page_dump_one(FILE *fp, struct page *p)
{
  int i;
  for (i = 0; i < p->used; ++i)
    {
      if ('#' == p->p[i])
	fputc('\n',fp);
      fprintf(fp,"%s ", p->p[i]);
    }
  fputc('\n',fp);
}
