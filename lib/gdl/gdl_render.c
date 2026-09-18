#include <oraccsys.h>
#include "gdl.h"

gdlr_text_fnc gdl_text_p;

unsigned char *
gdl_render(Node *np)
{
  Node *c;
  for (c = w->kids; c; c = c->next)
    {
      if (!strcmp(c->name, "g:x"))
	{
	  int no_post_x_delim = 0;
	  Prop *p = prop_find_kv(c->props, "g:type", NULL);
	  if (!strcmp(c->text, "..."))
	    {
	      fputc('x', wfp);
	    }
	  else
	    {
	      if (!p
		  || (p->u.k->v
		      && strcmp(p->u.k->v, "comment")
		      && strcmp(p->u.k->v, "dollar")
		      && strcmp(p->u.k->v, "linebreak")))
		mesg_verr(w->mloc, "gdl_wf_nodes: unhandled g:x text `%s'\n", c->text);
	      else if (p && (!strcmp(p->u.k->v, "comment") || !strcmp(p->u.k->v, "dollar")))
		no_post_x_delim = 1;
	    }
	  if (!no_post_x_delim)
	    {
	      Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	      if (d)
		fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	    }
	}
      else if (!strcmp(c->name, "g:det"))
	{
	  gdlstate_t s = 0L;
	  if (c->kids)
	    s = prop_get_state(c->kids);
	  else
	    mesg_verr(c->mloc, "gdl_wf_nodes: found a g:det with no ->kids");
	  if (!gs_is(s,gs_excised))
	    fputc('{', wfp);
	  Prop *p = prop_find_kv(c->props, "g:char", NULL);
	  if (p)
	    fputs(p->u.k->v, wfp);
	  gdl_wf_nodes(c, wfp);
	  if (!gs_is(s,gs_excised))
	    fputc('}', wfp);
	  /* post-determinatives have g:delim on the g:det node */
	  Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	  if (d)
	    fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	}
      else if (!strcmp(c->name, "g:gg"))
	{
	  Prop *p = prop_find_kv(c->props, "g:type", NULL);
	  Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	  if (p)
	    {
	      if (!strcmp(p->u.k->v, "correction"))
		fputs(c->kids->text, wfp);
	      else if (!strcmp(p->u.k->v, "alternation"))
		{
		  fputs(c->kids->text, wfp);
		  d = prop_find_kv(c->last->props, "g:delim", NULL);
		}
	      else if (!strcmp(p->u.k->v, "diszless"))
		fputs(c->text, wfp);
	      else
		gdl_wf_nodes(c, wfp);
	    }
	  else
	    gdl_wf_nodes(c, wfp);

	  if (!d && c->last)
	    d = prop_find_kv(c->last->props, "g:delim", NULL);

	  if (d)
	    fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	}
      else if (strcmp(c->name, "g:d") /*&& strcmp(c->name, "g:z")*/
	       && strcmp(c->name, "g:p") && strcmp(c->name, "g:gloss"))
	{
	  gdlstate_t s = prop_get_state(c);
	  if (!gs_is(s,gs_excised))
	    {
	      gdlstate_t n = c->next ? prop_get_state(c->next) : 0L;
	      /* 20260609 if c->user and c->user->orig are non-NULL
		 use the original transliteration in the word form
		 because that is what lemm-xxx.sig match against; set
		 gdl_wf_c10e=1 to force word forms to be
		 canonicalized */
	      if (c->text)
		{
		  gdlr_text_p(c);
		  if (c->next)
		    {
		      Prop *d = prop_find_kv(c->props, "g:delim", NULL);
		      if ((!gs_is(n, gs_excised) || c->next->next) && (d || (d = gdl_wf_deep_delim(c->next))))
			{
			  fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
			}
		    }
		}
	      else if (strcmp(c->name, "g:sur"))
		mesg_verr(c->mloc, "NULL text in %s node\n", c->name);
	    }
	  else
	    {
	      ++word_excisions;
	    }
	}
    }
}

void
gdlr_gdl_text(Node *c)
{
  const char *t = c->text;
  if (!gdl_wf_c10e && c->user && ((gvl_g*)c->user)->orig)
    t = (ccp)((gvl_g*)c->user)->orig;
  fputs(t, wfp);
}

void
gdlr_atf_text(Node *c)
{
  Prop *p = prop_find_kv(c->props, "atf:ascii", NULL);
  if (p)
    t = p->u.k->v;
  if ('c' == c->name[2])
    {
      fputc('|', wfp);
      Node *k;
      for (k = c->kids; k; k = k->next)
	{
	  if ('m' == k->name[2])
	    fputc('@', wfp);
	  else if ('f' == k->name[2])
	    fputc('~', wfp);
	  if ((p = prop_find_kv(k->props, "atf:ascii", NULL)))
	    fputs(p->u.k->v, wfp);
	  else if ('d' == k->name[2])
	    {
	      if (((unsigned)k->text[0]) > 127)
		fputc('x', wfp);
	      else
		fputs(k->text, wfp);
	    }
	  else
	    fputs(k->text, wfp);
	}
      fputc('|', wfp);
    }
  else
    {
      fputs(t, wfp);
      if (c->kids)
	{
	  /* skip g:b node and output mods */
	  Node *k;
	  for (k = c->kids->next; k; k = k->next)
	    {
	      if ('m' == k->name[2])
		fputc('@', wfp);
	      else
		fputc('~', wfp);
	      fputs(k->text, wfp);
	    }
	}
    }
}
