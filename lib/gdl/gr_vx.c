#include <oraccsys.h>
#include <unidef.h>
#include "gdl.h"

int
gr_node_vx(Node *np, FILE *fp)
{
  if ('g' == np->name[0])
    {
      gdlr_openers(np, fp);
      (void)gr_funcs[np->name[3] ? np->name[3] : np->name[2]](np, fp);
      gdlr_flags(np, fp);
      gdlr_closers(np, fp);
      const char *d = prop_val(np, "g:delim");
      if (d)
	fputs(d, fp);
    }
  return 0;
}

/* This routine needs to use smart aliasing to normalize
   transliterations according to the lemmatization (and possibly the
   morphology) */
int
gdlr_oatf_text(Node *np, FILE *fp)
{
#if 0
  if (np->text)
    {
      if ('.' == *np->text || 'X' == *np->text || ('d' == *np->text && !np->text[1]))
	fputs(np->text, fp);
      else
	{
	  if (sll_has_sign_indicator((uccp)np->text))
	    {
	      const char *catf = hash_find(h_catf_sn, (uccp)np->text);
	      if (!catf)
		{
		  uccp lc = utf_lcase((uccp)np->text);
		  if (!(catf = hash_find(h_catf_sn, (uccp)lc)))
		    {
		      mesg_verr(np->mloc, "sign %s not found in C-ATF map\n", np->text);
		      catf = (ccp)utf2atf((uccp)np->text);
		    }
		}
	      fputs(catf, fp);
	    }
	  else
	    {
	      const char *catf = hash_find(h_catf, (uccp)np->text);
	      if (!catf)
		{
		  mesg_verr(np->mloc, "text %s not in C-ATF map\n", np->text);
		  catf = (ccp)utf2atf((uccp)np->text);
		}
	      fputs(catf, fp);
	    }
	}
    }
#endif
  return 0;
}

int
gdlr_vxc_openers(Node *np, FILE *fp)
{
  const char *o = prop_val(np, "g:o");
  const char *ao = prop_val(np, "atf:o");
  if (o)
    fputs(o, fp);

#if 0
  if (ao)
    fputs(ao, fp);
#endif
  
  return 0;
}

int
gdlr_vxo_openers(Node *np, FILE *fp)
{
  const char *o = prop_val(np, "g:o");
  const char *ao = prop_val(np, "atf:o");
  const char *ho = prop_val(np, "g:ho");
  if (o)
    fputs(o, fp);
  if (ho)
    fputs(U_ulhsq_u8str, fp);

#if 0
  if (ao)
    fputs(ao, fp);
#endif

  return 0;
}

int
gdlr_vxc_closers(Node *np, FILE *fp)
{
  const char *c = prop_val(np, "g:c");
  const char *ac = prop_val(np, "atf:c");
  const char *h = prop_val(np, "g:break");

  if (h && 'd' == *h)
    fputc('#', fp);

#if 0
  if (ac)
    fputs(ac, fp);
#endif
  
  if (c)
    fputs(c, fp);
  return 0;
}

int
gdlr_vxo_closers(Node *np, FILE *fp)
{
  const char *c = prop_val(np, "g:c");
  const char *ac = prop_val(np, "atf:c");
  const char *h = prop_val(np, "g:break");
  const char *hc = prop_val(np, "g:hc");

  if (hc)
    fputs(U_urhsq_u8str, fp);
  else if (h && 'd' == *h)
    fputc('#', fp);

#if 0
  if (ac)
    fputs(ac, fp);
#endif

  if (c)
    fputs(c, fp);
  return 0;
}

int
gdlr_vx_flags(Node *np, FILE *fp)
{
  const char *gc = (prop_val(np, "g:collated") ? "*" : "");
  const char *gq = (prop_val(np, "g:queried")  ? "?" : "");
  const char *gr = (prop_val(np, "g:remarked") ? "!" : "");
  const char *gu1 = prop_val(np, "g:uflag1");
  const char *gu2 = prop_val(np, "g:uflag2");
  const char *gu3 = prop_val(np, "g:uflag3");
  const char *gu4 = prop_val(np, "g:uflag4");
  if (gu1 || gu2 || gu3 || gu4)
    fprintf(stderr, "vxa_status_flag: no output defined for user flags.\n");
  fprintf(fp, "%s%s%s", gc, gr, gq);
  return 0;
}

GDLR_config *
gdl_render_setup_vx(int gdl_vx_mode, gdlr_node_fnc textfunc)
{
  GDLR_config *cp = gdlr_clone_fncs(&gr_wf_fncs);
  (*cp)[0] = gr_node_vx;
  if (textfunc)
    (*cp)[1] = textfunc;
  else
    (*cp)[1] = gdlr_orig_text;
  if (bit_get(gdl_vx_mode, GDLR_VX_CATF))
    {
      (*cp)[2] = gdlr_vxc_openers;
      (*cp)[3] = gdlr_vxc_closers;
      (*cp)[4] = gdlr_vx_flags;
      gdlr_ascii_funcs(cp);
    }
  else if (bit_get(gdl_vx_mode, GDLR_VX_OATF))
    {
      (*cp)[2] = gdlr_vxo_openers;
      (*cp)[3] = gdlr_vxo_closers;
      (*cp)[4] = gdlr_vx_flags;
    }
  else /* GDLR_VX_IDENTITY */
    {
      (*cp)[2] = gdlr_vxo_openers;
      (*cp)[3] = gdlr_vxo_closers;
      (*cp)[4] = gdlr_vx_flags;
    }
  return cp;
}
