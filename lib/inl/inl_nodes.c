#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

static int double_at_flag = 0;
int suppress_empty_text_nodes = 1;
static void inl_span_node(Scan *sp, Scanseg *ssp, Tree *tp, const char *stext);
static void inl_text_node(Scan *sp, Tree *tp, const char *text, int len);

#define INL_CMD_MAX 1023

struct inltok *
inl_command(char *s)
{
  int bmax = INL_CMD_MAX;
  char buf[bmax+1];
  if (isalpha(s[1]))
    {
      char *t = s+1, *b = buf;
      while (isalpha(*t))
	{
	  if (b - buf < bmax)
	    *b++ = *t++;
	  else
	    break;
	}
      *b = '\0';
    }
  else
    {
      buf[0] = s[1];
      buf[1] = '\0';
    }
    
  struct inltok *itp = inltok(buf, strlen(buf));
  if (!itp)
    {
      /* @* and @= are impossible sequences for token lookup */
      if (inl_wild_mode)
	{
	  itp = inltok("@*", 2);
	  itp->text = (ccp)pool_copy((uccp)buf, inl_scan_p);
	}
      else if (inl_self_func && inl_self_func(buf))
	{
	  itp = inltok("@=", 2);
	  itp->text = (ccp)pool_copy((uccp)buf, inl_scan_p);
	}
    }
  return itp;
}

char *
inl_self(Scan *sp, struct inltok *itp, Tree *tp, char *s)
{
  inl_text_node(sp, tp, itp->text, strlen(itp->text));
  return s + strlen(itp->text) + 1; /* s points at '@' of '@listdef' */
}

char *
inl_wild(Scan *sp, struct inltok *itp, Tree *tp, char *s)
{
  Scanseg *ssp = memo_new(inl_scanseg_m);
  ssp->sp = sp;
  ssp->name = "langi";
  inl_span_node(sp, ssp, tp, s);
  return s + strlen(itp->text) + 1; /* s points at '@' of '@me' */
}

static void
inl_span_node(Scan *sp, Scanseg *ssp, Tree *tp, const char *stext)
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

char *
inl_span(Scan *sp, struct inltok *itp, Tree *tp, char *s)
{
  Scanseg *ssp = memo_new(inl_scanseg_m);
  ssp->sp = sp;
  ssp->name = itp->name;

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
      if (itp->term)
	stext = scan_str_term(sp, itp->term, s, &s);
      else if ('{' == *s)
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
      inl_span_node(sp, ssp, tp, stext);
    }
  return s;
}

static void
unescape_at(char *buf, const char *txt, int len)
{
  char *dest = buf;
  int i;
  for (i = 0; i < len; ++i)
    {
      if ('@' == *txt && '@' == txt[1])
	{
	  *dest++ = '@';
	  txt += 2;
	  ++i;
	}
      else
	*dest++ = *txt++;
    }
  *dest = '\0';
}

static void
inl_text_node(Scan *sp, Tree *tp, const char *text, int len)
{
  Scanseg *ssp = memo_new(inl_scanseg_m);
  ssp->sp = sp;
  ssp->np = tree_add(tp, NS_INL, "text", tp->curr->depth, NULL);
  ssp->name = NULL;
  char *tmp = (char*)pool_alloc(len+1, inl_scan_p);
  if (double_at_flag)
    unescape_at(tmp, text, len);
  else
    {
      strncpy(tmp, text, len);
      tmp[len] = '\0';
    }
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
      struct inltok *itp = NULL;
      if ('@' == *s && (itp = inl_command(s)))
	{
	  switch (itp->name[1])
	    {
	    case '*':
	      s = inl_wild(sp, itp, np->tree, s);
	      break;
	    case '=':
	      s = inl_self(sp, itp, np->tree, s);
	      break;
	    default:
	      s = inl_span(sp, itp, np->tree, s+strlen(itp->name)+1);
	      break;
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
	      if (at)
		{
		  if ('@' == at[1])
		    {
		      s += 2;
		      ++double_at_flag;
		    }
		  else if (at > t)
		    break;
		  else
		    ++s;
		}
	      else
		{
		  if (!*s)
		    break;
		  else
		    ++s;
		}
	    }
	  inl_text(sp, np->tree, t, at ? at-t : strlen(t));
	  if (at)
	    s = at;
	  else
	    s += strlen(s);
	}
    }
  return s;
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
