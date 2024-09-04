#include <oraccsys.h>
#include "nx.h"

typedef enum nxmode { NXM_NONE, NXM_NUM, NXM_DATA } nxmode;

static List *tlist = NULL, *dlist = NULL, *curr = NULL;
static nxmode mode = NXM_NONE;

static const uchar *untok = NULL;

static const uchar *nx_input_tok(FILE *fp);
static void nx_input_ne(const uchar *t);
static const uchar *nx_input_until_quote(const uchar *start, FILE *fp);

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
  nx_reset();
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
	  if (len)
	    {
	      buf[len] = '\0';
	      return pool_copy(buf, nxp->p);
	    }
	  else
	    continue; /* skip leading whitespace before tokens */
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
		  nx_input_unset();
		  nx_input_setup();
		}
	      else if (mode == NXM_NONE)
		{
		  nx_input_setup();
		}
	    }
	  else if (!strcmp((ccp)t, "%%d"))
	    {
	      mode = NXM_DATA;
	      curr = dlist;
	    }
	  else if (!strcmp((ccp)t, "%%."))
	    {
	      nx_exec_lists(tlist, dlist);
	      nx_input_unset();
	      const uchar *u = nx_input_tok(nxp->input);
	      if (u && strcmp((ccp)u, "%%n"))
		nx_input_setup();
	      nx_input_untok(u);
	    }
	  else
	    {
	      list_add(curr, (void*)t);
	    }
	}
      else if ('$' == *t)
	{
	  nx_input_ne(t);
	}
      else
	{
	  list_add(curr, (void*)t);
	}
    }

  if (tlist && list_len(tlist))
    {
      nx_exec_lists(tlist, dlist);
      nx_input_unset();
    }
}

static void
nx_input_ne(const uchar *t)
{
  if (t[2] == '=' || t[3] == '=')
    {
      char e[3];
      e[0] = t[1];
      if (t[2] != '=')
	{
	  e[1] = t[2];
	  e[2] = '\0';
	  t = t+4;
	}
      else
	{
	  e[1] = '\0';
	  t = t+3;
	}
      struct nxt_tab *net = ne(e, strlen(e));
      if (net)
	{
	  if ('"' == *t)
	    {
	      if (!(t = nx_input_until_quote(++t, nxp->input)))
		fprintf(stderr, "runaway ne definition for %s; env setting ignored\n", e);
	      else
		hash_add(nxp->env, (ucp)net->name, hpool_copy(t, nxp->e));
	      /*fprintf(stderr, "env added %s \"=\" %s\n", e, t);*/
	    }
	  else
	    {
	      hash_add(nxp->env, (ucp)net->name, hpool_copy(t, nxp->e));
	      /*fprintf(stderr, "env added %s = %s\n", e, t);*/
	    }
	}
      else
	fprintf(stderr, "nx_input: ignoring unknown ne %s\n", e);
    }
  else
    fprintf(stderr, "nx_input: ignoring malformed ne %s\n", t);
}

static const uchar *
nx_input_until_quote(const uchar *start, FILE *fp)
{
  uchar *buf = NULL;
  int siz, len;

  len = strlen((ccp)start) + 1;
  siz = 3*len;
  buf = malloc(siz);
  strcpy((char*)buf, (ccp)start);
  strcat((char*)buf, " ");
  
  int ch;
  while ((EOF != (ch = fgetc(fp))))
    {
      if ('"' == ch)
	{
	  buf[len] = '\0';
	  uchar *ret = pool_copy(buf, nxp->p);
	  free(buf);
	  return ret;
	}
      if (siz <= len)
	{
	  siz *= 2;
	  buf = realloc(buf, siz);	  
	}
      buf[len++] = ch;
    }
  free(buf);
  return NULL;
}
