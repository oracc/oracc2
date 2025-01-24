#include <oraccsys.h>
#include "nx.h"

Pool *nspool;

const char *arg_dat = NULL;

int opt_json;
int opt_pacify;
int opt_trace;
int opt_xml;

int verbose;
int status;

const char *jfn = NULL;
const char *xfn = NULL;

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
  nxp->lsys = list_create(LIST_SINGLE);
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
  list_free(nxp->lsys, NULL);
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
main(int argc, char * const *argv)
{
  extern int parse_trace, nsdebug, token_trace, values_trace;

  /*build_trace,*/
    
  if (options(argc, argv, "d:D:jpx"))
    exit(1);

#if 1
  if (nxd_lex())
    ns_flex_debug = nsflextrace = 1;

  if (nxd_yacc())
    nsdebug = 1;
  else
    nsdebug = ns_flex_debug = 0;

  if (nxd_parse())
    parse_trace = 1;

  if (nxd_tokens())
    token_trace = 1;

  if (nxd_values())
    values_trace = 1;
#else
  build_trace = ns_flex_debug = nsflextrace = debug;
#endif
  
  nx_init();

  ns_data();

  if (opt_xml)
    {
      (void)ns_jx(nxp->hsys, nxp->lsys);
      exit(0);
    }

  if (argv[optind])
    {
      nx_cli_input(argv+optind);
    }
  else
    {
      if (opt_pacify)
	fprintf(stderr, "nx: waiting for input:\n");
      
      nxp->input = stdin; /* set this in opts to read from file/toks/etc */
      
      nx_input();
    }

  nx_term();
}

int
opts(int c, const char *v)
{
  switch (c)
    {
    case 'D':
      nxd_arg = v;
      if (!nxd_arg_ok(nxd_arg))
	return 1;
      break;
    case 'd':
      arg_dat = v;
      break;
    case 'j':
      opt_json = 1;
      break;
    case 'p':
      opt_pacify = 1;
      break;
    case 'x':
      opt_xml = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

void
help(void)
{
  printf("nx [-d[DATA]] [-D[LYTPV]] [-j] [-p] [-x]\n");
}
