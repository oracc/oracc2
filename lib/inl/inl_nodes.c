#include <oraccsys.h>
#include <scan.h>
#include "inl.h"

extern int line;

char *
inl_span(Tree *tp, char *s)
{
  Scan *sp = memo_new(inl_scan_m);
  sp->name = scan_name(s, &s);
  if (!strcmp(sp->name, "cite"))
    sp->np = tree_add(tp, NS_BIB, "cite", tp->curr->depth, NULL);
  else
    sp->np = tree_add(tp, NS_INL, "span", tp->curr->depth, NULL);
  if ('[' == *s)
    scan_square(sp, s, &s, &line);
  if ('{' == *s)
    sp->np->text = scan_curly(s, &s);
  else
    sp->np->name = "#";
  sp->np->user = sp;

  if (sp->np->text)
    {
      (void)tree_push(tp);
      (void)inl_nodes(sp->np, (char*)sp->np->text);
      (void)tree_pop(tp);
    }
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
	  s = inl_span(np->tree, s);
	  save = '\0';
	}
      else
	{
	  char *at = strchr(s, '@');
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
