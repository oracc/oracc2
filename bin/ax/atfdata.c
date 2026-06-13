#include <oraccsys.h>
#include <hash.h>
#include <roco.h>

#include "atfdata.h"
#include "atf_cdli.h"

const char *curr_file = "<file>";
int curr_line = 0;
const char *curr_pqx;

int all_00atf = 0, dry = 0;

const char *cat_mode;
const char *catproj;
const char *file;
const char *fix_dups_tab, *drops_lst;
const char *inputs_file = NULL;
const char *output_dir, *split_dir;
const char *project;
FILE *keepfp, *outfp, *splitfp;
int dups_mode; /* just check for duplicates, no other output */
int fix_dups_mode; /* fix dups using the table fix_dups_tab */
int fixing = 0; /* set to 1 for second pass when fixing actually happens */
int identity_mode;
int need_project = 1;
extern int atfd_flex_debug, atfl_flex_debug, atfi_flex_debug;
extern Hash *hseen;
extern Pool *pseen;
extern void atfdlex(void);
extern void atfilex(void);

static Roco *rdups;

static void
atfdata_init(void)
{
  hseen = hash_create(20000);
  pseen = pool_init();
}

static void
atfdata_term(void)
{
  hash_free(hseen, NULL);
  pool_term(pseen);
}

static void
atf_load_drops(const char *lst)
{
  unsigned char **d = loadfile_lines3((uccp)lst, NULL, NULL);
  int i;
  for (i = 0; d[i]; ++i)
    hash_add(hseen, d[i], (void*)(uintptr_t)1);
}

static int
atf_load_dups(const char *tab)
{
  rdups = roco_load1(tab);
  int i;
  for (i = 0; i < rdups->nlines; ++i)
    {
      int j;
      for (j = 0; rdups->rows[i][j] && *rdups->rows[i][j]; ++j)
	;
      if (j > 2)
	{
	  fprintf(stderr, "atf_load_dups: j=%d; [0]=%s; [1]=%s; [j-1]=%s\n", j,
		  rdups->rows[i][1], rdups->rows[i][1], rdups->rows[i][j-1]);
	  rdups->rows[i][1] = rdups->rows[i][j-1]; /* force the last relocation into slot 2 of the table */
	}
    }
  roco_hash(rdups);
  return 0;
}

static char **
atf_files(void)
{
  static glob_t gres;
  if (!glob_pattern("00atf/*.atf",&gres))
    return gres.gl_pathv;
  else
    return NULL;
}

void
atf_fix_dup(char *pqx)
{
  /* copy new pqx to buf pqx */
  unsigned char *r = hash_find(rdups->hdata, (uccp)pqx);
  if (r)
    fprintf(stderr, "atf_fix_dup: passed %s; found %s\n", pqx, r);
  if (r && *r)
    strcpy(pqx, (ccp)r);
}

const char **
atfdata_inputs(int argc, char *const*argv)
{
  static const char *xatfs[2] = { "-" , 0 };
  const char **ret = NULL;
  if (argv[optind])
    {
      xatfs[0] = argv[optind];
      ret = xatfs;
    }
  else if (all_00atf)
    {
      if (inputs_file)
	{
	  fprintf(stderr, "atfdata: no sense using -a (all-00atf) with -I (inputs-file)\n");
	  exit(1);
	}
      ret = (ccp*)atf_files();
    }
  else if (inputs_file)
    {
      if ('-' == *inputs_file && argv[optind])
	{
	  fprintf(stderr, "atfdata: -I<space><file>: use -I or -I<file>\n");
	  exit(1);
	}
      if (!dups_mode && !fix_dups_mode && !identity_mode)
	{
	  fprintf(stderr, "atfdata: must give -d, -f, or -i with -I <inputs_file>\n");
	  exit(1);
	}
      if ((fix_dups_mode || identity_mode || split_dups) && !output_dir)
	{
	  fprintf(stderr, "atfdata: must give -O <output_dir> when using -I <inputs_file>\n");
	  exit(1);
	}
      ret = (ccp*)loadfile_lines3((uccp)inputs_file,NULL,NULL);
    }
  else if (dups_mode || fix_dups_mode || identity_mode)
    ret = (ccp*)atf_files();
  else
    ret = xatfs;
  return ret;
}

void
atfdata_outputs(void)
{
  if (!output_dir)
    {
      if (fix_dups_mode)
	output_dir = "atfd_fix.d";
      else if (split_dups || identity_mode)
	output_dir = "atfdata.d";
    }

  if (split_dups && !split_dir)
    split_dir = "atfd_split.d";

  if (output_dir)
    {
      if (xmkdirs(output_dir))
	{
	  fprintf(stderr, "atfdata: error making output dir %s\n", output_dir);
	  exit(1);
	}
    }
  if (split_dir)
    {
      if (xmkdirs(split_dir))
	{
	  fprintf(stderr, "atfdata: error making split dir %s\n", split_dir);
	  exit(1);
	}
    }
}

int
main(int argc, char * const*argv)
{
  const char **atfs = NULL;
  options(argc, argv, "ac:dD:f:iI::j:lnO:p:S::xX");

  extern int gdl_flex_debug, gdldebug, atfdebug, atf_flex_debug; /* yydebug in gdl.y */

  if (!project && need_project)
    {
      fprintf(stderr, "%s: must give project on command line. Stop.\n", argv[0]);
      exit(1);
    }

  if (cat_mode && (!strcmp(cat_mode, "local") || !strcmp(cat_mode, "custom")))
    catproj = project;
  else
    catproj = "cdli";

  atfs = atfdata_inputs(argc, argv);
  atfdata_outputs();

  if (fix_dups_tab)
    atf_load_dups(fix_dups_tab);
  
  atfdata_init();

  if (drops_lst)
    atf_load_drops(drops_lst);
  
  atfd_flex_debug = 0;
  atfi_flex_debug = 0;
  mesg_init();

  if (dups_mode || fix_dups_mode || identity_mode)
    {
      int i;
      if (dups_mode)
	{
	  for (i = 0; atfs[i]; ++i)
	    {
	      FILE *fp = atfi_input(atfs[i]);
	      if (fp)
		{
		  /*fprintf(stderr, "atfdata: input: %s\n", atfs[i]);*/
		  if (identity_mode || split_dups)
		    {
		      keepfp = outfp = atfi_output(atfs[i], output_dir);
		      if (split_dups)
			splitfp = atfi_split(atfs[i], split_dir);
		    }
		  else
		    outfp = NULL; /* no output when outfp == NULL */
		  atfilex();
		  fclose(fp);
		  if (outfp)
		    fclose(outfp);
		}
	    }
	}
      else if (fix_dups_mode)
	{
	  fixing = 1;
	  for (i = 0; atfs[i]; ++i)
	    {
	      FILE *fp = atfi_input(atfs[i]);
	      FILE *outfp = atfi_output(atfs[i], output_dir);
	      atfilex();
	      fclose(fp);
	      if (outfp)
		fclose(outfp);
	    }
	}
      else
	{
	  if (fix_dups_mode || identity_mode || split_dups)
	    outfp = stdout;
	  atfilex();
	}
    }
  else
    {
      atf_cdli_init();
      int i;
      for (i = 0; atfs[i]; ++i)
	{
	  FILE *fp = atfd_input(atfs[i]);
	  atfdlex();
	  fclose(fp);
	}
      atf_cdli_term();
    }
  mesg_print(stderr);
  atfdata_term();
}

void help(void) { fprintf(stderr, "atfdatax: [-dfxX] -c [cat_mode] -p [project]\n"); }
int
opts(int c, const char *arg)
{
  switch (c)
    {
    case 'a':
      all_00atf = 1;
      break;
    case 'c':
      cat_mode = arg;
      break;
    case 'D':
      split_dups = 1;
      if (arg)
	drops_lst = arg;
    case 'd':
      need_project = 0;
      dups_mode = 1;
      break;
    case 'f':
      need_project = 0;
      fix_dups_mode = 1;
      fix_dups_tab = arg;
      break;
    case 'I':
      inputs_file = (arg ? arg : "-"); /* list of files to process */
      break;
    case 'i':
      identity_mode = 1;
      break;
    case 'n':
      dry = 1;
      break;
    case 'O':
      output_dir = arg;
      break;
    case 'j':
    case 'p':
      project = arg;
      break;
    case 'S':
      need_project = 0;
      dups_mode = split_dups = 1;
      split_dir = arg;
      break;
    case 'X':
      x_mode = 2;
    case 'x':
      need_project = 0;
      printf("id_text\tdesignation\tperiod\tprovenience\n");
      if (x_mode)
	exit(0);
      x_mode = 1;
      break;
    }
  return 0;
}
