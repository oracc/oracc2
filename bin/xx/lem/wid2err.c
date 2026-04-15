#include <oraccsys.h>
#include <runexpat.h>

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static const char *current_PQ = NULL;
static FILE *tab = NULL;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "g:w")) /* name[0] == 'w' && name[1] == '\0') */
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    fprintf(tab,"%s\t%s:%d\n",atts[i+1],pi_file,pi_line);
	}
    }
}

static void
eH(void *userData, const char *name)
{
}

int
main(int argc, char **argv)
{
  char PQ[512];
  const char *fname[2];
  tab = stdout;
  if (argc > 1)
    {
      if (!strcmp(argv[1], "-s"))
	{
	  runexpat(i_stdin, NULL, sH, eH);
	}
      else
	{
	  fname[0] = argv[1];
	  fname[1] = NULL;
	  runexpat(i_list, fname, sH, eH);
	}
    }
  else
    {
      while (fgets(PQ,512,stdin))
	{
	  const char *fname[2];
	  char *dot;
	  PQ[strlen(PQ)-1] = '\0';
	  if ((dot = strchr(PQ,'.')))
	    *dot = '\0';
	  current_PQ = PQ;
	  fname[0] = expand(NULL, PQ, "xtf");
	  fname[1] = NULL;
	  runexpat(i_list, fname, sH, eH);
	}
    }
  return 0;
}

const char *prog = "wid2err";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "labeltable <XTF >TAB";
void help (void) { }
int opts(int arg,const char*str){ return 1; }
