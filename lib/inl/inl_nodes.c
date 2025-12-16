#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

int suppress_empty_text_nodes = 1;
static void inl_text_node(Scan *sp, Tree *tp, const char *text);

char *
inl_span(Scan *sp, Tree *tp, char *s)
{
  Scanseg *ssp = memo_new(inl_scanseg_m);
  ssp->sp = sp;
  ssp->name = scan_name(sp, s, &s);

  struct inltok *itp = inltok(ssp->name, strlen(ssp->name));
  if (itp)
    {
      if (itp->tag)
	{
	  ssp->name = itp->tag;
	  ssp->attr = itp->attr;
	}
      else if ('[' == *s)
	scan_square(ssp, (uchar*)s, (uchar**)&s);

      char *stext = NULL;
      if (itp->txt)
	{
	  if ('{' == *s)
	    stext = scan_curly(sp, s, &s);
	  else
	    mesg_verr(&sp->mp, "missing {...} content after %s\n", ssp->name);
	}

      if (itp->h)
	{
	  itp->h(sp, ssp, stext);
	  ssp->np->user = ssp;
	}
      else
	{
	  ssp->np = tree_add(tp, NS_INL, "span", tp->curr->depth, NULL);
	  ssp->np->text = stext;
	  if (!stext)
	    ssp->np->name = "#";

	  ssp->np->user = ssp;

	  if (ssp->np->text)
	    {
	      (void)tree_push(tp);
	      (void)inl_nodes(sp, ssp->np, (char*)ssp->np->text);
	      (void)tree_pop(tp);
	    }
	}
    }
  else
    {
      if (inl_self_func && inl_self_func(ssp->name))
	{
	  char buf[strlen(ssp->name)+2];
	  sprintf(buf, "@%s", ssp->name);
	  inl_text_node(sp, tp, (ccp)pool_copy((uccp)buf, inl_scan_p));
	}
      else
	mesg_verr(&sp->mp, "bad inline tag %s\n", ssp->name);
    }
  return s;
}

static void
inl_text_node(Scan *sp, Tree *tp, const char *text, int len)
{
  Scanseg *ssp = memo_new(inl_scanseg_m);
  ssp->sp = sp;
  ssp->np = tree_add(tp, NS_INL, "text", tp->curr->depth, NULL);
  ssp->name = NULL;
  char *tmp = pool_alloc(len+1);
  strncpy(tmp, text, len);
  tmp[len] = '\0';
  ssp->np->text = tmp;
  ssp->np->user = ssp;
}

void
inl_text(Scan *sp, Tree *tp, const char *text, int len)
{
  if (text)
    {
      if (*text || !suppress_empty_text_nodes)
	inl_text_node(sp, tp, text, len);
    }
} 

char *
inl_nodes(Scan *sp, Node *np, char *s)
{
#if 1
  while (*s)
    {
      char *e = NULL
      if ('@' == *s && inl_command(s, &e))
	{
	  s = inl_span(sp, np->tree, s);
	}
      else
	{
	  char *at = NULL;
	  const char *t = s;
	  while (1)
	    {
	      /* Find the end of this text chunk, skipping over @@ */
	      at = strchr(s, '@');
	      if (at && '@' == at[1])
		s += 2;
	      else
		break;
	    }
	  inl_text(sp, np->tree, t, at-t);
	  if (at)
	    s = at;
	  else
	    s += strlen(s);
	}
    }
#else
  char save = (s && '@' == *s) ? '@' : '\0';
  while (*s)
    {
      if ('@' == save && '@' != s[1])
	{
	  save = '\0';
	  if (isalpha(*s))
	    s = inl_span(sp, np->tree, s);
	  else
	    {
	      switch (*s)
		{
		case '-':
		  break;
		case '"':
		case '?':
		  break;
		default:
		  /* error, unknown; ask if it's the old @me for
		     italicize-foreign-word; use @- now */
		  save = '@';
		  ++s;
		  break;
		}
	    }
	}
      else
	{
	  char *at = NULL;
	  const char *t = s;
	  while (1)
	    {
	      /* Find the end of this text chunk, skipping over @@ */
	      at = strchr(s, '@');
	      if (at && '@' == at[1])
		s += 2;
	      else
		break;
	    }
	  if (at)
	    {
	      save = '@';
	      *at = '\0';
	      inl_text(sp, np->tree, t);
	      s = ++at;
	    }
	  else
	    {
	      save = '\0';
	      inl_text(sp, np->tree, t);
	      s = s + strlen(s);
	    }
	}
    }
  return s;
#endif
}
