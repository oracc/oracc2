#include <oraccsys.h>

int qnames = 0;
const char *type = NULL;
#define _MAX_LINE 2048
static char *line, linebuf[_MAX_LINE];

const char *
rnc(const char *type)
{
  char r[strlen(oracc())+strlen("/lib/rnc/.rnc0")+strlen(type)];
  sprintf(r, "%s/lib/rnc/%s.rnc", oracc(), type);
  return strdup(r);
}

int
main(int argc, char * const*argv)
{
  
  options(argc, argv, "qt:");
  if (!type)
    {
      fprintf(stderr, "rnvx: must give RNC type on command line with -t TYPE. Stop.\n");
      exit(1);
    }
  const char *rncfn = rnc(type);
  if (argv[optind])
    {
      int i;
      for (i = optind; argv[i]; ++i)
	rnv_check(rncfn, argv[i]);
    }
  else
    {
      while ((line = fgets(linebuf,_MAX_LINE,stdin)))
	{
	  line[strlen(line)-1] = '\0';
	  rnv_check(rncfn, expand(NULL, line, type));
	}
    }
}

const char *prog = "rnvx";
int major_version = 1, minor_version = 0;
const char *usage_string = "rnvx [-q] -t TYPE FILES|(P:PQX from STDIN)";
void help (void) { }
int
opts(int arg,const char*str)
{
  switch (arg)
    {
    case 'q':
      qnames = 1;
      return 0;
    case 't':
      type = str;
      return 0;
    default:
      return 1;
    }
}
