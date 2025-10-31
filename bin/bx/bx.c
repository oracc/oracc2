#include <oraccsys.h>
#include <locale.h>
#include <oracclocale.h>
#include <runexpat.h>
#include "bx.h"
#if 0
#include "bbl.h"
#endif

Bx b = {
  .mode=BX_CIT ,
  .pre[BX_CIT] = bx_cit_pre,
  .pre[BX_ICF] = bx_icf_pre,
  .pre[BX_KEY] = bx_key_pre,
  .pre[BX_REF] = bx_ref_pre,
  .run[BX_CIT] = bx_cit_run,
  .run[BX_ICF] = bx_icf_run,
  .run[BX_KEY] = bx_key_run,
  .run[BX_REF] = bx_ref_run,
  .out[BX_CIT] = bx_cit_out,
  .out[BX_ICF] = bx_icf_out,
  .out[BX_KEY] = bx_key_out,
  .out[BX_REF] = bx_ref_out
};

extern int optind;

const char *bibsfile = NULL; /* file of paths to .bib files */
const char *cite_type = NULL;
int verbose = 0;

int
main(int argc, char * const*argv)
{
  setlocale (LC_ALL, ORACC_LOCALE);
  b.argv = argv;
  options(argc, argv,
	  "fkr"     /* mode args */
	  "b:c:"    /* input args */
	  "d:h:ix:" /* output args */ 
	  "npqtv"     /* adjunct args */
	  );
  mesg_init();
  b.p = pool_init();
  b.pre[b.mode](&b);
  b.run[b.mode](&b);
  b.out[b.mode](&b);

  mesg_print(stderr);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'f':
      b.mode = BX_ICF;
      break;
    case 'k':
      b.mode = BX_KEY;
      break;
    case 'r':
      b.mode = BX_REF;
      break;
    case 'b':
      b.flist_bib = arg;
      break;
    case 'c':
      b.flist_cit = arg;
      break;
    case 'n':
      b.no_output = 1;
      break;
    case 'o':
      b.outfile = arg; /* in keys_mode a file; in dotbib_mode a .bib file */
      break;
    case 'p':
      b.project = arg;
      break;
    case 'q':
      b.quiet = 1;
      break;
    case 't':
      cite_type = arg;
      break;
    case 'v':
      ++verbose;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void) {}
