#include <oraccsys.h>
#include "gdl.h"
#include "gdlstate.h"

GDLR_config *gdlr_wfa_config, *gdlr_wfc_config, *gdlr_wfo_config;

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

GDLR_config *
gdlr_clone_fncs(GDLR_config *f)
{
  GDLR_config *cp = calloc(1, sizeof(GDLR_config));
  memcpy(cp, f, sizeof(GDLR_config));
  return cp;
}

/* This should only be called by gdl_render_setup_vx; see gr_vx for vx
   setup */
static GDLR_config *
gdl_render_init(int gdl_mode_opts)
{
  GDLR_config *cp = NULL;
  if (bit_get(gdl_mode_opts, GDLR_WF_INPUT))
    cp = gdlr_clone_fncs(&gr_wf_fncs);
    
  if (bit_get(gdl_mode_opts, GDLR_TEXT_ASCII))
    {
      (*cp)[0] = gdlr_ascii_text;
      gdlr_ascii_funcs(cp);
    }
  else if (bit_get(gdl_mode_opts, GDLR_TEXT_C10E))
    (*cp)[0] = gdlr_c10e_text;
  else
    (*cp)[0] = gdlr_orig_text;

  return cp;
}

void
gdl_render_setup_wf(void)
{
  gdlr_wfa_config = gdl_render_init(GDLR_WF_INPUT|GDLR_TEXT_ASCII);
  gdlr_wfc_config = gdl_render_init(GDLR_WF_INPUT|GDLR_TEXT_C10E);
  gdlr_wfo_config = gdl_render_init(GDLR_WF_INPUT|GDLR_TEXT_ORIG);
}

unsigned char *
gdl_render(Node *np, GDLR_config *c)
{
  /*gdl_render_setup(c);*/
  gr_funcs = *c;
  char *wf_buf = NULL;
  size_t wf_len = 0;
  gdl_word_excisions = 0;
  FILE *wf_fp = open_memstream(&wf_buf, &wf_len);
  gdlr_node(np, wf_fp);
  fclose(wf_fp);
  return (ucp)wf_buf;
}

void
gdl_output(Node *np, FILE *fp)
{
  gdlr_node(np, fp);
}

int
gdlr_orig_text(Node *c, FILE *fp)
{
  const char *t = c->text;
  if (c->user && ((gvl_g*)c->user)->orig)
    t = (ccp)((gvl_g*)c->user)->orig;
  fputs(t, fp);
  return 0;
}

int
gdlr_c10e_text(Node *c, FILE *fp)
{
  fputs(c->text, fp);
  return 0;
}

int
gdlr_ascii_text(Node *c, FILE *fp)
{
  const char *t = prop_val(c->props, "atf:ascii");
  fputs(t ? t : c->text, fp);
  return 0;
}
