#include <oraccsys.h>
#include <hash.h>
#include "atfdata.h"
const char *cat_mode;
const char *catproj;
const char *file;
const char *fix_dups_tab;
const char *inputs_file = NULL;
const char *output_dir, *split_dir;
const char *project;
FILE *keepfp, *outfp, *splitfp;
int dups_mode; /* just check for duplicates, no other output */
int fix_dups_mode; /* fix dups using the table fix_dups_tab */
int fixing = 0; /* set to 1 for second pass when fixing actually happens */
int identity_mode;
int need_project = 1;
int x_mode; /* extract ID and NAME for X-IDs */
extern int atfd_flex_debug, atfl_flex_debug, atfi_flex_debug;
extern Hash *hseen;
extern Pool *pseen;
extern void atfdlex(void);
extern void atfilex(void);

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

void
atf_fix_dup(char *pqx)
{
  /* copy new pqx to buf pqx */
}

int
main(int argc, char **argv)
{
  options(argc, argv, "c:dDf:iI::j:lO:p:S:xX");

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

  if (argv[optind])
    {
      if (!freopen(argv[optind], "r", stdin))
	{
	  fprintf(stderr, "atfdata: unable to freopen %s. Stop.\n", argv[optind]);
	  exit(1);
	}
    }
  atfdata_init();
  atfd_flex_debug = 0;
  atfi_flex_debug = 0;
  mesg_init();

  if (inputs_file)
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
    }  

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
  
  if (dups_mode || fix_dups_mode || identity_mode)
    {
      if (inputs_file)
	{
	  const char **atffiles = (ccp*)loadfile_lines3((uccp)inputs_file,NULL,NULL);
	  int i;
	  for (i = 0; atffiles[i]; ++i)
	    {
	      FILE *fp = atfi_input(atffiles[i]);
	      if (fp)
		{
		  /*fprintf(stderr, "atfdata: input: %s\n", atffiles[i]);*/
		  if (identity_mode || split_dups)
		    {
		      keepfp = outfp = atfi_output(atffiles[i], output_dir);
		      if (split_dups)
			splitfp = atfi_split(atffiles[i], split_dir);
		    }
		  else
		    outfp = NULL; /* no output when outfp == NULL */
		  atfilex();
		  fclose(fp);
		  if (outfp)
		    fclose(outfp);
		}
	    }
	  if (fix_dups_mode)
	    {
	      fixing = 1;
	      for (i = 0; atffiles[i]; ++i)
		{
		  FILE *fp = atfi_input(atffiles[i]);
		  FILE *outfp = atfi_output(atffiles[i], output_dir);
		  atfilex();
		  fclose(fp);
		  if (outfp)
		    fclose(outfp);
		}
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
      atfdlex();
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
    case 'c':
      cat_mode = arg;
      break;
    case 'D':
      need_project = 0;
      split_dups = 1;
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
