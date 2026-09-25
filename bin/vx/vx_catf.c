#include <oraccsys.h>
#include <gdl.h>
#include <lng.h>
#include <roco.h>
#include "vx.h"

GDLR_config *vx_catf_config;

static Hash *h_catf = NULL;
static Hash *h_catf_sn = NULL;
static Roco *r_catf = NULL;
const char *file;

int
gdlr_catf_text(Node *np, FILE *fp)
{
  if (np->text)
    {
      if ('.' == *np->text || 'X' == *np->text || 'x' == *np->text || ('d' == *np->text && !np->text[1]))
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
  return 0;
}

static void
vxc_load_map(void)
{
  char *catf_map = oracc_data("catf-map.tsv");
  r_catf = roco_load1(catf_map);
  int i;
  for (i = 0; i < r_catf->nlines; ++i)
    {
      if (!*r_catf->rows[i][1])
	r_catf->rows[i][1] = r_catf->rows[i][0];
    }
  h_catf = roco_hash(r_catf);
  h_catf_sn = hash_create(1024);
  for (i = 0; i < r_catf->nlines; ++i)
    {
      if ('_' != *r_catf->rows[i][2])
	{
	  char *s = (char*)r_catf->rows[i][2];
	  while (s && *s)
	    {
	      char *k = s;
	      s = strchr(s, ' ');
	      if (s)
		*s++ = '\0';
	      hash_add(h_catf, (uccp)k, r_catf->rows[i][1]);
	    }
	}
      hash_add(h_catf_sn, (uccp)r_catf->rows[i][0], r_catf->rows[i][4]);
      hash_add(h_catf_sn, (uccp)r_catf->rows[i][3], r_catf->rows[i][4]);
    }
}

void
vx_catf_init(void)
{
  vxc_load_map();
  vx_catf_config = gdl_render_setup_vx(GDLR_VX_CATF, gdlr_catf_text);
  (*vx_catf_config)[2] = gdlr_vxc_openers;
  (*vx_catf_config)[3] = gdlr_vxc_closers;
  (*vx_catf_config)[4] = gdlr_vx_flags;
  extern int gdl_no_xml_ids;
  lng_init();
  gdl_init();
}

void
vx_catf(Tree *tp, FILE *fp)
{
  static CATF_helper ch;
  ch.outfp = fp;

  if (tp->root)
    {
      Node *start = tp->root;
      Vxcfnctab *vp = vxcfnctab(start->name, strlen(start->name));
      if (!vp)
	{
	  if (start->kids && (vp = vxcfnctab(start->kids->name, strlen(start->kids->name))))
	    start = start->kids;
	  else
	    {
	      if (start->kids)
		fprintf(stderr, "vx_catf: root node %s / root->kids %s not a C-ATF node. Stop.\n",
			start->name, start->kids->name);
	      else
		fprintf(stderr, "vx_catf: root node %s not a C-ATF node. Stop.\n",
			start->name);
	    }
	}
      if (vp)
	{
	  vx_catf_init();
	  gr_funcs = *vx_catf_config;
	  ch.start = start;
	  vp->fnc(start, &ch);
	}
    }
}
