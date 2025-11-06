#include <oraccsys.h>
#include <locale.h>
#include <oracclocale.h>
#include <runexpat.h>
#include "bx.h"

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
	  "fFkKrR"      /* mode args */
	  "b:c:"        /* input args */
	  "d:h:io:x:"    /* output args */ 
	  "D:npqstv"    /* adjunct args */
	  );

  mesg_init();
  b.mem = list_create(LIST_SINGLE);
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
    case 'F':
      b.icfonly = 1;
    case 'f':
      b.mode = BX_ICF;
      break;
    case 'K':
      b.keys_from_bib = 1;
      break;
    case 'k':
      b.mode = BX_KEY;
      break;
    case 'R':
      b.bibonly = 1;
    case 'r':
      b.mode = BX_REF;
      break;
    case 'b':
      b.flist_bib = arg;
      break;
    case 'c':
      b.flist_cit = arg;
      break;
    case 'C':
      b.citations_file = arg;
      break;
    case 'D':
      if (strstr(arg, "people"))
	b.dbs |= BX_DB_PEOPLE;
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
    case 's':
      b.sort = 1;
      break;
    case 't':
      cite_type = arg;
      break;
    case 'v':
      ++verbose;
      break;
    case 'd':
      b.htmloutput = 1;
      b.html_mode = BX_HTML_DIV;
      b.outfile = arg;
      break;
    case 'h':
      b.htmloutput = 1;
      b.html_mode = BX_HTML_PAGE;
      b.outfile = arg;
      break;
    case 'x':
      b.xmloutput = 1;
      b.outfile = arg;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void) {}
