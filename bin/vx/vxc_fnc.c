#include <oraccsys.h>
#include "vx.h"

static void
vxc_kids(Node *np, CATF_helper *chp)
{
  Node *kp;
  for (kp = np->kids; kp; kp = kp->next)
    {
      if (NS_GDL == kp->ns)
	vx_atf_gdl_node(kp, chp->outfp);
      else
	{
	  Vxcfnctab *vp = vxcfnctab(kp->name, strlen(kp->name));
	  if (vp)
	    vp->fnc(kp, chp);
	  else
	    fprintf(stderr, "vxc: node %s not handled by vxcfnctab\n", kp->name);
	}
    }
}

void
vxc_andline(Node *np, FILE *fp)
{
  const char *pqx = vxa_prop_val(np, "xml:id");
  const char *n = vxa_prop_val(np, "n");
  fprintf(fp, "&%s = %s\n", pqx, n);
}

void
vxc_credit(Node *np, FILE *fp)
{
  const char *project = vxa_prop_val(np, "project");
  const char *pqx = vxa_prop_val(np, "xml:id");
  fprintf(fp, "#catforigin: after http://oracc.org/%s/%s\n", project, pqx);
}

void
vxc_composite(Node *np, CATF_helper *chp)
{
  vxc_andline(np, chp->outfp);
  vxc_kids(np, chp);
}

void
vxc_div(Node *np, CATF_helper *chp)
{
  char *type = strdup(vxa_prop_val(np, "type"));
  const char *n = vxa_prop_val(np, "n");
  fprintf(chp->outfp, "@div %s %s\n", type, n);
  vxc_kids(np, chp);
  fprintf(chp->outfp, "@end %s\n", type);
  free(type);
}

void
vxc_variants(Node *np, CATF_helper *chp)
{
  fprintf(chp->outfp, "@variants\n");
  vxc_kids(np, chp);
  fprintf(chp->outfp, "@endvariants\n");
}

void
vxc_variant(Node *np, CATF_helper *chp)
{
  fprintf(chp->outfp, "@variant\n");
  vxc_kids(np, chp);
}

void
vxc_nonx(Node *np, CATF_helper *chp)
{
  const char *strict = vxa_prop_val(np, "strict");
  if ('0' == *strict)
    fprintf(chp->outfp, "$ (%s)\n", np->text);
  else
    fprintf(chp->outfp, "$ %s\n", np->text);
}

void
vxc_lg(Node *np, CATF_helper *chp)
{
  vxc_kids(np, chp);
}

void
vxc_transliteration(Node *np, CATF_helper *chp)
{
  vxc_andline(np, chp->outfp);
  vxc_kids(np, chp);
}

void
vxc_protocols(Node *np, CATF_helper *chp)
{
  vxc_kids(np, chp);
  vxc_credit(chp->start, chp->outfp);
}

void
vxc_protocol(Node *np, CATF_helper *chp)
{
  const char *type = vxa_prop_val(np, "type");
  if (type)
    {
      if (np->text)
	{
	  if (!strstr(np->text, "math")) /* C-ATF doesn't allow use math */
	    {
	      if (strstr(np->text, "unicode"))
		fprintf(chp->outfp, "#%s: use ascii\n", type);
	      else
		fprintf(chp->outfp, "#%s: %s\n", type, np->text);
	    }
	}
      else
	{
	  if (!strcmp(type, "after"))
	    {
	      const char *url = vxa_prop_val(np, "url");
	      fprintf(chp->outfp, "#key: after %s\n", url);
	    }
	}
    }
  else
    fprintf(stderr, "vxc: no @type on protocol\n");
}

void
vxc_obj_sur(Node *np, CATF_helper *chp)
{
  const char *implicit = vxa_prop_val(np, "implicit");
  if (!implicit || '1' != *implicit)
    {
      const char *type = vxa_prop_val(np, "type");
      const char *n = vxa_prop_val(np, "type");
      fprintf(chp->outfp, "@%s", type);
      fprintf(chp->outfp, " %s", n);
      vxa_status_flags(np, chp->outfp);
      fputc('\n', chp->outfp);
    }
  vxc_kids(np, chp);
}

void
vxc_column(Node *np, CATF_helper *chp)
{
  const char *implicit = vxa_prop_val(np, "implicit");
  if (!implicit || '1' != *implicit)
    {
      const char *n = vxa_prop_val(np, "type");      
      fprintf(chp->outfp, "@column %s", n);
      vxa_status_flags(np, chp->outfp);
      fputc('\n', chp->outfp);
    }
  vxc_kids(np, chp);
}

void
vxc_l(Node *np, CATF_helper *chp)
{
  const char *n = vxa_prop_val(np, "n");      
  fprintf(chp->outfp, "%s. ", n);
  vxc_kids(np, chp);
  fputc('\n', chp->outfp);
}

void
vxc_v(Node *np, CATF_helper *chp)
{
  const char *n = vxa_prop_val(np, "varnum");      
  fprintf(chp->outfp, "%s: ", n);
  vxc_kids(np, chp);
  fputc('\n', chp->outfp);
}

void
vxc_xcl(Node *np, CATF_helper *chp)
{
  /* do nothing; just a stopper */
}
