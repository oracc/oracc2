#include <oraccsys.h>
#include <mesg.h>
#include <tree.h>
#include <joxer.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <xml.h>
#include <lng.h>
#include <atf.h>
#include <gdl.h>
#include "ax.h"

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

int odt_serial = 0;

extern int atfflextrace , atftrace, gdlflextrace, gdltrace;

int bootstrap_mode = 0;
int exit_status = 0;
int lem_autolem = 0;
int fuzzy_aliasing = 0;
int harvest_notices, lem_dynalem = 0, lem_forms_raw = 0, lem_props_yes = 0,
  lem_props_strict = 0, line_is_unit = 0, named_ents = 0, perform_dsa = 0,
  warn_unlemmatized = 0;

FILE *f_log, *f_unlemm;
int check_mode = 0;
int trace_mode = 0;
int xcl_output = 0;
int xml_output = 0;

extern struct catinfo *ax_check (const char *str,size_t len);

void
ax_input(const char *f)
{
  mesg_init();
  langtag_init();
  nl_init();
  ngramify_init();
  gdlparse_init();
  Run *rp = run_init();
  Tree *tp = atf_read(f);
  proj_init(rp, (ccp)atfp->project);
  if (tp)
    {
      List *ap = NULL;
      if (xcl_output)
	{
	  XCL *xp = ax_xcl(rp, tp->root->kids);
	  xp->utype = N_U_XCL;
	  tree_curr(tp->root->kids);
	  Node *np = tree_add(tp, NS_XCL, "xcl:xcl", tp->root->kids->depth, tp->root->kids->mloc);
	  np->user = xp;
	  ax_lem(rp, xp);
	  ap = xcl_jox_xcl_ratts(xp);
	  const char **apcc = (const char **)list2array(ap);
	  int i;
	  for (i = 0; i < list_len(ap); i+=2)
	    atf_xprop(np, apcc[i], apcc[i+1]);
	}

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

  f_log = stderr;
  
  gdl_flex_debug = gdldebug = 0;

  options(argc, argv, "cltx");

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
    case 'l':
      xcl_output = xml_output = 1;
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
