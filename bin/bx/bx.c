#include <oraccsys.h>
#include <locale.h>
#include <oracclocale.h>
#include <runexpat.h>
#include "bx.h"

Bx b = {
  .mode=BX_CIT ,
  .pre[BX_BID] = bx_bid_pre,
  .pre[BX_CIT] = bx_cit_pre,
  .pre[BX_ICF] = bx_icf_pre,
  .pre[BX_KEY] = bx_key_pre,
  .pre[BX_REF] = bx_ref_pre,
  .run[BX_BID] = bx_bid_run,
  .run[BX_CIT] = bx_cit_run,
  .run[BX_ICF] = bx_icf_run,
  .run[BX_KEY] = bx_key_run,
  .run[BX_REF] = bx_ref_run,
  .out[BX_BID] = bx_bid_out,
  .out[BX_CIT] = bx_cit_out,
  .out[BX_ICF] = bx_icf_out,
  .out[BX_KEY] = bx_key_out,
  .out[BX_REF] = bx_ref_out
};

extern int optind;

const char *bibsfile = NULL; /* file of paths to .bib files */
const char *cite_type = NULL;
int overwrite = 0, verbose = 0;

int
main(int argc, char * const*argv)
{
  setlocale (LC_ALL, ORACC_LOCALE);
  b.argv = argv;

  b.mode = BX_TOP;
  
  options(argc, argv,
	  "fFiIkKrR"     /* mode args */
	  "b:c:"        /* input args */
	  "d:h:l:no:x:" /* output args */ 
	  "D:LOpqsStv"   /* adjunct args */
	  );

  if (BX_TOP == b.mode)
    {
      b.mode = BX_REF;
      b.bibonly = 1;
    }
  
  mesg_init();
  b.mem = list_create(LIST_SINGLE);
  b.p = pool_init();

  b.pre[b.mode](&b);
  b.run[b.mode](&b);
  b.out[b.mode](&b);

  mesg_print(stderr);
}

static void
bx_output_check(const char *file, const char *mode)
{
  if (!overwrite && !access(file, R_OK))
    {
      fprintf(stderr, "bx: %s output file `%s' already exists; remove it or use -O\n",
	      mode, file);
      exit(1);
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'F':
      b.icfonly = 1;
      /*falls through*/
    case 'f':
      b.mode = BX_ICF;
      break;
    case 'I':
      b.bid_mode = BX_BID_IDENTITY;
      /*falls through*/
    case 'i':
      b.mode = BX_BID;
      b.bibonly = 1;
      break;
    case 'K':
      b.keys_from_bib = 1;
      break;
    case 'k':
      b.mode = BX_KEY;
      break;
    case 'R':
      b.bibonly = 1;
      /*falls through*/
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
    case 'L':
      b.bid_mode = BX_BID_LAST;
      b.mode = BX_BID;
      break;
    case 'l':
      b.reffile = arg;
      break;
    case 'n':
      b.no_output = 1;
      break;
    case 'O':
      overwrite = 1;
      break;
    case 'o':
      bx_output_check(arg, "default");
      b.outfile = arg; /* in keys_mode a file; in dotbib_mode a .bib file */
      break;
    case 'p':
      b.project = arg;
      break;
    case 'q':
      b.quiet = 1;
      break;
    case 'S':
      b.sortfields = 1;
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
      bx_output_check(arg, "div");
      b.htmloutput = 1;
      b.html_mode = BX_HTML_DIV;
      b.outfile = arg;
      b.sort = 1;
      break;
    case 'h':
      bx_output_check(arg, "html");
      b.htmloutput = 1;
      b.html_mode = BX_HTML_PAGE;
      b.outfile = arg;
      b.sort = 1;
      break;
    case 'x':
      bx_output_check(arg, "xml");
      b.xmloutput = 1;
      b.outfile = arg;
      b.sort = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void) {}
