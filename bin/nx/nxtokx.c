#include <oraccsys.h>
#include <form.h>
#include "nx.h"

/* read .tok output and write a token list for processing by nx */
static Pool *p;
static List *toks = NULL;
static List *data = NULL;
static int non_num = 0;

static void
prtoks(List *tp, List *dp)
{
  const char *first_data = (dp ? ((const char *)list_first(dp)) : NULL);
  const char *last_tok = (tp ? ((const char *)list_last(tp)) : NULL);
  const uchar *t;

  if (last_tok && !strcmp(last_tok, "%lb"))
    {
      list_pop(tp);
      list_pop(dp);
    }
  
  printf("%%%%id %s %%%%n", first_data);
  for (t = list_first(tp); t; t = list_next(tp))
    printf(" %s", t);
  printf(" %%%%d");
  for (t = list_first(dp); t; t = list_next(dp))
    printf(" %s", t);
  printf(" %%%%.\n");
  list_free(toks, NULL);
  list_free(data, NULL);
  toks = data = NULL;
  pool_reset(p);
}

static const uchar *
tokcgp(const uchar *t, int *nflag)
{
  static Form f;
  memset(&f, '\0', sizeof(Form));
  if (form_parse(NULL, -1, pool_copy(t,p), &f, NULL) > 0)
    {
      if (f.cf && f.gw && (f.pos && (!f.pos[1] || 'N' != f.pos[1])))
	{
	  uchar tmp[strlen((ccp)f.cf)+strlen((ccp)f.gw)+3];
	  sprintf((char*)tmp,"%s[%s]", f.cf, f.gw);
	  struct nw_map *nmp = nwmap((ccp)tmp, strlen((ccp)tmp));
	  if (nmp)
	    return (uccp)nmp->mapped;
	  else
	    return (uccp)strdup((ccp)tmp);
	}
      else if (f.pos && !strcmp((ccp)f.pos, "n"))
	{
	  *nflag = 1;
	  struct nw_map *nmp = nwmap((ccp)f.form, strlen((ccp)f.form));
	  if (nmp)
	    return (uccp)nmp->mapped;
	  else
	    return f.form;
	}
      else if (f.pos)
	return f.pos;
      else
	return NULL;
    }
  else
    return NULL;
}

static void
savetoks(uchar *s)
{
  int nflag = 0;
  uchar *t = (uchar*)strchr((ccp)s, '\t');
  *t++ = '\0';
  uchar *e = (ucp)strchr((ccp)t, '\t');
  if (e)
    *e = '\0';
  const uchar *cgp = tokcgp(t, &nflag);
  if (cgp)
    {
      if (nflag /* || numword(cgp) */) /*numword must be true for gc and unit-words*/
	{
	  if (non_num)
	    {
	      if (toks && list_len(toks))
		prtoks(toks,data);
	      non_num = 0;
	    }
	}
      else
	++non_num;
      if (!toks)
	{
	  toks = list_create(LIST_SINGLE);
	  data = list_create(LIST_SINGLE);
	}
      uchar *cgp2 = pool_copy(cgp, p);
      if (strchr((ccp)cgp, '['))
	free((void*)cgp);
      list_add(toks, cgp2);
      list_add(data, pool_copy(s, p));
    }
}

static void
newline(uchar *s)
{
#if 1
  if (toks)
    {
      list_add(toks, "%lb");
      list_add(data, "-");
    }
#else
  if (list_len(toks))
    {
      mesg_print(stderr);
      mesg_init();
      prtoks(toks, data);
    }
#endif
}

int
main(int argc, const char **argv)
{
  uchar *l;
  int doing = 0;
  mesg_init();
  p = pool_init();
  while ((l = loadoneline(stdin, NULL)))
    {
      switch (*l)
	{
	case 'Y':
	  if (!strcmp((ccp)l+2, "xcl"))
	    doing=1;
	  else
	    doing=0;
	  break;
	case 'T':
	  if (toks && list_len(toks))
	    prtoks(toks,data);	  
	  break;
	case 'l':
	  if (doing)
	    savetoks(l+2);
	  break;
	case 'L':
	  if (doing)
	    newline(l);
	  break;
	case 'D':
	  if (doing && !strncmp((ccp)l, "D.eof", 5))
	    {
	      if (toks && list_len(toks))
		prtoks(toks,data);
	    }
	  break;
	}
    }
  if (toks && list_len(toks))
    prtoks(toks,data);
  mesg_print(stderr);
  (void)loadoneline(NULL,NULL);
  mesg_term();
  pool_term(p);
}
