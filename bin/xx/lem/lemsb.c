#include <oraccsys.h>

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

int
atf_line(int lnum)
{
  curr = atflines[--lnum]; /* This should now index the main transliteration line */
  while (strncmp((ccp)curr, "#lem:", 5) && atflines[lnum+1])
    curr = atflines[++lnum];
  return strncmp((ccp)curr, "#lem:", 5);
}

int
atf_inst(char *ins, int nth)
{
  char *ip = (char*)curr;
  while ((ip = strstr(ip, ins)) && --nth > 0)
    ip += strlen(ins);

  if (ip)
    {
      ip += strlen(ins);
      if (*ip)
	{
	  while (*ip && ';' != *ip)
	    {
	      if (ip[1] == '.' && (ip[0] == '+' || ip[0] == '-'))
		return 1; /* already an sb */
	      ++ip;
	    }
	  if (*ip)
	    *ip = 0x01;
	  else
	    {
	      while (isspace(ip[-1]) && ip > (char*)curr)
		--ip;
	      if (']' == ip[-1])
		ip[-1] = 0x02;
	      else
		return -1;
	    }
	}
      return 1;
    }

  return 0;    
}

void
atf_term(void)
{
  int i;
  for (i = 0; atflines[i]; ++i)
    if (strpbrk((ccp)atflines[i], "\x01\x02"))
      {
	unsigned char *s = atflines[i];
	while (*s)
	  {
	    if (0x01 == *s)
	      fputs(" +. ;", stdout);
	    else if (0x02 == *s)
	      fputs("] +.", stdout);
	    else
	      fputc(*s, stdout);
	    ++s;
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
  FILE *sbfp = NULL;

  if (argv[1])
    sbfp = xfopen(argv[1], "r");
  else
    sbfp = stdin;
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
	      if (strstr(inst, "+.") || strstr(inst, "-."))
		continue;
	      int lnum = atoi(ln), nth = 1;
	      if ((ln = strchr(ln, '/')))
		nth = atoi(ln+1);

	      if (!atflines)
		atf_init(fn);

	      if (atf_line(lnum))
		{
		  fprintf(stderr, "%s:%d: #lem line not found\n", fn, lnum);
		  continue;
		}
		
	      int ret = atf_inst(inst, nth);
	      if (ret == 0)
		fprintf(stderr, "%s:%d: inst `%s' occurrence %d not found in #lem line\n",
			fn, lnum, inst, nth);
	      else if (ret < 0)
		fprintf(stderr, "%s:%d: inst `%s' occurrence %d: unknown character at end of line\n",
			fn, lnum, inst, nth);
	    }
	  else
	    {
	      fprintf(stderr, "lemsb: bad sb row. Stop\n");
	      exit(1);
	    }
	}
      if (atflines)
	atf_term();
    }
}
