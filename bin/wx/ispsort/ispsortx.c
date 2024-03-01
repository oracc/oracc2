#include <oraccsys.h>
#include "ispsort.h"

FILE *fpag = NULL;
Hash_table *seen = NULL;
Pool *pg2_pool;
struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

int nheadfields = 0;
int *headfields = NULL;
const char *terminal_sort_field = "designation";

#if 0
extern int use_linkmap;
#endif

const char *heading_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *project = NULL;

static int sk_lookup(const char *k);

static int
sk_lookup(const char *k)
{
  static char field[128];
  char *f = field;
  int i;
  int vbar = 0;

 retry:
  {
    while (*k && *k != ',' && *k != '|')
      *f++ = *k++;
    *f = '\0';
    vbar = '|' == *k;
        
    for (i = 0; i < sip->nmapentries; ++i)
      if (!strcmp(field,(const char*)sip->fields[i].n))
	break;
    
    if (i == sip->nmapentries)
      {
	if (vbar)
	  {
	    ++k;
	    f = field;
	    goto retry;
	  }
	else
	  {
	    fprintf(stderr,"pg: field %s not in fields list\n", field);
	    exit(1);
	  }
      }
    return sip->fields[i].field_index;
  }
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
main(int argc, char **argv)
{
  struct item *items = NULL, **pitems = NULL;
  struct page *pages = NULL;
  int nitems = 0, nlevels = 0, npages = 0;

  options(argc,argv,"l:p:s:");

  seen = hash_create(1024);
  pg2_pool = pool_init();

  if (NULL == fdbg)
    fdbg = stderr;
  if (!fpag)
    fpag = stdout;
  
  if (!heading_keys)
    {
      heading_keys = (char *)npool_copy((unsigned char *)sort_keys, pg2_pool);
      tmp = (char*)heading_keys + strlen(heading_keys) - strlen(p2opts->sort_final);
      *tmp = '\0';
    }
      
  if (NULL == (sip = si_load_csi()))
    exit(1);

  if (heading_keys)
    headfields = set_keys(heading_keys, &nheadfields);

  items = pg_load(&nitems);

  pitems = pg_sort(items, &nitems, sort_keys);

  /*op = pg_outline(&nlevels);*/

  if (nitems)
    pages = pg_page(pitems, nitems, &npages, op);

  pg_page_dump_all(fpag,pages,npages);

  fclose(fpag);

  hash_free2(seen, free, NULL);

  exit(0);
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'l':
      listfile = arg;
      break;
#if 0      
    case 'm':
      use_linkmap = 1;
      break;
#endif
    case 'p':
      project = arg;
      break;
    case 's':
      sort_keys = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "ispsortx";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
void
help ()
{

}
