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
  .pre[BX_REF] = bx_ref_pre
};

extern int optind;

const char *bibsfile = NULL; /* file of paths to .bib files */
const char *cite_type = NULL;
int no_output = 0;
int verbose = 0;

const char *output = NULL;

int
main(int argc, char * const*argv)
{
  setlocale (LC_ALL, ORACC_LOCALE);
  options(argc, argv,
	  "fkr"     /* mode args */
	  "b:ptv"   /* adjunct args */
	  "d:h:ix:" /* output args */ 
	  );

  b.pre[b.mode](&b);
  b.run[b.mode](&b);
  b.out[b.mode](&b);
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
      bibsfile = arg;
      break;
    case 'o':
      output = arg; /* in keys_mode a file; in dotbib_mode a .bib file */
      break;
    case 'p':
      b.project = arg;
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
