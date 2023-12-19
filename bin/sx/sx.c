#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <mesg.h>
#include <tree.h>
#include <help.h>
#include <json.h>
#include <xml.h>
#include <asl.h>
#include <ns-asl.h>
#include <gdl.h>
#include <joxer.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include "sx.h"

Mloc *xo_loc;
FILE *f_xml;

const char *file;
int quiet, verbose;
int asltrace,rnvtrace;

int status = 0; /* for rnc; should be in library there */

int akas_dump = 0;
int asl_output = 0;
int dump_stdout = 0;
int identity_mode = 0;
int images_dump = 0;
int jsn_output = 0;
int list_dump = 0;
int list_names_mode = 0;
int listdef_check = 0;
int oid_list = 0;
int oid_tab = 0;
int sll_output = 0;
int sortcode_output = 0;
int syss_dump = 0;
int tree_output = 0;
int unicode_table = 0;
int xml_output = 0;
int validate = 1;

extern int asl_raw_tokens; /* ask asl to produce list of @sign/@form/@v tokens */
extern int ctrace;

int boot_mode = 0;
int check_mode = 0;
int trace_mode = 0;
extern int asl_flex_debug, gdl_flex_debug, gdl_unicode;

const char *missing_lists = NULL;

const char *jfn = NULL, *xfn = NULL;

int
main(int argc, char * const*argv)
{
  struct sl_signlist *sl;
  FILE *sllout = stdout;
  
  xo_loc = malloc(sizeof(Mloc));
  mesg_init();
  asl_flex_debug = gdl_flex_debug = 0;
  gdl_unicode = 1;

  gsort_init();
  
  options(argc, argv, "abcCD:d:ijJ:m:nMoOsStTuxX:?");
  asltrace = asl_flex_debug = trace_mode;

  if (boot_mode)
    {
      sll_output = 1;
      file = "00lib/ogsl.asl";
      if (!freopen(file, "r", stdin))
	{
	  fprintf(stderr, "sx: unable to read from %s\n", file);
	  exit(1);
	}
      const char *outfile = "02pub/sl/sl.tsv"; /* FIXME: should be @@ORACC@@/ogsl ... */
      if (!(sllout = fopen(outfile, "w")))
	{
	  fprintf(stderr, "sx: unable to write to %s\n", outfile);
	  exit(1);
	}
    }
  else
    {
      if (argv[optind])
	{
	  file = argv[optind];
	  if (!freopen(file, "r", stdin))
	    {
	      fprintf(stderr, "sx: unable to read from %s\n", file);
	      exit(1);
	    }
	}
    }
  
  mesg_init();
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  gdl_init();
  asl_init();
  sl = aslyacc(file);

  if (sl)
    {
      if (list_names_mode)
	{
	  sx_list_dump(stdout, sl);
	  exit(0);
	}

      sx_marshall(sl);

      if (unicode_table)
	{
	  FILE *f = stdout; /*fopen("sx-unicode.tab","w");*/
	  if (f)
	    {
	      sx_unicode_table(f, sl);
	      fclose(f);
	    }
	  else
	    fprintf(stderr, "sx: unable to dump @list data; can't write sx-listdata.out\n");
	}
      if (akas_dump)
	{
	  FILE *lfp = dump_stdout ? stdout : fopen("sx-akas.out","w");
	  if (lfp)
	    {
	      sx_akas_dump(lfp, sl);
	      if (!dump_stdout)
		fclose(lfp);
	      if (!dump_stdout && !quiet)
		fprintf(stderr, "sx: aka data written to sx-akas.out\n");
	    }
	  else
	    fprintf(stderr, "sx: unable to dump @aka data; can't write sx-akas.out\n");
	}
      if (images_dump)
	{
	  FILE *lfp = dump_stdout ? stdout : fopen("sx-images.out","w");
	  if (lfp)
	    {
	      sx_images_dump(lfp, sl);
	      if (!dump_stdout)
		fclose(lfp);
	      if (!dump_stdout && !quiet)
		fprintf(stderr, "sx: image data written to sx-images.out\n");
	    }
	  else
	    fprintf(stderr, "sx: unable to dump @image data; can't write sx-images.out\n");
	}
      if (list_dump)
	{
	  FILE *lfp = dump_stdout ? stdout : fopen("sx-lists.out","w");
	  if (lfp)
	    {
	      sx_list_dump(lfp, sl);
	      if (!dump_stdout)
		fclose(lfp);
	      if (!dump_stdout && !quiet)
		fprintf(stderr, "sx: list data written to sx-lists.out\n");
	    }
	  else
	    fprintf(stderr, "sx: unable to dump list data; can't write sx-lists.out\n");
	}
      if (syss_dump)
	{
	  FILE *lfp = dump_stdout ? stdout : fopen("sx-syss.out","w");
	  if (lfp)
	    {
	      sx_syss_dump(lfp, sl);
	      if (!dump_stdout)
		fclose(lfp);
	      if (!dump_stdout && !quiet)
		fprintf(stderr, "sx: sys data written to sx-syss.out\n");
	    }
	  else
	    fprintf(stderr, "sx: unable to dump @sys data; can't write sx-syss.out\n");
	}

      if (sortcode_output)
	sx_sortcodes(sl);
      
      if (listdef_check && !list_dump)
	sx_listdefs(sl, missing_lists);

      if (oid_list)
	sx_oid_list(sl);
      
      if (oid_tab)
	sx_oid_tab(sl);
      
      if (asl_output)
	sx_walk(sx_w_asl_init(stdout, "-"), sl);

      if (sll_output)
	sx_s_sll(sllout, sl);

      if (!sll_output && (validate || xml_output || jsn_output))
	{
	  if (!jfn)
	    jfn = "sl.jsn";
	  if (!xfn)
	    xfn = "sl.xml";
	  FILE *jfp = jsn_output ? fopen(jfn, "w") : NULL;
	  FILE *xfp = xml_output ? fopen(xfn, "w") : NULL;

	  jox_jsn_output(jfp);
	  jox_xml_output(xfp);
	  joxer_init(&asl_data, "asl", validate, xfp, jfp);	  
	  sx_walk(sx_w_jox_init(), sl);
	  joxer_term(xfp,jfp);
	}
    }
  
  gdl_term();
  asl_term();
  asl_bld_term(sl);
  mesg_print(stderr);
  return mesg_status();
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'a':
      asl_output = 1;
      break;
    case 'b':
      boot_mode = 1; /* write sll output to 02pub/sl/sl.tsv */
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'C':
      ctrace = 1;
      break;
    case 'D':
      dump_stdout = 1;
      /*falls through*/
    case 'd':
      if (strstr(arg, "aka"))
	akas_dump = 1;
      if (strstr(arg, "image"))
	images_dump = 1;
      if (strstr(arg, "list"))
	list_dump = 1;
      if (strstr(arg, "sys"))
	syss_dump = 1;
      if (!akas_dump && !list_dump && !images_dump && !syss_dump)
	fprintf(stderr, "sx: the -d option must contain any or all of 'aka,list,image,sys'\n");
      break;
    case 'i':
      asl_output = identity_mode = 1;
      break;
    case 'J':
      jfn = arg;
    case 'j':
      jsn_output = 1;
      break;
    case 'n':
      list_names_mode = 1;
      break;
    case 'm':
      listdef_check = 1;
      missing_lists = arg;
      break;
    case 'M':
      listdef_check = 1;
      missing_lists = NULL;
      break;
    case 'O':
      oid_tab = 1;
      break;
    case 'o':
      ++oid_list;
      break;
    case 'q':
      quiet = 1;
      break;
    case 's':
      sll_output = 1;
      break;
    case 'S':
      sortcode_output = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'u':
      unicode_table = 1;
      break;
    case 'X':
      xfn = arg;
    case 'x':
      xml_output = 1;
      break;
    case 'v':
      validate = 1;
      break;
    case 'V':
      validate = 0;
      break;
    case '?':
      help();
      exit(1);
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
  help_longest_opt("m [LIST]");
  help_title("sx: The Oracc signlist processor for .asl files");
  help_usage("  Usage: sx [OPTIONS] [ASL-FILE]");

  help_heading("Mode Options");
  help_option("b", "boot-mode: write signlist data output to 02pub/sl/sl.tsv");
  help_option("c", "check-mode: check the signlist and exit");
  help_option("C", "ctrace-mode: trace compound processing for debugging purposes");
  help_option("i", "identity-mode: produce 'identity' .asl output.\n"
	      	   "\t\tOutput may be re-sorted and/or have new @ucun tags.");
  help_option("t", "trace-mode: turn on tracing for debugging purposes");

  help_heading("Output Options");
  help_option("", "(All the following outputs are written to stdout)\n");
  help_option("a", "asl-output: this adds @letter and @group tags. See also -i.");
  help_option("j", "json-output: a JSON version of the signlist; beta");
  help_option("o[o]", "oid-output: two-column table of OID and sign/form names.\n"
	      	     "\t\tWith 'oo' dump all OIDs, with 'o' omit OIDs with uage=0.");
  help_option("O", "oid-index.tab output for use with the OID resolver\n");
  help_option("s", "sll-output: data for the Sign-List-Library, sll, also used by GVL");
  help_option("S", "Sortcode-output: show a list of OIDs and sort-codes");
  help_option("x", "xml-output: an XML version of the signlist");
  
  help_heading("List and Coverage Options");
  help_option("", "(All the following outputs are written to stdout)\n");
  help_option("d", "aka|image|list|sys: show all aka/images/lists/sys entries in signlist");
  help_option("M", "missing-all: show missing entry information for all lists");
  help_option("m [LIST]", "missing [LIST]: show missing entry information for the LIST, e.g., -m MZL");
  help_option("n", "names-of-lists: show list -names defined in signlist");
  help_option("u", "unicode: show a Unicode coverage data");
  help_str("",0);
}
