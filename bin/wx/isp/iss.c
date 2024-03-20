#include <oraccsys.h>
#include "isp.h"

FILE *fpag = NULL;
Hash *seen = NULL;
Pool *pg2_pool;
struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

extern int nsortfields;
int nheadfields = 0;
int *headfields = NULL;
const char *terminal_sort_field = "designation";
char *tmp;

struct sortinfo *sip;

#if 0
extern int use_linkmap;
#endif

const char *heading_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *project = NULL;

#include "iss_sk_lookup.c"

static char *
append_designation(Isp *ip, const char *s)
{
#if 1
  char buf[strlen(s)+strlen(",designation0")];
  strcpy(buf, s);
  strcat(buf, ",designation");
  s = (ccp)pool_copy((uccp)buf,ip->p);
#else
  /* need do figure out what the sort_final stuff in p2 was; for now we do a dumb append */
  char *tmp2 = malloc(strlen(entry)+strlen(p2opts->sort_final)+2);
  sprintf(tmp2, "%s,%s", entry, p2opts->sort_final);
  s = (char*)pool_copy((unsigned char *)tmp2,ip->p);
  free(tmp2);
#endif
  return (char*)s;
}

static unsigned char *
adjust_s(unsigned char *stop, unsigned char *s)
{
  unsigned char *t = s;
  while (t > stop && isspace(t[-1]))
    --t;
  if (isspace(*t))
    *t = '\0';
  while (isspace(s[1]))
    ++s;
  return s;
}

static int
is_lang_id(const char *s)
{
  while (isalnum(*s) || '-' == *s)
    ++s;
  if (*s == '.')
    ++s;
  return 'x' == *s;
}

struct item *
pg_load(int *nitems)
{
  unsigned char *buf = NULL, *s;
  size_t buflen;
  if (listfile)
    buf = loadfile((unsigned const char *)listfile,&buflen);
  else
    buf = loadstdin(&buflen);
  s = buf;
  
  /* we know that each entry takes up at least 8 bytes */
  items = malloc(((buflen / 8) + 1) * sizeof(struct item));
  while (s - buf < buflen)
    {
      char *colon = NULL, *dot;
      unsigned char *orig_s = s;
      colon = strchr((char*)s,':');
      if (colon)
	{
	  orig_s = s;
	  s = (unsigned char *)++colon;
	}
      if (*s == 'P' || *s == 'Q' || *s == 'X' || is_lang_id((char *)s) || *s == 'o')
	{
	  items[items_used].s = orig_s;
	  while (*s && '\n' != *s)
	    ++s;
	  s = adjust_s(buf,s);
	  *s++ = '\0';
	  items[items_used].pq = (unsigned char*)strdup(colon 
							? colon
							: (char*)items[items_used].s);
	  if (colon)
	    {
	      items[items_used].qpq = malloc((colon - (ccp)orig_s)+strlen((ccp)items[items_used].pq)+2);
	      strncpy((char*)items[items_used].qpq, (ccp)orig_s, 1+(colon-(ccp)orig_s));
	      strcat((char*)items[items_used].qpq, (ccp)items[items_used].pq);
	    }
	  else
	    items[items_used].qpq = items[items_used].pq;
								     
	  if ((dot = strchr((const char *)items[items_used].pq,'.')))
	    {
	      *dot++ = '\0';
	      items[items_used].lkey = atoi(dot);
	    }
	  else
	    items[items_used].lkey = 0;
	  ++items_used;
	}
      else
	{
	  fprintf(stderr,"pg: bad list `%s'\n",listfile);
	  exit(1);
	}
    }
  *nitems = items_used;
  return items;
}

#if 0
static int
o_cmp(const void *a,const void*b)
{
  return ((struct outline*)a)->sic->seq - ((struct outline*)b)->sic->seq;
}

struct outline *
pg_outline(int *nlevelsp)
{
  int i, nlevels = nsortfields - 1, poffset;
  struct outline *o;
  u4 *u4s;
  o = malloc(sic_size * sizeof(struct outline));
  u4s = malloc(2 * sizeof(u4) * sic_size * nlevels);
  poffset = sic_size * nlevels;
  
  for (i = 0; i < sic_size; ++i)
    {
      int index = i * nlevels, j;
      o[i].count = o[i].page = -1;
      o[i].icounts = &u4s[index];
      o[i].poffsets = &u4s[poffset + index];
      o[i].sic = sicache[i];
      for (j = 0; j < nlevels; ++j)
	{
	  u4*pindex = sip->pindex + (sicache[i]->codes - sip->scodes);
	  o[i].poffsets[j] = pindex[sortfields[j]];
	}
    }

  qsort(o,sic_size,sizeof(struct outline),o_cmp);

  *nlevelsp = nlevels;
  return o;
}
#endif

static int *
set_keys(const char *s, int *nfields)
{
  int nkeys = 1, *fields;
  const char *t;
  
  if (!s || !strcmp(s,"none"))
    {
      *nfields = 0;
      return NULL;
    }

  t = s;
  while (*t)
    if (*t++ == ',')
      ++nkeys;
  fields = malloc(sizeof(int)*(nkeys+1));
  for(t = s, nkeys = 0; *t; ++nkeys)
    {
      if (isdigit(*t))
	fields[nkeys] = atoi(t);
      else
	fields[nkeys] = sk_lookup(t);
      while (*t && ',' != *t)
	++t;
      if (',' == *t)
	++t;
    }
  *nfields = nkeys;
  fields[nkeys] = -1;
  return fields;
}

int
ispsort(Isp *ip, const char *arg_project, const char *arg_listfile, const char *arg_sortkeys)
{
  struct item *items = NULL, **pitems = NULL;
  struct page *pages = NULL;
  int nitems = 0;

  if (ip)
    {
      project = ip->project;
      listfile = ip->cache.list;
      sort_keys = arg_sortkeys;
    }
  else
    {
      project = arg_project;
      listfile = arg_listfile;
      sort_keys = arg_sortkeys;
    }

  if (!project)
    {
      fprintf(stderr, "ispsortx: must give project with -p PROJECT. Stop.\n");
      exit(1);
    }

  if (listfile && access(listfile, R_OK))
    {
      fprintf(stderr, "ispsortx: list file non-existent or unreadable. Stop.\n");
      if (ip)
	{
	  ip->err = "list file non-existent or unreadable";
	  return 1;
	}
      else
	exit(1);
    }

  seen = hash_create(1024);
  pg2_pool = pool_init();

  if (NULL == (sip = si_load_csi(ip)))
    {
      if (ip)
	return 1;
      else
	exit(1);
    }

  if (!sort_keys)
    {
      sort_keys = "period,genre,provenience";
      ip->is.zlev = 3;
    }
  else
    {
      ip->is.zlev = 1;
      const char *k = sort_keys;
      while (*k)
	if (',' == *k++)
	  ++ip->is.zlev;
    }

  u4 i;
  Hash *known_fields = hash_create(5);
  for (i = 0; i < sip->nfields; ++i)
    hash_add(known_fields, sip->fields[i].n, "");
  char *s = strdup(sort_keys), *f, *tok;
  int badf = 0;
  tok = s;
  int designation_ok = 0;
  while ((f = strtok(tok, ",")))
    {
      tok = NULL;
      if (!hash_find(known_fields, (uccp)f))
	{
	  fprintf(stderr, "ispsortx: unknown field %s\n", f);
	  ++badf;
	}
      else if (!strcmp(f, "designation"))
	designation_ok = 1;
    }
  hash_free(known_fields, NULL);
  free(s);

  if (!designation_ok)
    sort_keys = append_designation(ip, sort_keys);

  if (badf)
    {
      if (ip)
	{
	  ip->err = "bad fields in sort specification";
	  return 1;
	}
      else
	exit(1);
    }
  
  if (!heading_keys)
    {
      heading_keys = (ccp)pool_copy((uccp)sort_keys, pg2_pool);
      char *des = strstr(heading_keys, "designation");
      if (des && !strchr(des, ',')) /* assumes field1,designation,field3 */
	*des = '\0';
    }

  if (heading_keys)
    headfields = set_keys(heading_keys, &nheadfields);
  
  items = pg_load(&nitems);
  
  if (NULL == (pitems = pg_sort(ip, items, &nitems, sort_keys)))
    return 1;

#if 0
  ip->op = pg_outline(&ip->op_nlevels);
#endif

  if (nitems)
    pages = pg_page(ip, pitems, nitems);
  
  pg_page_dump(ip, pages);
  
  hash_free2(seen, free, NULL);

  return 0;
}
