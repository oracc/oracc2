#include <oraccsys.h>
#include "nx.h"

Pool *nspool;

static const uchar **nx_toks(uchar *t, int *ntoks);

/* The data manager object for the run; probably need to reset this
   after each text to handle large runs */
nx *nxp;

void
nx_init(void)
{
  nxp = calloc(1, sizeof(nx));
  nspool = nxp->p = pool_init();
  nxp->m_sys = memo_init(sizeof(ns_sys), 10);
  nxp->m_step = memo_init(sizeof(ns_step), 100);
  nxp->m_nx_step = memo_init(sizeof(nx_step), 100);
  nxp->m_inst = memo_init(sizeof(ns_inst), 1000);
  nxp->m_nx_number = memo_init(sizeof(nx_number), 100);
  nxp->ir = hash_create(1024);
}


const char *test[] = { "1(u)" , "ba" , "2(u)" , "1(aš)" , "bu" , NULL };


int
main(int argc, char **argv)
{
  ns_flex_debug = nsflextrace = 0;
  nx_init();

  nx_data();

  uchar *lp;
  size_t lplen;
  while ((lp = loadoneline(stdin, &lplen)))
    {
      int ntoks;
      const uchar **toks = nx_toks(lp, &ntoks);
      mesg_init();
      nx_result *r = nx_parse(toks, NULL, -1);
      nxr_print(r, stdout);
      mesg_print(stderr);
    }
}

static const uchar **
nx_toks(uchar *t, int *ntoks)
{
  int n = 0;
  while (isspace(*t))
    ++t;
  uchar *s = t;
  while (*s)
    {
      ++n;
      while (*s && !isspace(*s))
	++s;
      while (isspace(*s))
	++s;
    }
  const uchar **toks = malloc((n+1)*sizeof(const uchar *));
  s = t;
  int i = 0;
  while (*s)
    {
      toks[i++] = s;
      while (*s && !isspace(*s))
	++s;
      if (isspace(*s))
	*s++ = '\0';
      while (isspace(*s))
	++s;
    }
  toks[i] = NULL;
  *ntoks = n;
  return toks;
}
