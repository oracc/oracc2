#include <oraccsys.h>
#include "gdl.h"
#include "gdlstate.h"

int gdl_word_excisions;
int gdl_wf_c10e = 0;
gdlr_node_fnc *gr_funcs;

/* gdl_render handles several different calling cases, and is passed two kinds of input.
 *
 * INPUTS
 * ======
 *
 * gdl_render is (1) called when processing GDL/GVL in which case the
 * inputs is the XML tree that has been built during reading ATF,
 * including state set in gs_state.  When called in this mode
 * bracketing is by default omitted because the uses are to set
 * grapheme value attributes that omit the graphemic metadata.
 *
 * gdl_render is (2) called when processing a GDL tree that has been
 * created by reading XTF output.  In this mode no gs_state is used,
 * and bracketing must be handled via the attribute properties.
 *
 * OUTPUTS
 * =======
 *
 * Grapheme @form attributes on graphemes:
 *
 *       These can select between the original input, canonicalized,
 *       or ascii versions
 *
 * Word @form
 *
 *       This is used for lemmatization matching and needs to use the
 *	 original input and also excise any text in <<...>>
 *
 * ATF output
 *
 * 	 When called vx and other programs that render XTF back to
 *	 ATF, excised text must be included, but the grapheme text
 *	 choices may involve canonicalization or rendering as ASCII
 *	 ATF/C-ATF 
 */

#if 0
      if (c->text)
	{
	  gdlr_text_p(c);
	}
      ...
      else if (strcmp(c->name, "g:sur"))
	mesg_verr(c->mloc, "NULL text in %s node\n", c->name);
#endif

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
gr_node(Node *np, FILE *fp)
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

void
gdl_render_setup(GDLR_config gdl_mode_opts)
{
  gr_funcs = gr_wf_fncs;
}

unsigned char *
gdl_render(Node *np, GDLR_config c)
{
  gdl_render_setup(c);
  char *wf_buf = NULL;
  size_t wf_len = 0;
  gdl_word_excisions = 0;
  FILE *wf_fp = open_memstream(&wf_buf, &wf_len);
  gr_node(np, wf_fp);
  fclose(wf_fp);
  return (ucp)wf_buf;
}

int
gdlr_gdl_text(Node *c, FILE *fp)
{
  const char *t = c->text;
  if (!gdl_wf_c10e && c->user && ((gvl_g*)c->user)->orig)
    t = (ccp)((gvl_g*)c->user)->orig;
  fputs(t, fp);
  return 0;
}

int
gdlr_atf_text(Node *c, FILE *fp)
{
  const char *t;
  Prop *p = prop_find_kv(c->props, "atf:ascii", NULL);
  if (p)
    t = p->u.k->v;
  if ('c' == c->name[2])
    {
      fputc('|', fp);
      Node *k;
      for (k = c->kids; k; k = k->next)
	{
	  if ('m' == k->name[2])
	    fputc('@', fp);
	  else if ('f' == k->name[2])
	    fputc('~', fp);
	  if ((p = prop_find_kv(k->props, "atf:ascii", NULL)))
	    fputs(p->u.k->v, fp);
	  else if ('d' == k->name[2])
	    {
	      if (((unsigned)k->text[0]) > 127)
		fputc('x', fp);
	      else
		fputs(k->text, fp);
	    }
	  else
	    fputs(k->text, fp);
	}
      fputc('|', fp);
    }
  else
    {
      fputs(t, fp);
      if (c->kids)
	{
	  /* skip g:b node and output mods */
	  Node *k;
	  for (k = c->kids->next; k; k = k->next)
	    {
	      if ('m' == k->name[2])
		fputc('@', fp);
	      else
		fputc('~', fp);
	      fputs(k->text, fp);
	    }
	}
    }
  return 0;
}
