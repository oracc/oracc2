#include <oraccsys.h>
#include "gdl.h"
#include "gdlstate.h"

/* Functions and accessor array for gr_wf_funcs */

GDLR_config gdlr_wfa_config, gdlr_wfo_config;

#define grf(x) gr_wf_##x

int
grf(a)(Node *np, FILE *fp)
{
  fprintf(fp, "~%s", np->text);
  return 0;
}

/* GDL Render Word Form functions */

int
grf(b)(Node *np, FILE *fp)
{
  if (np->kids)
    {
      Node *k;
      for (k = np->kids->next; k; k = k->next)
	gr_node(k, fp);
    }
  else
    gr_funcs[0](np, fp);
  return 0;
}

int
grf(c)(Node *np, FILE *fp)
{
  fputc('|', fp);
  if (np->kids && !strcmp(np->kids->name, "g:b"))
    {
      gr_node(np->kids, fp);
      fputc('|', fp);
      Node *k;
      for (k = np->kids->next; k; k = k->next)
	gr_node(k, fp);
    }
  else
    {
      Node *k;
      for (k = np->kids; k; k = k->next)
	gr_node(k, fp);
      fputc('|', fp);
    }  
  return 0;
}

int
grf(d)(Node *np, FILE *fp)
{
  gdlstate_t s = 0L;
  if (np->kids)
    s = prop_get_state(np->kids);
  else
    mesg_verr(np->mloc, "gr_node: found a g:d with no ->kids");
  if (!gs_is(s,gs_excised))
    fputc('{', fp);
  Prop *p = prop_find_kv(np->props, "g:char", NULL);
  if (p)
    fputs(p->u.k->v, fp);
  gr_funcs[0](np, fp);
  if (!gs_is(s,gs_excised))
    fputc('}', fp);
  return 0;
}

int
grf(f)(Node *np, FILE *fp)
{
  /* graphetics not used in word forms */
  return 0;
}

int
grf(g)(Node *np, FILE *fp)
{
  Prop *p = prop_find_kv(np->props, "g:type", NULL);
  if (p)
    {
      if (!strcmp(p->u.k->v, "correction"))
	fputs(np->kids->text, fp);
      else if (!strcmp(p->u.k->v, "alternation"))
	fputs(np->kids->text, fp);
      else if (!strcmp(p->u.k->v, "diszless"))
	fputs(np->text, fp);
      else
	gr_funcs[0](np, fp);
    }
  else
    gr_funcs[0](np, fp);    
  return 0;
}

/* l is used for g:gloss */
int
grf(l)(Node *np, FILE *fp)
{
  /* not output in word form */  
  return 0;
}

int
grf(m)(Node *np, FILE *fp)
{
  fprintf(fp, "@%s", np->text);
  return 0;
}

int
grf(n)(Node *np, FILE *fp)
{
  gr_node(np->kids, fp);
  fputc('(', fp);
  gr_node(np->kids->next, fp);
  fputc(')', fp);
  return 0;
}

int
grf(o)(Node *np, FILE *fp)
{
  fputs(np->text, fp);
  return 0;
}

int
grf(p)(Node *np, FILE *fp)
{
  /* not output in word form */
  return 0;
}

int
grf(q)(Node *np, FILE *fp)
{
  gr_node(np->kids, fp);
  fputc('(', fp);
  gr_node(np->kids->next, fp);
  fputc(')', fp);
  return 0;
}

int
grf(r)(Node *np, FILE *fp)
{
  fputs(np->text, fp);
  return 0;
}

int
grf(s)(Node *np, FILE *fp)
{
  if (np->kids && !strcmp(np->kids->name, "g:b"))
    {
      Node *k;
      for (k = np->kids; k; k = k->next)
	gr_node(k, fp);
    }
  else
    {
      gr_funcs[0](np, fp);
    }
  return 0;
}

int
grf(t)(Node *np, FILE *fp)
{
  fputs(np->text, fp);
  return 0;
}

int
grf(v)(Node *np, FILE *fp)
{
  if (np->kids && !strcmp(np->kids->name, "g:b"))
    {
      Node *k;
      for (k = np->kids; k; k = k->next)
	gr_node(k, fp);
    }
  else
    {
      gr_funcs[0](np, fp);
    }
  return 0;
}

int
grf(w)(Node *np, FILE *fp)
{
  Node *k;
  for (k = np->kids; k; k = k->next)
    gr_node(k, fp);  
  return 0;
}

int
grf(x)(Node *np, FILE *fp)
{
  /*int no_post_x_delim = 0;*/
  Prop *p = prop_find_kv(np->props, "g:type", NULL);
  if (!strcmp(np->text, "..."))
    {
      fputc('x', fp);
    }
  else
    {
      if (!p
	  || (p->u.k->v
	      && strcmp(p->u.k->v, "comment")
	      && strcmp(p->u.k->v, "dollar")
	      && strcmp(p->u.k->v, "linebreak")))
	mesg_verr(np->mloc, "gr_node: unhandled g:x text `%s'\n", np->text);
      else if (p && (!strcmp(p->u.k->v, "comment") || !strcmp(p->u.k->v, "dollar")))
	/*no_post_x_delim = 1*/;
    }
  return 0;
}

gdlr_node_fnc grf(fncs)[128] =
  {
    [0] = gdlr_gdl_text,
    ['a'] = grf(a),
    ['b'] = grf(b),
    ['c'] = grf(c),
    ['d'] = grf(d),
    ['f'] = grf(f),
    ['g'] = grf(g),
    ['l'] = grf(l),
    ['m'] = grf(m),
    ['n'] = grf(n),
    ['o'] = grf(o),
    ['p'] = grf(p),
    ['q'] = grf(q),
    ['r'] = grf(r),
    ['s'] = grf(s),
    ['t'] = grf(t),
    ['v'] = grf(v),
    ['w'] = grf(w),
    ['x'] = grf(x)
  };
