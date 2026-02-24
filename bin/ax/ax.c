#include <oraccsys.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <atf.h>
#include <gdl.h>
#include "ax.h"

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int atfflextrace , atftrace, gdlflextrace, gdltrace;

int check_mode = 0;
int trace_mode = 0;
int xml_output = 0;

extern struct catinfo *ax_check (const char *str,size_t len);

void
ax_input(const char *f)
{
  mesg_init();
  gdlparse_init();
  Tree *tp = atf_read(f);
  if (tp)
    {
      if (xml_output)
	ax_jox(tp);
      else
	ax_atf(atfmp->atf);
      atf_term();
      gdlparse_term();
      mesg_print(stderr);
      mesg_term();
    }
}

int
main(int argc, char **argv)
{
  static int multifile = 0;
  extern int gdl_flex_debug, gdldebug; /* yydebug in gdl.y */
  
  gdl_flex_debug = gdldebug = 0;

  options(argc, argv, "ctx");

  atfflextrace = atftrace = gdlflextrace = gdltrace = gdldebug = trace_mode;

  gdlxml_setup();
  gvl_setup("osl", "osl","020");
  nodeh_register(treexml_o_handlers, NS_XTF, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_XTF, treexml_c_generic);

  if (argv[optind])
    {
      int fnum = optind;
      multifile = argv[fnum] && argv[fnum+1];
      if (multifile && !check_mode)
	printf("<xtf-multi>");
      while (argv[fnum])
	ax_input(argv[fnum++]);
      if (multifile && !check_mode)
	printf("</xtf-multi>");
    }
  else
    ax_input(NULL);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'c':
      check_mode = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'x':
      xml_output = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "ax: read input from stdin; use -c to check only\n");
}
