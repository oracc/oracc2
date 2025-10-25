#include "inl.h"

char *
inl_span(Tree *tp, char *s)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->n.name = scan_name(s, &s);
  sp->attr = scan_square(s, &s);
  sp->n.text = scan_curly(s, &s);
  sp->n.user = sp;
  tree_add(np->tree, &sp->n);
  (void)tree_push(np->tree);
  s = inl_nodes(&sp->n, sp->n.text);
  (void)tree_pop(np->tree);
  return s;
}

void
inl_text(Tree *tp, const char *text)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->n.name = "#text";
  sp->n.user = sp;
  tree_add(np->tree, &sp->n);
}

const char *
inl_nodes(Node *np, const char *s)
{
  while (*s)
    {
      if ('@' == *s)
	{
	  s = inl_span(tp, s);
	}
      else
	{
	  const char *at = strchr(s, '@');
	  if (at)
	    {
	      *at = '\0';
	      inl_text(tp, s);
	      s = at+1;
	    }
	  else
	    {
	      inl_text(tp, s);
	      s = s + strlen(s);
	    }
	}
    }
  return s;
}
