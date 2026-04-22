#include <oraccsys.h>
#include <runexpat.h>

const char *prog = NULL;
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = NULL;
void help (void) { }
int opts(int arg,const char*str){ return 1; }

/* A generic main() call for use with simple expat programs */
int
emain(const char *pname,
      int argc, char **argv,
      XML_StartElementHandler startElement, 
      XML_EndElementHandler endElement)
{
  char PQ[512];
  const char *fname[2];
  char usg[128];
  tab = stdout;
  prog = pname;
  sprintf(usg, "%s [-s] | FILENAME | <read PROJ:PQX names from stdin>", pname);
  usage_string = usg;

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
	  fname[0] = expand(NULL, PQ, "xtf");
	  fname[1] = NULL;
	  runexpat(i_list, fname, sH, eH);
	}
    }
  
  return 0;
}
