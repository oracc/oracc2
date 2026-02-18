#include <oraccsys.h>
#include <c2types.h>
#include <iome.h>
#include <pool.h>
#include <tree.h>
#include <lng.h>
#include <gdl.h>
#include <gt.h>
#include <cbd.h>
#include <xnn.h>
#include <ns-cbd.h>
#include <cbdyacc.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <joxer.h>
#include <kis.h>
#include "gx.h"

int bootstrap_mode, lem_autolem, lem_dynalem;
static int major_version = 1, minor_version = 0;
static const char *project = NULL;
static const char *prog = "gx";
static const char *usage_string = "[OPTIONS] [-I input-type] [-O output-type] -i <FILE|-> -o <FILE|->";

const char *jfn = NULL, *xfn = NULL;

const char *kis_file = "02pub/tokl.kis";

extern int cbd_flex_debug;

int jsn_output = 0, xml_output = 1;

const char *log_file;
FILE *log_fp;

#ifdef __APPLE__
int cbddebug;
#else
extern int cbddebug;
#endif

static const char *default_input_method = "tg1";

extern Iome *iomethod(const char *str, size_t len);
static Iome *input_method, *output_method;
static Iome_io  input_io, output_io;
const char *input_file, *output_file;

Pool *cbd_pool = NULL;
Tree *cbd_tree = NULL;

/* Different error mechanisms in libraries need to be unified */
const char *errmsg_fn = NULL;
const char *efile = NULL;

/* Hash *cbds = NULL; */ /* defined in nss/cbd/cbdyacc.c */

int check = 0;
int keys = 0;
int trace_mode = 0;
int rnvtrace = 0;
int keepgoing = 0;
int input_validation = 0;
int lnum = 0;
int math_mode = 0;
int output_validation = 1;
int parser_status = 0;
int sigs = 0;
int status = 0;
int stdin_input = 0;
int stdin_output = 0;
int summaries = 0;
int verbose = 0;

const char *file = NULL;
FILE *f_log, *f_xml;
int no_pi = 0;
int do_cuneify = 0;
int use_unicode = 1;
int with_textid = 0;

struct cbd* curr_cbd;
struct entry*curr_entry;

extern int parser_status;

extern int cbdparse(void);
extern void cbdrestart(FILE*);

extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

static void gx_init(void);
static void gx_term(void);
static void io_init(void);

static void
gx_init(void)
{
  f_log = stderr;
  math_mode = no_pi = do_cuneify = use_unicode = 1;
  common_init();
  cbd_pool = pool_init();
  tree_init();
  mesg_init();
#if 1
  Hash *htokens = hash_create(1024);
  Memo *mtokens = memo_init(sizeof(Gt), 1024);
  gdl_unicode = 1;
  gdl_init();
  gsort_init();
  gt_config(htokens, mtokens);
#else
  gvl_setup("osl","osl","020");
#endif
  lng_init();
  curr_lang_ctxt = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  cbds = hash_create(1);
  with_textid = 0;
}

#if 0
static void
gx_run()
{
  if (file)
    {
      FILE *fp;
      efile = errmsg_fn = file;
      if ((fp = xfopen(efile, "r")))
	tg1restart(fp);
    }
  curr_cbd = bld_cbd();
  phase = "syn";
  if (tg1parse() || parser_status)
    {
      if (!keepgoing)
	{
	  mesg_print(stderr);
	  fprintf(stderr, "gx: exiting after syntax errors\n");
	  exit(1);
	}
    }

  if (check || output)
    validator(curr_cbd);
  
  if (identity_output)
    identity(curr_cbd);

  if (xml_output)
    {
      extern void rnvxml_rnvif_init(void);
      int rnvok = -1;
      rnvxml_rnvif_init();
      rnvif_init();
      rnv_validate_start();      
      xmloutput(curr_cbd);
      rnvok = rnv_validate_finish();
      rnvif_term();
      if (verbose)
	fprintf(stderr, "rnv returned %d\n", rnvok);
    }
  
  mesg_print(stderr);
}
#endif

static void
gx_term(void)
{
  lng_term();
  gdlparse_term();
  mesg_term();
  pool_term(cbd_pool);
  /*tree_term(cbd_tree);*/
  common_term();
  /*treemem_term();*/
}

static void
io_init(void)
{
  if (!input_method)
    input_method = iomethod(default_input_method, strlen(default_input_method));

  if (!check && !output_method)
    output_method = iomethod(default_input_method, strlen(default_input_method));

  if (output_method)
    {
      input_validation = 0;
      output_validation = 1;
    }
  else
    {
      input_validation = 1;
      output_validation = 0;
    }
  
  memset(&input_io, '\0', sizeof(Iome_io ));
  memset(&output_io, '\0', sizeof(Iome_io ));

  if (!input_file)
    input_file = "-";
  if (!check && !output_file)
    output_file = "-";

  input_io.fn = input_file;
  if (!efile)
    efile = errmsg_fn = input_file;
  if (!strcmp(input_file, "-"))
    input_io.use_stdio = 1;

  if (output_file)
    {
      output_io.fn = output_file;
      if (!strcmp(output_file, "-"))
	output_io.use_stdio = 1;
    }
}

static void
io_run(void)
{
  extern void cbd_l_init(Iome_io  *ip);
  extern void cbd_l_term(void);
  
  int parse_return = 0;
  
  switch (input_method->type)
    {
    case iome_cbd:
      cbd_l_init(&input_io);
      curr_cbd = cbd_bld_cbd();
      curr_cbd->file = input_io.fn;
      phase = "syn";
      parse_return = cbdparse();
      cbd_l_term();

      /*rnvtgi_term();*/

      if (parse_return || parser_status)
	{
	  mesg_print(stderr);
	  if (!keepgoing)
	    {
	      fprintf(stderr, "gx: exiting after syntax errors\n");
	      exit(1);
	    }
	}

      cbd_psus();

      if (check)
	validator(curr_cbd);

      if (keys)
	{
	  Kis *k = kis_load("02pub/tokl.kis");
	  cbd_kis(curr_cbd, k);
	}

      break;
    default:
      fprintf(stderr, "gx: %s input not supported\n", input_method->name);
      exit(1);
      break;
    }

  /* THIS IS WHERE ACTIONS WILL BE EXECUTED */

  if (output_method)
    {
      switch (output_method->type)
	{
	case iome_cbd:
	  /*identity(curr_cbd);*/
	  fprintf(stderr, "gx: tg1 output request should use tg2\n");
	  exit(1);
	  break;
	case iome_x11:
	case iome_x12:
	case iome_x21:
	case iome_x22:
	default:
	  fprintf(stderr, "gx: %s output not supported\n", output_method->name);
	  break;
	}
    }
}

void
gx_output(void)
{
  gt_codes();

  if (xml_output || jsn_output)
    {
      if (!jfn)
	jfn = "cbd.jsn";
      if (!xfn)
	xfn = "cbd.xml";

      FILE *jfp = jsn_output ? fopen(jfn, "w") : NULL;
      FILE *xfp = xml_output ? fopen(xfn, "w") : NULL;

      if (xml_output)
	{
	  jox_xml_output(xfp);
	  joxer_init(&cbd_data, "cbd", 1, xfp, NULL);
	  o_jox(curr_cbd);
	  joxer_term(xfp,NULL);
	}

      if (jsn_output)
	{
	  jox_jsn_output(jfp);
	  joxer_init(&cbd_data, "cbd", 0, NULL, jfp);
	  o_jsn(curr_cbd);
	  joxer_term(xfp,NULL);
	}
    }

  if (summaries)
    gx_summaries(curr_cbd);
}

int
main(int argc, char **argv)
{
  extern void cbdset_debug(int);
  extern int gdl_flex_debug, gdldebug;
  program_values(prog, major_version, minor_version, usage_string, NULL);
  status = 0;
  options(argc,argv,"A:I:O:i:o:chjJkK::l::p:rsStTxXv");

  if (status)
    {
      fprintf(stderr, "gx: quitting after errors in option processing\n");
      exit(1);
    }

  if (log_file)
    {
      log_fp = xfopen(log_file, "w");
      if (log_fp)
	cbd_log(log_fp);
      else
	exit(1);
    }
  
  if (argv[optind] && !input_file)
    input_file = argv[optind];
  
  gdl_flex_debug = gdldebug = cbd_flex_debug = cbddebug = trace_mode;
  gdl_word_mode = 1;
  
  cbdset_debug(trace_mode);

  mesg_init();
  
  gx_init();

  io_init();

  if (!input_method)
    input_method = iomethod("cbd", 3);

  io_run();

  gx_output();
  
  gx_term();

  return 1;
}

void help(void)
{
  fprintf(stderr, "OPTIONS:\n\n");
  fprintf(stderr, "\t-c\tcheck input and exit without creating output\n");
  fprintf(stderr, "\t-d\tdebug mode\n");
  fprintf(stderr, "\t-e\tset file name to use in errors\n");
  fprintf(stderr, "\t-k\tkeep going despite errors\n");
  fprintf(stderr, "\t-t\ttrace tokenizing\n");
  fprintf(stderr, "\t-T\ttrace TGI validation\n");
  fprintf(stderr, "\t-v\tverbose mode\n");
  fprintf(stderr, "\n\t-A [ACTIONS]\n\n");
  fprintf(stderr, "ACTIONS:\n\n");
  fprintf(stderr, "\t(none yet)\n\n");
  exit(0);
}
int opts(int och, const char *oarg)
{
  switch (och)
    {
    case 'I':
      if (!(input_method = iomethod(optarg, strlen(optarg))))
	{
	  fprintf(stderr, "gx: unknown input method: %s\n", optarg);
	  status = 1;
	}
      break;
    case 'O':
      if (!(output_method = iomethod(optarg, strlen(optarg))))
	{
	  fprintf(stderr, "gx: unknown output method: %s\n", optarg);
	  status = 1;
	}	
      break;
    case 'a':
      break;
    case 'b':
      break;
    case 'c':
      check = 1;
      break;
    case 'd':
      cbddebug = 1; /* = tg2debug */
      break;
    case 'e':
      efile = errmsg_fn = optarg;
      break;
    case 'f':
      break;
    case 'g':
      break;
    case 'h':
      usage();
      break;
    case 'i':
      input_file = optarg;
      break;
    case 'J':
      xml_output = 0;
    case 'j':
      jsn_output = 1;
      break;
    case 'k':
      keepgoing = 1;
      break;
    case 'K':
      keys = 1;
      if (optarg)
	kis_file = optarg;
      break;
    case 'l':
      if (optarg)
	log_file = optarg;
      else
	log_file = "gx.log";
      break;
    case 'n':
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'p':
      project = optarg;
      break;
    case 'r':
      rnvtrace = 1;
      break;
    case 's':
      sigs = 1;
      break;
    case 'S':
      summaries = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'T':
      rnvtrace = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'X':
      jsn_output = 0;
    case 'x':
      xml_output = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

#if 0
void
cbd_rnc_init(void)
{
  if (xml_output)
    {
#if 0      
      extern int cbdrnc_len;
      char *cbd = cbdrnc();
      rnc_start = rnl_s("ORACC_SCHEMA/cbd.rnc",cbd,cbdrnc_len);
      status = !rnc_start;
#endif
    }
  else if (output_method)
    {
      char fn[12];
      sprintf(fn, "%s.rnc", output_method->name);
      if (!xaccess(fn, R_OK, 0))
	{
	  fprintf(stderr, "gx: found output method schema %s\n", fn);
	  rnc_start = rnl_fn(fn);
	  status = !rnc_start;
	}
      else
	{
	  fprintf(stderr, "gx: no such output method schema %s\n", fn);
	}
    }
}
#endif
