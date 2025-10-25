#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

char *
inl_span(Tree *tp, char *s)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->np = tree_add(tp, NS_INL, "span", tp->curr->depth, NULL);
  sp->name = scan_name(s, &s);
  scan_square(sp, s, &s, 0);
  sp->np->text = scan_curly(s, &s);
  sp->np->user = sp;
  
  (void)tree_push(tp);
  s = inl_nodes(sp->np, (char*)sp->np->text);
  (void)tree_pop(tp);
  return s;
}

void
inl_text(Tree *tp, const char *text)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->np = tree_add(tp, NS_INL, "text", tp->curr->depth, NULL);
  sp->name = NULL;
  sp->np->user = sp;
} 

char *
inl_nodes(Node *np, char *s)
{
  while (*s)
    {
      if ('@' == *s)
	{
	  s = inl_span(np->tree, s);
	}
      else
	{
	  char *at = strchr(s, '@');
	  if (at)
	    {
	      *at = '\0';
	      inl_text(np->tree, s);
	      s = at+1;
	    }
	  else
	    {
	      inl_text(np->tree, s);
	      s = s + strlen(s);
	    }
	}
    }
  return s;
}
