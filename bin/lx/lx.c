#include <oraccsys.h>
#include "lx.h"

int check;
int inplace;
/* The -m PROJECT option forces the CAT field of all items to the
   named project before output. If a project needs to set arbitrary
   catalogue fields for each item that can be done by creating
   00lib/proxy-cat.lst which gives just item and cat, e.g.,
   P123456@blms. If this file exists it is added as the last thing
   lx-inputs does and has the effect of forcing the catalogue field
   for all items in proxy-cat.lst
 */
char *metaforce;
const char *output;
const char *prog;
const char *project;
const char *projfile;
int proxy;
int qualify;
int sort;
int status = 0; /* 0 = OK to proceed after args; 1 = exit after args */
int uniq;	/* uniq does not require sort in lx */
int verbose;
int ximport;	/* import x-field from proxies */
int zfirstoptional;
int all_optional = 0;

int
main(int argc, char * const*argv)
{
  loadfile_prog((prog = argv[0]));
  
  if (options(argc, argv, "a:cio:p:qsuvxz"))
    return 1;

  if (qualify && !project)
    {
      fprintf(stderr, "%s: -q specified without -p PROJECT. Stop.\n", argv[0]);
      return 1;
    }

  List *todo = NULL;
  
  if (argv[optind])
    {
      if (inplace)
	output = argv[optind];

      mesg_init();
      todo = lx_set_args(argv, optind);
    }
  else if (projfile)
    {
      todo = lx_set_args(lx_projfile(projfile), 1);
    }
  else
    {
      fprintf(stderr, "%s: nothing to do. Stop.\n", argv[0]);
      exit(1);
    }

  /* We output the errors detected during load-time first; if any
     were unrecoverable todo will be NULL otherwise we will go
     ahead and process the todo list anyway
     
     mesg_print frees the mesg list */
  mesg_print(stderr);
  
  Lxfile *lxf = NULL;
  if (todo && list_len(todo))
    {
      if ((lxf = lx_set_ops(todo)))
	lx_finish(lxf);
      else
	++status;
    }
  
  if (lxf)
    {
      if (!check || output)
	lx_output(lxf, output);
      lx_free(lxf);
    }
  else if (status)
    fprintf(stderr, "%s: lx processing failed. Stop.\n", argv[0]);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'a':
      projfile = arg;
      all_optional = 1;
      break;
    case 'c':
      check = 1;
      break;
    case 'i':
      inplace = 1;
      break;
    case 'm':
      metaforce = (char*)arg;
      break;
    case 'o':
      output = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'q':
      qualify = 1;
      break;
    case 's':
      sort = 1;
      break;
    case 'u':
      uniq = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      ximport = 1;
      break;
    case 'z':
      zfirstoptional = 1;
      break;
    default:
      return 1;  
    }
  return 0;
}

void help(void) { fprintf(stderr, "lx: use -c to check, -s to show, files on command line\n"); }
