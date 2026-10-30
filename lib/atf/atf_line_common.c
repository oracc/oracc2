#include <oraccsys.h>
#include <gdllib.h>
#include <lng.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"
#include "etcsl.h"

#include "linenums.c"

const char *clid; /* current line ID */
/* offset used in calculation of word-ids.  In main text word-ids
   add 1..n; in exemplar 1 they add 1001 .. n+1000; in exemplar 2
   they add 2001 .. n+2000, etc. */
int exemplar_offset = 0;
int bil_offset = 0;
Hash *last_tlit_h_hash = NULL;
struct node **last_tlit_h = NULL;
int lth_used = 0;
int last_tlit_h_decay = 0;
Line *curr_line, *curr_lem_host;
int line_lines = 0;
unsigned const char *lnstr(Mloc *mp, int number,int primes);
unsigned char *map_uscore(const unsigned char *vbar);

int lg_mode = 0;
int mylines = 1;
Pool *etcsl_pool = NULL;
Hash *etcsl_labels = NULL;
const char *etcsl_lid = NULL;

#if 1
Pool *ln_hpool;
void
lnstr_init(void)
{
  ln_hpool = hpool_init();
}
void
lnstr_term(void)
{
  if (ln_hpool)
    hpool_term(ln_hpool);
  ln_hpool = NULL;
}
/* Rewrite for ax/atfgdl: we always use mylines so instead of building
   lninfo as we go we see if the line number token can parse into a
   static line num, and if it can we hash-pool-copy it */
unsigned const char *
lnstr_tok(char *tok)
{
  int number;
  if (isdigit(*tok))
    {
      char *end;
      number = (int)strtol(tok, &end, 10);
      if (!end && number < 5000)
	return (uccp)lnstrs[number];
    }
  return hpool_copy((uccp)tok, ln_hpool);
}
#else
/*FIXME: test the array bounds and generate strings dynamically if 
  the line numbers are out of range */
unsigned const char *
lnstr(Mloc *mp, int number,int primes)
{
  if (primes)
    {
      if (primes > 4)
	{
	  mesg_verr(mp, "%d is too many primes: restructure using @fragment\n", primes);
	  return ucc("");
	}
      else if (number > 500)
	{
	  mesg_verr(mp, "%d is too big!",number);
	  return ucc("");
	}
      else
	return ucc(lnstrsp[primes-1][number]);
    }
  else
    {
      if (number > 4999)
	{
	  /* mesg_verr(&l, "%d is too big!",number); */
	  char buf[10];
	  sprintf(buf,"%d",number);
	  return pool_copy((unsigned char*)buf, atfmp->pool);
	}
      else
	return ucc(lnstrs[number]);
    }
}
#endif

unsigned char *
map_uscore(const unsigned char *vbar)
{
  unsigned char *tmp = pool_copy(vbar, atfmp->pool), *s;
  for (s = tmp; *s; ++s)
    if (*s == '_')
      *s = ' ';
  return tmp;
}

