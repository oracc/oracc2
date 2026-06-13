#include <oraccsys.h>
#include <mesg.h>
#include <tree.h>
#include <joxer.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <xml.h>
#include <gvl.h>
#include <lng.h>
#include <xmd.h>
#include <atf.h>
#include <gdl.h>
#include <inl.h>
#include <l3props.h>
#include "ax.h"

extern int xmd_ignore_missing;

char *trafile, *xtffile;
int val_flag = 0;

Mloc xo_loc;
FILE *f_xml, *ok_no_ok_fp, *ok_no_no_fp;
const char *file;
int links_standalone = 0, links_wordrefs = 1;
int ok_no_files = 0;
int status;
int rnvtrace;
int verbose;

int odt_serial = 0;

static Run *rp;
const char *inputs_from_file = NULL;

extern int atfflextrace , atftrace, gdlflextrace, gdltrace;

int bootstrap_mode = 0;
int exit_status = 0;
int lem_autolem = 0;
int memuse_mode = 0;
int fuzzy_aliasing = 0;
int harvest_notices, lem_dynalem = 0, lem_forms_raw = 0, lem_props_yes = 0,
  lem_props_strict = 0, line_is_unit = 0, named_ents = 0, perform_dsa = 0,
  warn_unlemmatized = 0;

FILE *f_log, *f_unlemm;
int check_mode = 0;
int lem_mode = 0;
int trace_mode = 0;
int xcl_output = 0;
int xml_output = 0;

extern struct catinfo *ax_check (const char *str,size_t len);

void
ax_input(const char *f)
{
  xmd_ignore_missing = 1;
  mesg_init();
  lng_init();
  form_init();
  nl_init();
  ngramify_init();
  gdlparse_init();
  inl_init(0);
  Tree *tp = atf_read(f);
  proj_init(rp, (ccp)atfp->project);
  XCL *xp = NULL;
  List *ap = NULL;
  const char **apcc = NULL;
  if (tp)
    {
      if (lem_mode)
	{
	  xp = ax_xcl(rp, tp->root->kids);
	  xp->utype = N_U_XCL;
	  tree_curr(tp->root->kids);
	  Node *np = tree_add(tp, NS_XCL, "xcl", tp->root->kids->depth, tp->root->kids->mloc);
	  np->user = xp;
	  ax_lem(rp, xp);
	  if (xcl_output)
	    {
	      ap = xcl_jox_xcl_ratts(xp);
	      apcc = (const char **)list2array(ap);
	      int i;
	      for (i = 0; i < list_len(ap); i+=2)
		atf_xprop(np, apcc[i], apcc[i+1]);
	    }
	}

      if (!check_mode)
	{
	  if (xml_output)
	    ax_jox(tp);
	  else
	    ax_atf(atfmp->atf);
	}

      if (xp)
	{
	  xcl_destroy(&xp);
	  list_free(ap, NULL);
	  free((char*)apcc);
	}

      if (ok_no_files)
	fprintf(status ? ok_no_no_fp : ok_no_ok_fp, "%s\n", f);
      
      if (status)
	{
	  exit_status = status;
	  status = 0;
	}

      atf_term();
      rnvval_term();
      xmd_term();
      inl_term();
      gdlparse_term();
      mesg_print(stderr);
      mesg_term();
      inl_term();
    }
}

/* This is where major components like GVL/SLL and lemmatizer sig sets
   get unloaded after all files have been processed. */
static void
ax_full_term(void)
{
  gvl_wrapup("osl");
  sll_term_t(sll_sl);
  sig_context_term();
  utf2wcs(NULL,NULL);
  ngramify_term();
  nl_term();
  form_term();
  props_run_term();
  run_term(rp);
  xcl_final_term();
  lng_term();
}

#if 0
static int
pqx_ok(const char *s)
{
  const char *t = s;
  if ('P' == *s || 'Q' == *s || 'X' == *s)
    {
      ++s;
      while (*s)
	{
	  if (isdigit(*s) && s - t < 8)
	    ++s;
	  else
	    return 0;
	}
    }
  return s - t == 7;
}
#endif

static char *
atffile_of(const char *d)
{
  const char *fn = expand(NULL, d, "atf");
  if (fn)
    return strdup(fn);
  else
    {
      fprintf(stderr, "ax: invalid ATF path `%s': must end with a valid PQX-id\n", d);
      return NULL;
    }
}

static char *
trafile_of(const char *d)
{
  const char *fn = expand_xtr(NULL, d, "project", "en");
  return strdup(fn);
}

static char *
xtffile_of(const char *d)
{
  const char *fn = expand(NULL, d, "xtf");
  return strdup(fn);
}

static void
process_inputs(int argc, char * const *argv)
{
  if (inputs_from_file)
    {
      unsigned char **files = NULL, *fmem;
      size_t nfiles = 0, i;
      files = loadfile_lines3((uccp)inputs_from_file,&nfiles,&fmem);
      for (i = 0; i < nfiles; ++i)
	{
	  char *s = NULL;
	  for (s = (char*)files[i]; *s; ++s)
	    if (!isspace(*s))
	      break;
	  if (*s)
	    {
	      int ret = 0;
	      atffile = atffile_of(s);
	      if (atffile)
		{
		  if (verbose)
		    fprintf(stderr,"%s\n",s);
		  if (check_mode)
		    {
		      ax_input(atffile);
		      fflush(stdout); /* in case flex did some default output */
		      free((char*)atffile);
		    }
		  else
		    {
		      xtffile = xtffile_of(s);
		      trafile = trafile_of(s);
		      if (!(ret = ax_outputs(xtffile, trafile)))
			ax_input(atffile);
		      fflush(stdout); /* in case flex did some default output */
		      free((char*)atffile);
		      free(trafile);
		      free(xtffile);
		      atffile = trafile = xtffile = /*cdtfile = */ NULL;
		    }
		  if (memuse_mode)
		    show_current_memory(stderr, NULL, NULL);
		}
	      else
		ret = 1;
	      
	      if (ret)
		{
		  if (!atffile)
		    fprintf(stderr, "ax: error setting atf file from %s. Stop.\n", files[i]);
		  else
		    fprintf(stderr, "ax: error setting outputs. Stop.\n");
		  break;
		}
	    }
	}
      free(files);
      free(fmem);
    }
  else if (argv[optind])
    {
      static int multifile = 0;
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
main(int argc, char * const*argv)
{
  extern int gdl_flex_debug, gdldebug, atfdebug, atf_flex_debug; /* yydebug in gdl.y */

  f_log = stderr;
  
  gdl_flex_debug = gdldebug = 0;

  options(argc, argv, "ACDcgI:Llmtvx");

  /* -l sets xcl_output and xml_output so this ensures that -cl doesn't do output */
  if (check_mode)
    xml_output = xcl_output = 0;

  if (ok_no_files)
    {
      ok_no_ok_fp = xfopen("ax.ok", "w");
      ok_no_no_fp = xfopen("ax.no", "w");
      if (!ok_no_ok_fp || !ok_no_no_fp)
	exit(1);
    }

  inl_set_ns(NS_HTM);
  
  atf_flex_debug = atfflextrace = atftrace = atfdebug = trace_mode;

  if (trace_mode > 1)
    gdlflextrace = gdltrace = gdldebug = 1;

  rp = run_init();
  gdlxml_setup();
  gvl_setup("osl", "osl","020");
  nodeh_register(treexml_o_handlers, NS_XTF, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_XTF, treexml_c_generic);
  langtag_init();

  /* lang_switch uses memo_auto so for the global_lang we clone it into persistent mem */
  struct lang_context *x = lang_switch(NULL,"sux",NULL,NULL,0);
  global_lang = calloc(1, sizeof(struct lang_context));
  *global_lang = *x;
  
  process_inputs(argc, argv);
  
  ax_full_term();
  free(global_lang);
  if (ok_no_files)
    {
      xfclose("ax.ok", ok_no_ok_fp);
      xfclose("ax.no", ok_no_no_fp);
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'A':
      amp_trace = 1;
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'C':
      xcl_output = xml_output = 1;
      break;
    case 'D':
      /* future use with morph analyzer */
      break;
    case 'g':
      ok_no_files = 1;
      break;
    case 'I':
      inputs_from_file = optarg;
      break;
    case 'L':
      line_trace = 1;
      break;
    case 'l':
      if (lem_mode++)
	{
	  l3verbose = 1; /* say -ll to get lem tracing */
	  extern int sigsets_debug;
	  if (lem_mode++ >= 2)
	    sigsets_debug = 1;
	}
       lem_mode = xcl_output = xml_output = 1;
      break;
    case 'm':
      if (i_am_linux)
	memuse_mode = 1;
      break;
    case 't':
      ++trace_mode;
      break;
    case 'V':
      val_flag = 1;
      break;
    case 'v':
      ++verbose;
      if (verbose > 1)
	{
	  extern int l3verbose;
	  l3verbose = 1;
	}
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
