#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

extern int line;

char *
inl_span(Tree *tp, char *s)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->name = scan_name(s, &s);

  struct inltok *itp = inltok(sp->name, strlen(sp->name));
  if (itp)
    {
      if ('[' == *s)
	scan_square(sp, (uchar*)s, (uchar**)&s, (size_t*)&line);

      char *stext = NULL;
      if ('{' == *s)
	stext = scan_curly(s, &s);

      if (itp->h)
	{
	  itp->h(tp, sp, stext);
#if 0
	  if (!strcmp(sp->name, "cite"))
	    sp->np = tree_add(tp, NS_BIB, "cite", tp->curr->depth, NULL);
#endif
	}
      else
	{
	  sp->np = tree_add(tp, NS_INL, "span", tp->curr->depth, NULL);
	  sp->np->text = stext;
	  if (!stext)
	    sp->np->name = "#";

	  sp->np->user = sp;

	  if (sp->np->text)
	    {
	      (void)tree_push(tp);
	      (void)inl_nodes(sp->np, (char*)sp->np->text);
	      (void)tree_pop(tp);
	    }
	}
    }
  else
    ;/*error*/
  return s;
}

void
inl_text(Tree *tp, const char *text)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->np = tree_add(tp, NS_INL, "text", tp->curr->depth, NULL);
  sp->name = NULL;
  sp->np->text = text;
  sp->np->user = sp;
} 

char *
inl_nodes(Node *np, char *s)
{
  char save = (s && *s) ? *s : '\0';
  while (*s)
    {
      if ('@' == save)
	{
	  save = '\0';
	  if (isalpha(*s))
	    s = inl_span(np->tree, s);
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
	      /* Find the next text chunk, skipping over @@ */
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
	      inl_text(np->tree, s);
	      s = ++at;
	    }
	  else
	    {
	      save = '\0';
	      inl_text(np->tree, s);
	      s = s + strlen(s);
	    }
	}
    }
  return s;
}
