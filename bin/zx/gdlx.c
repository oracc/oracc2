#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <tree.h>
#include <xml.h>
#include <xnn.h>
#include <ns-gdl.h>
#include <gdl.h>
#include <ns-asl.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <gvl.h>
#include <gsort.h>

/* test harness for gvl/gdl libraries */

static Mloc ml;

Hash *keyhash = NULL;
Pool *keypool = NULL;

Mloc xo_loc;
FILE *f_xml;
const char *file;
int key = 0;
int verbose = 0;
int status;
int rnvtrace;

extern int gdl_flex_debug, gdldebug, gdl_orig_mode, gdl_unicode;

int bare_mode = 0;
int check_mode = 0;
int cuneify_mode = 0;
int gdlseq_mode = 0;
int gdlseq_oid_mode = 0;
extern int gdlsig_depth_mode;
int error_stdout = 0;
const char *fname = NULL;
int gdl_c10e_mode = 1;
int gdl_one_off_mode = 0;
int gsort_mode = 0;
const char *gvl_script_type = NULL;
int identity_mode = 0;
int json_mode = 0;
int ns_output = 0;
int pedantic = 0;
const char *project = "osl"; /* use the signlist found in this project */
int rnv_xml = 0;
int signatures = 0;
int trace_mode = 0;
int uname_mode = 0;
int validate = 0;
int wrapper = 0;

FILE *fp;
List *gslp;

static void
test_identity(char *s, Tree *t)
{
  const char *res = NULL;
  if (t->root && t->root->kids)
    {
      if (t->root->kids->text)
	res = t->root->kids->text;
      else
	printf("%s\t(null)\n", s);
    }

  if (res && strcmp(s, res))
    printf("%s\t%s\n", s, res);
}

void
do_one(char *s)
{
  Tree *tp = NULL;
  List *mp = NULL;
  static int saved_deep = -1;

  if (-1 == saved_deep)
    saved_deep = gdlsig_depth_mode;
  
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  if (s[strlen(s)-1] == '+')
    {
      s[strlen(s)-1] = '\0';
      saved_deep = gdlsig_depth_mode;
      gdlsig_depth_mode = 1;
    }
  ++ml.line;
  mesg_init();
  tp = gdlparse_string(&ml, s);

  if (cuneify_mode)
    {
      const unsigned char *ucun = (uccp)gvl_cuneify_tree(tp), *ivs;
      if (bare_mode)
	{
	  if (key)
	    printf("\t%s\n", ucun);
	  else
	    printf("%s\t%s\n", s, ucun);
	}
      else
	{
	  if (gvl_script_type)
	    ivs = gvl_ivs(ucun, NULL);
	  else
	    ivs = (uccp)"not done";
	  printf("ucun %s => %s => ivs %s\n", s, ucun, ivs);
	}
    }
  else if (gdlseq_mode)
    {
      unsigned const char *seq = NULL;
      unsigned char s2[strlen(s)+1];
      if (key)
	seq = gdlseq((ucp)s);
      else
	{
	  strcpy((char*)s2,s);
	  seq = gdlseq(s2);
	}
      if (gdlseq_oid_mode)
	seq = sll_oids_of(seq, "_");
      if (key)
	printf("\t%s\n", seq);
      else
	{
	  printf("%s\t%s\n", s, seq);
	}
    }
  else if (uname_mode)
    {
      if (strstr(project, "pcsl"))
	gvl_uname_prefix = "PROTO-CUNEIFORM";
      else
	gvl_uname_prefix = "CUNEIFORM";
      const unsigned char *uname = (uccp)gvl_uname_tree(tp);
      if (key)
	printf("\t%s\n", uname);
      else
	printf("%s\t%s\n", s, uname);
    }
  else
    {
      mp = mesg_retrieve();
      if (mp && list_len(mp))
	{
	  List *tmp = list_create(LIST_SINGLE);
	  unsigned const char *msg = NULL;
	  for (msg = list_first(mp); msg; msg = list_next(mp))
	    if (pedantic || (!strstr((ccp)msg, "qualified") && !strstr((ccp)msg, "unknown")))
	      list_add(tmp, (void*)msg);
	  if (list_len(tmp))
	    {
	      if (error_stdout)
		{
		  mesg_print2(stdout, tmp);
		  fflush(stdout);
		}
	      else
		mesg_print2(stderr, tmp);
	    }
	}

      if (identity_mode)
	test_identity(s, tp);
      else if (gsort_mode)
	{
	  GS_head *ghp;
	  list_add(gslp, (ghp = gsort_prep(tp)));
	  if (verbose)
	    gsort_show(ghp);
	}
      else if (!check_mode)
	{
	  if (signatures)
	    {
	      const char *sig = gdlsig(tp);
	      if (bare_mode)
		fprintf(stdout, "%s\n", sig);
	      else
		fprintf(stdout, "%s\t%s\n", s, sig);
	      fflush(stdout);
	    }
	  else if (ns_output)
	    tree_xml_rnv(stdout, tp, &gdl_data, "gdl");
	  else
	    tree_xml(stdout, tp);
	}
    }
  gdlparse_reset();
  gdlsig_depth_mode = saved_deep;
  /*tree_term(tp);*/
}

void
do_many(const char *fn)
{
  if ('-' == *fn)
    {
      fp = stdin;
      ml.file = "<stdin>";
      ml.line = 1;
    }
  else
    fp = fopen(fn, "r");
  if (fp)
    {
      char buf[1024], *s, *orig_s;
      ml.file = fn;
      ml.line = 1;
      if (wrapper)
	printf("<gdlx>");
      while ((s = fgets(buf, 1024, fp)))
	{
	  if (key)
	    {
	      s[strlen(s)-1] = '\0';
	      if (!gsort_mode)
		printf("%s", s);
	      else
		orig_s = (char*)pool_copy((uccp)s, keypool);
	      int i = 1;
	      while (i < key)
		{
		  while (*s && '\t' != *s)
		    ++s;
		  ++i;
		  if (*s)
		    ++s;
		}
	      if (*s)
		{
		  char *t = s;
		  while (*t && '\t' != *t)
		    ++t;
		  if (*t)
		    *t = '\0';
		}
	      if (gsort_mode)
		hash_add(keyhash, pool_copy((uccp)s, keypool), orig_s);
	    }
	  do_one(s);
	}
      if (wrapper)
	printf("</gdlx>");
    }
  else
    fprintf(stderr, "gdlx: file %s can't be read\n", fn);
}

int
main(int argc, char **argv)
{
  gdl_flex_debug = gdldebug = 0;
  gdl_unicode = 1;
  setlocale(LC_ALL, ORACC_LOCALE);

  options(argc, argv, "1abcCdef:gG:ik:lnop:PqQrstuUvwW");
  
  gdl_flex_debug = gdldebug = trace_mode;
  
  if (gdl_one_off_mode)
    {
      if (argv[optind])
	{
	  char *res = gdl_one_off("<cli>",1,argv[optind], 1);
	  if (res)
	    {
	      printf("%s\t%s\n", argv[optind], res);
	    }
	  else
	    {
	      fprintf(stderr, "gdlx: one-off mode returned NULL\n");
	    }
	  return 0;
	}
      else
	{
	  fprintf(stderr, "gdlx: -1 (one-off mode) requires a grapheme on the command line\n");
	  return 1;
	}
    }

  if (ns_output)
    {
      extern int xml_printing;
      xml_printing = 1;
      rnvxml_init_err();
      rnvif_init();
      rnvxml_init(&asl_data, "gdl");
      gdlxml_rnv_setup();
    }
#if 0
  else if (json_mode)
    gdljsn_setup();
#endif
  else
    gdlxml_setup();
  if (!strcmp(project, "pcsl"))
    gvl_setup(project, project, "900");
  else
    gvl_setup(project, project, "020"); /*FIXME*/
  gdlparse_init();

  if (gvl_script_type)
    gvl_set_script(gvl_script_type);

  if (gsort_mode)
    {
      keyhash = hash_create(4096);
      keypool = pool_init();
      gsort_init();
      gslp = list_create(LIST_SINGLE);
    }
  
  if (argv[optind])
    {
      ml.file = "<argv1>";
      ml.line = 1;
      do_one(argv[optind]);
    }
  else if (fname)
    do_many(fname);
  else
    do_many("-");

  if (gsort_mode)
    {
      GS_head ** ghp = (GS_head**)list2array(gslp);
      int n = list_len(gslp), i;
      qsort(ghp, n, sizeof(GS_head*), gsort_cmp);
      if (key)
	{
	  for (i = 0; i < n; ++i)
	    fprintf(stdout, "%s\t%s\n", ghp[i]->s, (char*)hash_find(keyhash, ghp[i]->s));
	}
      else
	{
	  for (i = 0; i < n; ++i)
	    fprintf(stdout, "%s\n", ghp[i]->s);
	}
      gsort_term();
    }

  gdlparse_term();

  return 0;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case '1':
      gdl_one_off_mode = 1;
      break;
    case 'a':
      gdl_unicode = 0; /* defaults to 1; 0 forces atf2utf */
      break;
    case 'b':
      bare_mode = 1;
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'C':
      cuneify_mode = 1;
      break;
    case 'd':
      gdlsig_depth_mode = 1;
      break;
    case 'e':
      error_stdout = 1;
      mesg_prefix(">>");
      gvl_sans_report = 1;
      break;
    case 'f':
      fname = optarg;
      break;
    case 'g':
      gsort_mode = 1;
      break;
    case 'G':
      gvl_script_type = optarg;
      break;
    case 'i':
      identity_mode = 1;
      break;
    case 'j':
      json_mode = 1;
      break;
      /* process the key counting from 1; print the entire line first
	 and append the result of processing */
    case 'k':
      key = atoi(optarg);
      break;
    case 'l':
      gdl_legacy = 1;
      break;
    case 'n':
      ns_output = 1;
      break;
    case 'o':
      gdl_orig_mode = 1;
      break;
    case 'p':
      project = arg;
      break;
    case 'P':
      gvl_strict = pedantic = 1;
      break;
    case 'Q':
      gdlseq_oid_mode = 1;
    case 'q':
      gdlseq_mode = 1;
      break;
    case 'r':
      ns_output = rnv_xml = 1;
      break;
    case 's':
      signatures = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'u': /* uptranslate to canonicalized version */
      gdl_c10e_mode = 1;
      break;
    case 'U':
      uname_mode = 1;
      break;
    case 'v':
      validate = 1;
      break;
    case 'w':
      wrapper = 1;
      break;
    case 'W':
      gdl_word_mode = 1;
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
  fprintf(stderr, "gdlx: give grapheme on command line or use -c / -i and read lines from stdin\n");
}
