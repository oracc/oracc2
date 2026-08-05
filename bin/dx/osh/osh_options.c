#include <ctype.h>
#include <oraccsys.h>
#include <dx.h>
#include <osh.h>

#include <unistd.h>

static int badchar(char *s);
static int check_args(char **av);
static void copy_argv(char **dst, char * const *av);
static int count_args(char *s, char * const *av);
static Job *j;

/**osh_options parses any command line options and fills an array with
   the commands to be executed.  When called from ssh these commands
   are all in a single string, which is split here at any whitespace.

   Unlike a real shell, there is no expansion or escaping.  All
   osh handles is commands like 'rimanum build all' or 'dcclt/nineveh
   check links'.

   Command tokens are validated to ensure they contain only characters
   in the set [A-Za-z0-9-_/] or > 128.

   Filenames or keys must be given with the -f or -k options; this is
   not yet implemented.
 */
char **
osh_options(int argc, char **argv, Job *jp)
{
  char **av = NULL;
  int ac = argc, ssh_mode = 0;
  if (argc == 2 && strchr(argv[optind], ' '))
    {
      ssh_mode = 1;
      ac = count_args(argv[1], argv);
      av = malloc((ac+2)*sizeof(char *));
      av[0] = argv[0];
      copy_argv(av+1, argv);
    }
  else
    av = argv;
  j = jp;
  if (!options(ac, av, "ov"))
    {
      if (av[optind])
	{
	  int x = optind;
	  jp->project = strdup(av[x++]);
	  if (av[x])
	    jp->cmd1 = strdup(av[x]);
	}
      if (!jp->project || !jp->cmd1) /*(!av[0+verbose] || !av[1+verbose])*/
	{
	  fprintf(stderr, "%s: request must contain at least a project and a command\n", progname);
	  free(av);
	  av = NULL;
	  exit(1);
	}
      int len = check_args(av);
      if (len > 0)
	{
	  /* Set up the job cmd vector and cmd string members */
	  jp->cmd = malloc(len+1);
	  *jp->cmd = '\0';
	  jp->cmdc = argc - optind;
	  jp->cmdv = malloc((jp->cmdc+1)*sizeof(char*));
	  if (jp->verbose)
	    jp->cmdv[0] = "-v";
	  memcpy(jp->cmdv+jp->verbose, &av[optind], (jp->cmdc+1) * sizeof(char*));
	  int i;
	  for (i = 0; av[i]; ++i)
	    strcat(strcat(jp->cmd, av[i]), " ");
	  jp->cmd[strlen(jp->cmd)-1] = '\0';
	}
      else
	{
	  if (ssh_mode)
	    free(av);
	  av = NULL;
	}
    }
  else
    {
      if (ssh_mode)
	free(av);
      av = NULL;
    }
  return av;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'o':
      j->odo_builtins = 1;
      break;
    case 'v':
      j->verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

void
help(void)
{
}

static int
badchar(char *s)
{
  static unsigned char good[128] = {
    ['A']=1,['B']=1,['C']=1,['D']=1,['E']=1,['F']=1,['G']=1,['H']=1,['I']=1,['J']=1,['K']=1,['L']=1,['M']=1,
    ['N']=1,['O']=1,['P']=1,['Q']=1,['R']=1,['S']=1,['T']=1,['U']=1,['V']=1,['W']=1,['X']=1,['Y']=1,['Z']=1,
    ['a']=1,['b']=1,['c']=1,['d']=1,['e']=1,['f']=1,['g']=1,['h']=1,['i']=1,['j']=1,['k']=1,['l']=1,['m']=1,
    ['n']=1,['o']=1,['p']=1,['q']=1,['r']=1,['s']=1,['t']=1,['u']=1,['v']=1,['w']=1,['x']=1,['y']=1,['z']=1,
    ['0']=1,['1']=1,['2']=1,['3']=1,['4']=1,['5']=1,['6']=1,['7']=1,['8']=1,['9']=1,
    ['-']=1,['_']=1,['/']=1
  };
  while (*s)
    if (*(unsigned char*)s < 128 && !good[(unsigned char)*s++])
      return 1;
  return 0;
}

static int
check_args(char **av)
{
  int len = -1, i;
  for (i = optind; av[i]; ++i)
    if (badchar(av[i]))
      {
	fprintf(stderr, "%s: bad character in command token\n", av[0]);
	free(av);
	av = NULL;
	break;
      }
    else
      len += strlen(av[i])+1;
  return len;
}

static void
copy_argv(char **dst, char * const*argv)
{
  char *s = memo_dup(argv[optind]);
  int i = 0;
  while (*s)
    {
      dst[i++] = s;
      while (*s && !isspace(*s))
	++s;
      if (*s)
	{
	  *s++ = '\0';
	  while (isspace(*s))
	    ++s;
	}
    }
  dst[i] = NULL;
}

static int
count_args(char *s, char *const *argv)
{
  if (verbose)
    fprintf(stderr, "%s: ssh: passed arguments: %s\n", argv[0], s);
  int sp = 1;
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
  return sp;
}
