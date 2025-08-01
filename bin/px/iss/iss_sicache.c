#include <oraccsys.h>
#include "iss.h"

extern int csi_debug;
extern int nsortfields;
extern int *sortfields;
extern struct sortinfo *sip;

static int sic_id = 0;
static struct si_cache **siclist;

int
sic_cmp(const void *a, const void *b, const void *c)
{
  int i,max;
  for (i = 0,max=nsortfields-1; i < max; ++i)
    {
      int f = sortfields[i];
      if (((struct si_cache*)a)->codes[f] != ((struct si_cache*)b)->codes[f])
	return ((struct si_cache*)a)->codes[f] - ((struct si_cache*)b)->codes[f];
    }
  return 0;
}

int
sic_id_cmp(const void *a, const void *b)
{
  return (*(struct si_cache**)a)->id - (*(struct si_cache**)b)->id;
}

int
sic_pd_cmp(const void *a, const void *b)
{
  return (*(struct si_cache**)a)->codes[2] - (*(struct si_cache**)b)->codes[2];
}

/* resort the cache by period */
void
pd_sort_cache(void)
{
  qsort(siclist, sic_id, sizeof(struct si_cache *), sic_pd_cmp);
}

struct si_cache **
make_cache(Isp *ip, struct item **items, int nitems, int*sicsize)
{
  struct rbtree *rb;
  RBLIST *rbl;
  struct si_cache *sicp;
  int i;
  static u4 top;
  extern Hash *seen;

  sic_id = 0;

  /* No cache unless there are keys; NB: we use a cache
     even for 1 key because it simplifies X-processing */
  if (nsortfields == 0)
    return NULL;
  rb = rbinit(sic_cmp,NULL);

  top = sip->qoffset;

  /* cache the indexes into the sortinfo */
  for (i = 0; i < nitems; ++i)
    {
      static u4 id = -1, new_id;
      static int j = 0, last_j;

      if (0 == i)
	{
	  j = 0;
	  id = -1;
	}

      new_id = strtoul((const char *)items[i]->pq + (ip->ood ? 0 : 1),NULL,10);
      if (*items[i]->pq == 'X')
	new_id += 2000000;

      if (id == new_id)
	{
	  items[i]->skey = items[i-1]->skey;
	  items[i]->grp = items[i-1]->grp;
	  continue;
	}
      else
	id = new_id;
      /*      fprintf(stderr,"id=%ld\n",id);*/
      if (j <= sip->qoffset && items[i]->pq[0] == 'Q')
	{
	  /*  fprintf(stderr,"resetting j to %ld; top to %ld\n",sip->qoffset, sip->nmember); */
	  j = sip->qoffset;
	  top = sip->nmember;
	}

      for (last_j = j; j <= top; ++j)
	if (id == sip->idlist[j])
	  {
	    static struct si_cache sic_tmp;
	    struct si_cache *sic;
	    /* fprintf(stderr,"found id %ld at j=%ld\n",id,j); */
	    sic_tmp.codes = &sip->scodes[j*sip->nfields];
	    if (NULL == (sic = (struct si_cache*)rbfind(&sic_tmp, rb)))
	      {
		sic = malloc(sizeof(struct si_cache));
		sic->codes = &sip->scodes[j*sip->nfields];
		sic->id = sic_id++;
		(void)rbsearch(sic, rb);
	      }
	    items[i]->grp = sic->id;
	    items[i]->skey = sip->scodes[(sip->nfields*j++) + sortfields[nsortfields-1]];
	    break;
	  }
      if (j > top)
	{
	  static int one = 1;
	  char buf[10];
	  int *onep = NULL;
	  if (ip->ood)
	    sprintf(buf, "%04u", id);
	  else
	    sprintf(buf, "%u", id);
	  onep = hash_find(seen, (unsigned char *)buf);
	  if (!onep)
	    {
	      char pqxbuf[8];
	      if (id > 2000000)
		sprintf(pqxbuf, "X%06d", id-2000000);
	      else
		sprintf(pqxbuf, "P-or-Q%06d", id);

	      fprintf(stderr,"iss_sicache: member %s (%s) not in sortinfo\n", buf, pqxbuf);
	      hash_add(seen, (unsigned char*)strdup(buf), &one);
	    }
	  items[i]->skey = 1000000; /* "unknown" */
	  items[i]->grp = -1;
	  j = last_j;
	}
    }

  /*hash_free2(seen, free, NULL);*/
  
  /* create an array that can be indexed during sorting and grouping */
  siclist = malloc(sic_id * sizeof(struct si_cache*));
  rbl = rbopenlist(rb);
  for (i = 0; NULL != (sicp = (struct si_cache*)rbreadlist(rbl)); ++i)
    {
      sicp->seq = i;
      siclist[i] = sicp;
    }
  rbcloselist(rbl);

  /* the list is now sorted by sequence, so resort it by id so that
     indexing from pdata elements works correctly */
  qsort(siclist, sic_id, sizeof(struct si_cache *), sic_id_cmp);

  *sicsize = sic_id;
  return siclist;
}
