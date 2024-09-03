#include <oraccsys.h>
#include "nx.h"

typedef enum nxmode { NXM_NONE, NXM_NUM, NXM_DATA } nxmode;

static List *tlist = NULL, *dlist = NULL, *curr = NULL;
static nxmode mode = NXM_NONE;

static const uchar *untok = NULL;

static const uchar *nx_input_tok(FILE *fp);

static void
nx_input_untok(const uchar *u)
{
  untok = u;
}

static void
nx_input_setup(void)
{
  tlist = list_create(LIST_SINGLE);
  dlist = list_create(LIST_SINGLE);
  curr = tlist;
  mode = NXM_NUM;
  mesg_init();
}

static void
nx_input_unset(void)
{
  list_free(tlist, NULL);
  list_free(dlist, NULL);
  tlist = dlist = curr = NULL;
  mode = NXM_NONE;
  (void)nx_input_tok(NULL);
}

static const uchar *
nx_input_tok(FILE *fp)
{
  static uchar *buf = NULL;
  static int siz = -1;
  int len = 0;

  if (!fp)
    {
      if (buf)
	free(buf);
      buf = NULL;
      siz = -1;
      return NULL;
    }

  if (untok)
    {
      const uchar *ret = untok;
      untok = NULL;
      return ret;
    }
  
  int ch;
  while ((EOF != (ch = fgetc(fp))))
    {
      if (isspace(ch))
	{
	  buf[len] = '\0';
	  return pool_copy(buf, nxp->p);
	}
      if (siz <= len)
	{
	  if (siz < 0)
	    siz = 4;
	  else
	    siz *= 2;
	  buf = realloc(buf, siz);	  
	}
      buf[len++] = ch;
    }

  if (len)
    {
      buf[len] = '\0';
      return buf;
    }
  else
    return NULL;
}

void
nx_input(void)
{
  const uchar *t = NULL;
  nx_input_setup();
  
  while ((t = nx_input_tok(nxp->input)))
    {
      if ('%' == *t)
	{
	  if (!strcmp((ccp)t, "%%n"))
	    {
	      if (tlist && list_len(tlist))
		{
		  nx_exec_lists(tlist, dlist);
		  nx_term();
		  nx_input_unset();
		}
	      nx_init();
	      nx_input_setup();
	    }
	  else if (!strcmp((ccp)t, "%%d"))
	    {
	      mode = NXM_DATA;
	      curr = dlist;
	    }
	  else if (!strcmp((ccp)t, "%%."))
	    {
	      nx_exec_lists(tlist, dlist);
	      nx_term();
	      nx_input_unset();
	      const uchar *u = nx_input_tok(nxp->input);
	      if (u && strcmp((ccp)u, "%%n"))
		{
		  nx_init();
		  nx_input_setup();
		  nx_input_untok(u);
		}
	    }
	  else
	    {
	      list_add(curr, (void*)t);
	    }
	}
      else
	{
	  list_add(curr, (void*)t);
	}
    }

  if (tlist && list_len(tlist))
    {
      nx_exec_lists(tlist, dlist);
      nx_term();
      nx_input_unset();
    }
}

#if 0
static const uchar **
nx_toks(uchar *t, int *ntoks)
{
  int n = 0;
  while (isspace(*t))
    ++t;
  uchar *s = t;
  while (*s)
    {
      ++n;
      while (*s && !isspace(*s))
	++s;
      while (isspace(*s))
	++s;
    }
  const uchar **toks = malloc((n+1)*sizeof(const uchar *));
  s = t;
  int i = 0;
  while (*s)
    {
      toks[i++] = s;
      while (*s && !isspace(*s))
	++s;
      if (isspace(*s))
	*s++ = '\0';
      while (isspace(*s))
	++s;
    }
  toks[i] = NULL;
  *ntoks = n;
  return toks;
}
#endif
