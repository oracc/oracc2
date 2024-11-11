#include <oraccsys.h>
#include <form.h>
#include <lng.h>
#include <xcl.h>

FILE *f_log;
const char *file;
const char *project;
int lnum, verbose, fuzzy_aliasing, lem_autolem, lem_dynalem, bootstrap_mode;
int neo_mode = 0, oid_mode = 1;

int
main(int argc, char **argv)
{
  unsigned char *lp;
  Form f;
  int ln = 0;
  ccp lang;
  struct xcl_context *xcp = NULL;

  options(argc, argv, "no");

  if (neo_mode)
    {
      xcp = xcl_create();
      xcp->project = project = "neo";
    }
  
  mesg_init();
  form_init();
  lp = loadoneline(stdin, NULL);
  if (strncmp(lp, "@fields", strlen("@fields")))
    {
      fprintf(stderr, "sigx: first line of .sig file should start with @fields\n");
      exit(1);
    }
  while ((lp = loadoneline(stdin, NULL)))
    {
      const char *orig_lp = strdup(lp);
      memset(&f, '\0', sizeof(Form));
      if (!form_parse((uccp)"<stdin>", ++ln, lp, &f, NULL))
	{
	  if (oid_mode)
	    {
	      if (f.pos && isupper(f.pos[0]) && 'N' == f.pos[1])
		lang = "qpn";
	      else
		lang = f.core->name;
	      printf("%s\t%s[%s]%s\tword\n", lang, f.cf, f.gw, f.pos);
	      printf("%s\t%s[%s//%s]%s'%s\tsense\t%s[%s]%s\tword\n",
		     lang, f.cf, f.gw, f.sense, f.pos, f.epos, f.cf, f.gw, f.pos);
	    }
	  else if (neo_mode)
	    {
	      f.project = (uccp)"neo";
	      if (f.core)
		{
		  f.lang = (uccp)f.core->name;
		  f.morph2 = NULL;
		  printf("%s\n", form_sig(xcp, &f));
		}
	      else
		{
		  fprintf(stderr, "sigx: no f.core\n");
		}
	    }
	}
      else
	{
	  mesg_print(stderr);
	  mesg_init();
	}
      free(orig_lp);
    }
}
int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'n':
      neo_mode = 1;
      oid_mode = 0;
      break;
    case 'o':
      neo_mode = 0;
      oid_mode = 1;
      break;
    default:
      fprintf(stderr, "sigx: unknown option -%c\n", opt);
      return 1;
    }
  return 0;
}

void help(void) { }
