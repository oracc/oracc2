#include <oraccsys.h>
#include "nx.h"

Pool *nspool;

const char *arg_dat = NULL;
int opt_trace;

#if 0
static const uchar **nx_toks(uchar *t, int *ntoks);
#endif

/* The data manager object for the run; probably need to reset this
   after each text to handle large runs */
nx *nxp;

void
nx_init(void)
{
  nxp = calloc(1, sizeof(nx));
  nspool = nxp->b = pool_init();
  nxp->p = pool_init();
  nxp->m_ns_sys = memo_init(sizeof(ns_sys), 10);
  nxp->m_ns_step = memo_init(sizeof(ns_step), 100);
  nxp->m_ns_inst = memo_init(sizeof(ns_inst), 1000);
  nxp->m_nx_step = memo_init(sizeof(nx_step), 100);
  nxp->m_nx_inst = memo_init(sizeof(ns_inst), 1000);
  nxp->m_nx_number = memo_init(sizeof(nx_number), 100);
  nxp->hsys = hash_create(10);
  nxp->ir = hash_create(1024);
  nxp->env = hash_create(7);
  nxp->e = hpool_init();
  nxp->hashes = list_create(LIST_SINGLE);
  nxp->lists = list_create(LIST_SINGLE);
  nxp->data_is_char = 1;
}

void
nx_reset(void)
{
  pool_reset(nxp->p);
#if 0
  memo_reset(nxp->m_ns_sys);
  memo_reset(nxp->m_ns_step);
  memo_reset(nxp->m_ns_inst);
#endif
  if (nxp->toks)
    free(nxp->toks);
  if (nxp->data)
    free(nxp->data);
  nxp->toks = nxp->data = NULL;
  memo_reset(nxp->m_nx_step);
  memo_reset(nxp->m_nx_inst);
  memo_reset(nxp->m_nx_number);
}

void
nx_term(void)
{
  mesg_term();
  pool_term(nxp->b);
  pool_term(nxp->p);
  memo_term(nxp->m_ns_sys);
  memo_term(nxp->m_ns_step);
  memo_term(nxp->m_ns_inst);
  memo_term(nxp->m_nx_step);
  memo_term(nxp->m_nx_inst);
  memo_term(nxp->m_nx_number);
  hash_free(nxp->hsys, NULL);
  hash_free(nxp->ir, NULL);
  if (nxp->toks)
    free(nxp->toks);
  if (nxp->data)
    free(nxp->data);
  nxp->toks = nxp->data = NULL;
  if (nxp->hashes)
    {
      Hash *h;
      for (h = list_first(nxp->hashes); h; h = list_next(nxp->hashes))
	hash_free(h, NULL);
      list_free(nxp->hashes, NULL);
    }
  if (nxp->lists)
    {
      List *l;
      for (l = list_first(nxp->lists); l; l = list_next(nxp->lists))
	list_free(l, NULL);
      list_free(nxp->lists, NULL);
    }
  hpool_term(nxp->e);
  hash_free(nxp->env, NULL);
  free(nxp);
  nxp = NULL;
  nspool = NULL;
}

#if 0
const char *test[] = { "1(u)" , "ba" , "2(u)" , "1(aš)" , "bu" , NULL };
#endif

int
main(int argc, char **argv)
{
  options(argc, argv, "d:t");

  extern int build_trace, parse_trace, debug;

  if ((parse_trace = opt_trace))
    debug = 1;

  build_trace = ns_flex_debug = nsflextrace = debug;

  nx_init();

  ns_data();

#if 0
  nxp->testfp = fopen("test.tsv", "w");
  if (!nxp->testfp)
    fprintf(stderr, "nx: unable to open test.tsv; no testdata will be written\n");
#endif
  
  nxp->input = stdin; /* set this in opts to read from file/toks/etc */
  
  nx_input();

#if 0
  if (nxp->testfp)
    fclose(nxp->testfp);
#endif

  nx_term();
}

int
opts(int c, const char *v)
{
  switch (c)
    {
    case 'd':
      arg_dat = v;
      break;
    case 't':
      opt_trace = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

void
help(void)
{
}
