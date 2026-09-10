#include <oraccsys.h>

FILE *atffp;
unsigned char **atflines = NULL, *atfmem = NULL, *curr = NULL;

int
sbrow(unsigned char *lp, char **f, char **l, char **i)
{
  *f = (char*)lp;
  if ((lp = (ucp)strchr((ccp)lp, '\t')))
    {
      *lp++ = '\0';
      *l = (char*)lp;
      if ((lp = (ucp)strchr((ccp)lp, '\t')))
	{
	  *lp++ = '\0';
	  *i = (char*)lp;
	  if (!strchr((ccp)lp, '\t'))
	    return 1;
	}
    }	    
  return 0;
}


void
atf_init(const char *fn)
{
  atflines = loadfile_lines3((uccp)fn, NULL, &atfmem);
  if (!atflines)
    exit(1);
}

void
atf_line(int lnum)
{
  curr = atflines[lnum-1];
}

int
atf_inst(char *ins, int nth)
{
  char *ip = ins;
  while ((ip = strstr(ip, ins)) && --nth > 0)
    ;

  if (ip)
    return ip[strlen(ip)-1] = 0x01;

  return 0;    
}

void
atf_term(void)
{
  int i;
  for (i = 0; atflines[i]; ++i)
    if (strchr((ccp)atflines[i], 0x01))
      {
	unsigned char *s = atflines[i];
	while (*s)
	  {
	    if (0x01 == *s)
	      fputs("] +.", stdout);
	    else
	      fputc(*s, stdout);
	  }
	fputc('\n', stdout);
      }
    else
      {
	fputs((ccp)atflines[i], stdout);
	fputc('\n', stdout);
      }
}

int
main(int argc, const char *const *argv)
{
  FILE *sbfp = xfopen(argv[1], "r");
  if (sbfp)
    {
      unsigned char *lp = NULL;
      while ((lp = loadoneline(sbfp, NULL)))
	{
	  char *fn = NULL;
	  char *ln = NULL;
	  char *inst = NULL;
	  if (sbrow(lp, &fn, &ln, &inst))
	    {
	      int lnum = atoi(ln), nth = 1;
	      if ((ln = strchr(ln, '/')))
		nth = atoi(ln+1);
	      if (atffp)
		atf_term();
	      atf_init(fn);
	      atf_line(lnum);
	      if (!atf_inst(inst, nth))
		fprintf(stderr, "%s:%d: inst `%s' occurrence %d not found in #lem line\n",
			fn, lnum, inst, nth);
	    }
	  else
	    {
	      fprintf(stderr, "lemsb: bad sb row. Stop\n");
	      exit(1);
	    }
	}
      if (atffp)
	atf_term();
    }
}
