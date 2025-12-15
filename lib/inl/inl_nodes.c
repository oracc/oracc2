#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

int suppress_empty_text_nodes = 1;

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
	scan_square(sp, (uchar*)s, (uchar**)&s);

      char *stext = NULL;
      if (itp->txt)
	{
	  if ('{' == *s)
	    stext = scan_curly(sp, s, &s);
	  else
	    mesg_verr(sp->mp, "missing {...} content after %s\n", ssp->name);
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
    mesg_verr(sp->mp, "bad tag %s\n", ssp->name);
  return s;
}

void
inl_text(Scan *sp, Tree *tp, const char *text)
{
  if (text)
    {
      if (*text || !suppress_empty_text_nodes)
	{
	  Scanseg *ssp = memo_new(inl_scanseg_m);
	  ssp->sp = sp;
	  ssp->np = tree_add(tp, NS_INL, "text", tp->curr->depth, NULL);
	  ssp->name = NULL;
	  ssp->np->text = text;
	  ssp->np->user = ssp;
	}
    }
} 

char *
inl_nodes(Scan *sp, Node *np, char *s)
{
  char save = (s && '@' == *s) ? '@' : '\0';
  while (*s)
    {
      if ('@' == save)
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
		  break;
		}
	    }
	}
      else
	{
	  char *at = NULL;
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
	      inl_text(sp, np->tree, s);
	      s = ++at;
	    }
	  else
	    {
	      save = '\0';
	      inl_text(sp, np->tree, s);
	      s = s + strlen(s);
	    }
	}
    }
  return s;
}
