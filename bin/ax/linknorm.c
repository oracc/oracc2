#include <oraccsys.h>
#include <roco.h>

int
main(int argc, char *const *argv)
{
  Roco *r = NULL;

  options(argc, argv, "?");
  r = roco_load1("-");
  char *curr_pqx = "";
  Hash *symtab = NULL;
  Hash *rels = hash_create(1024);
  Pool *p = pool_init();

#define pqx(x) (r->rows[x][0])
#define rel(x) (r->rows[x][1])
#define lnk(x) (r->rows[x][2])
  
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
	  break;
	case '<':
	  break;
	case '|':
	  break;
	case '+':
	  fprintf(stderr, "linknorm: ++ found: %s %s %s\n", pqx(i), rel(i), lnk(i));
	  break;
	case 'p':
	  break;
	case 's':
	  break;
	default:
	  fprintf(stderr, "linknorm: unexpected: %s %s %s\n", pqx(i), rel(i), lnk(i));
	  break;
	}
    }
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
