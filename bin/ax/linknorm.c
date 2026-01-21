#include <oraccsys.h>
#include <roco.h>

#define pqx(x) (r->rows[x][0])
#define rel(x) (r->rows[x][1])
#define lnk(x) (r->rows[x][2])
  
static char *curr_pqx = "";
Hash *symtab = NULL;
Hash *rels;
Pool *p;

static const char *
get_sym(unsigned char *reln)
{
  const char *s = (ccp)reln;
  if (isalpha(*s))
    s = strchr(s, ' ')+1;
  else
    s += 2;
  if (hash_find(symtab, (uccp)s))
    return s;
  else
    return NULL;
}

static void
has_witness(Roco *r, int i)
{
  const char *sym = get_sym(rel(i));
  if (sym)
    {
      char buf[];
      hash_add(rels, pool_copy((ucp)buf, p), "");
    }
}

static void
is_witness_of(Roco *r, int i)
{
  const char *sym = get_sym(rel(i));
  if (sym)
    {
      char buf[];
      hash_add(rels, pool_copy((ucp)buf, p), "");
    }
}

static void
parallels(Roco *r, int i)
{
  const char *sym = get_sym(rel(i));
  if (sym)
    {
      char buf[];
      hash_add(rels, pool_copy((ucp)buf, p), "");
    }
}

int
main(int argc, char *const *argv)
{
  Roco *r = NULL;
  rels = hash_create(1024);
  p = pool_init();

  options(argc, argv, "?");
  r = roco_load1("-");

  int i;
  for (i = 0; r->rows[i]; ++i)
    {
      if (strcmp(curr_pqx, (ccp)pqx(i)))
	{
	  if (symtab)
	    hash_free(symtab, NULL);
	  symtab = hash_create(1024);
	  curr_pqx = (char*)pqx(i);
	}
      switch (*rel(i))
	{
	case 'd':
	  hash_add(symtab, rel(i)+4, lnk(i));
	  break;
	case '>':
	case '<':
	  has_witness(r, i);
	  break;
	case '|':
	case 'p':
	  parallels(r, i);	  
	  break;
	case '+':
	  fprintf(stderr, "linknorm: ++ found: %s %s %s\n", pqx(i), rel(i), lnk(i));
	  break;
	case 's':
	  has_witness(r, i);
	  is_witness_of(r, i);
	  break;
	default:
	  fprintf(stderr, "linknorm: unexpected: %s %s %s\n", pqx(i), rel(i), lnk(i));
	  break;
	}
    }

  int n;
  const char **rr = hash_keys2(rels, &n);
  qsort(rr, n, sizeof(const char*), cmpstringp);
  for (i = 0; i < n; ++i)
    puts(rr[i]);
}

const char *prog = "rocox";
int major_version = 1, minor_version = 0;
const char *usage_string = "";

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
}
