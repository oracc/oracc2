#include <ctype.h>
#include <oraccsys.h>
#include <dx.h>
#include <osh.h>

char **
osh_options(int argc, char **argv)
{
  char **av = NULL;
  options(argc, argv, "cp:v");
  if (argv[optind] && *argv[optind])
    {
      int sp = 1;
      if (ssh_mode)
	{
	  char *s = argv[optind];
	  if (verbose)
	    fprintf(stderr, "%s: passed arguments: %s\n", argv[0], s);
	  while (*s)
	    {
	      if (isspace(*s))
		{
		  ++sp;
		  while (isspace(*s))
		    ++s;
		}
	      else
		++s;
	    }
	}
      else
	{
	  sp = (argc - optind) + 1;
	}
      av = malloc((sp+2)*sizeof(char *));
      av[0] = "odo";
      if (ssh_mode)
	{
	  char *s = argv[optind];
	  sp = 1;
	  while (*s)
	    {
	      av[sp++] = s;
	      while (*s && !isspace(*s))
		++s;
	      if (*s)
		{
		  *s++ = '\0';
		  while (isspace(*s))
		    ++s;
		}
	    }
	  av[sp] = NULL;
	}
      else
	{
	  int i = 1;
	  while (argv[optind])
	    av[i++] = argv[optind++];
	  av[i] = NULL;
	}
    }
  return NULL;
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'c':
      ssh_mode = 1;
      break;
    case 'p':
      project = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

void
help()
{
}
