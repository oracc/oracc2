#include <oraccsys.h>

#define _MAX_LINE 2048
static char *line, linebuf[_MAX_LINE];

int r_access = 0;

int
main(int argc, char**argv)
{
  options(argc, argv, "r");
  setvbuf(stdout, NULL, _IONBF, 0);
  if (argv[optind+1])
    {
      puts(expand(NULL,argv[optind+1],argv[optind]));
    }
  else if (!argv[optind])
    {
      fprintf(stderr,"pqxpand: give extension as first argument, optional QID as second\n");
      exit(1);
    }
  else
    {
      while ((line = fgets(linebuf,_MAX_LINE,stdin)))
	{
	  if (line[strlen(line)-1] == '\n' || feof(stdin))
	    {
	      if ('\n' == line[strlen(line)-1])
		line[strlen(line)-1] = '\0';
	      if (strchr(line, ':'))
		{
		  char *at = strchr(line, '@');
		  if (at)
		    *at = '\0';
		  const char *fn = expand(NULL,line,argv[1]);
		  if (!r_access || !access(fn, R_OK))
		    puts(fn);
		}
	      else
		fprintf(stderr,"pqxpand only works on qualified IDs, ignoring %s\n", line);
	    }
	  else
	    {
	      fprintf(stderr,"expand: input line too long\n");
	      exit(1);
	    }
	}
    }
  return 0;
}

const char *prog = "pqxpand";
int major_version = 1, minor_version = 0;
const char *usage_string = "pqxpand EXTENSION";
void help (void) { }
int opts(int arg,const char*str){ if ('r' == arg) r_access = 1; return 0; }
