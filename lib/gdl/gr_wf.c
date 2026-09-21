#include <oraccsys.h>
#include "gdl.h"
#include "gdlstate.h"

extern int gdl_word_excisions;

/* Functions and accessor array for gr_wf_funcs */
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
	gdlr_node(k, fp);
    }
  else
    gdlr_text(np, fp);
  return 0;
}

int
grf(c)(Node *np, FILE *fp)
{
  fputc('|', fp);
  if (np->kids && !strcmp(np->kids->name, "g:b"))
    {
      gdlr_node(np->kids, fp);
      fputc('|', fp);
      Node *k;
      for (k = np->kids->next; k; k = k->next)
	gdlr_node(k, fp);
    }
  else
    {
      Node *k;
      for (k = np->kids; k; k = k->next)
	gdlr_node(k, fp);
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
  gdlr_text(np, fp);
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
	gdlr_text(np, fp);
    }
  else
    gdlr_text(np, fp);    
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
  gdlr_node(np->kids, fp);
  fputc('(', fp);
  gdlr_node(np->kids->next, fp);
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
grf(o_a)(Node *np, FILE *fp)
{
  if ((unsigned int)*np->text > 127)
    fputc('x', fp);
  else
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
  gdlr_node(np->kids, fp);
  fputc('(', fp);
  gdlr_node(np->kids->next, fp);
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
	gdlr_node(k, fp);
    }
  else
    {
      gdlr_text(np, fp);
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
	gdlr_node(k, fp);
    }
  else
    {
      gdlr_text(np, fp);
    }
  return 0;
}

int
grf(w)(Node *np, FILE *fp)
{
  Node *k;
  for (k = np->kids; k; k = k->next)
    gdlr_node(k, fp);  
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

void
gdlr_ascii_funcs(GDLR_config *cp)
{
  (*cp)['o'] = gr_wf_o_a;
}

static Prop *
gdl_wf_deep_delim(Node *c)
{
  while (c && strcmp(c->name, "g:w"))
    c = c->last;
  if (c)
    return prop_find_kv(c->last->props, "g:delim", NULL);
  else
    return NULL;
}

int
gr_node_wf(Node *np, FILE *fp)
{
  gdlstate_t s = prop_get_state(np);
  if (!gs_is(s,gs_excised))
    {
      gdlstate_t n = np->next ? prop_get_state(np->next) : 0L;
      if ('g' == np->name[0])
	{
	  if (gr_funcs[np->name[3] ? np->name[3] : np->name[2]](np, fp))
	    {
	      if (np->next)
		{
		  Prop *d = prop_find_kv(np->props, "g:delim", NULL);
		  if ((!gs_is(n, gs_excised) || np->next->next)
		      && (d || (d = gdl_wf_deep_delim(np->next))))
		    fputs(':' == *d->u.k->v ? "-" : d->u.k->v, fp);
		}
	    }
	}
    }
  else
    {
      ++gdl_word_excisions;
    }
  return 0;
}

gdlr_node_fnc grf(fncs)[128] =
  {
    [0] = gr_node_wf,
    [1] = gdlr_orig_text,
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
