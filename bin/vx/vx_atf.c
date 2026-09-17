#include <oraccsys.h>
#include "vx.h"

vx_simples_fnc vx_simples_p = vxa_simples;

int s_words;

char simples[128] =
  {
    ['a'] = 1,
    ['b'] = 1,
    ['d'] = 1,
    ['m'] = 1,
    ['o'] = 1,
    ['p'] = 1,
    ['q'] = 1,
    ['r'] = 1,
    ['s'] = 1,
    ['t'] = 1,
    ['v'] = 1,
    ['x'] = 1
  };

void
vxa_simples(Node *np, FILE *fp)
{
  vxa_openers(np, fp);
  if (np->text)
    fputs(np->text, fp);
  Node *mp;
  for (mp = np->kids; mp; mp = mp->next)
    vx_atf_gdl_node(mp, fp);
  vxa_closers(np, fp);
  vxa_g_delim(np, fp);
}

void
vxa_openers(Node *np, FILE *fp)
{
  if ('d' == np->name[2])
    fputc('{', fp);  
}
void
vxa_closers(Node *np, FILE *fp)
{
  if ('d' == np->name[2])
    fputc('}', fp);
}
void
vxa_g_delim(Node *np, FILE *fp)
{
  Prop *gd = prop_find_kv(np->props, "g:delim", NULL);
  if (gd)
    fputs(gd->u.k->v, fp);
}

void
vxa_status_flags(Node *np, FILE *fp)
{
  const char *gc = (vxa_prop_val(np, "g:collated") ? "*" : "");
  const char *gq = (vxa_prop_val(np, "g:queried")  ? "?" : "");
  const char *gr = (vxa_prop_val(np, "g:remarked") ? "!" : "");
  const char *gu1 = vxa_prop_val(np, "g:uflag1");
  const char *gu2 = vxa_prop_val(np, "g:uflag2");
  const char *gu3 = vxa_prop_val(np, "g:uflag3");
  const char *gu4 = vxa_prop_val(np, "g:uflag4");
  if (gu1 || gu2 || gu3 || gu4)
    fprintf(stderr, "vxa_status_flag: no output defined for user flags.\n");
  fprintf(fp, "%s%s%s", gc, gr, gq);
}

const char *
vxa_prop_val(Node *np, const char *aname)
{
  Prop *a = prop_find_kv(np->props, aname, NULL);
  if (a)
    return a->u.k->v;
  else
    return NULL;
}

void
vx_atf_gdl_node(Node *np, FILE *fp)
{
  Node *npp;
  const char *nodename = np->name;

  if (!strcmp(nodename, "g:w"))
    {
      if (s_words++)
	fputc(' ', fp);
      for (npp = np->kids; npp; npp = npp->next)
	vx_atf_gdl_node(npp, fp);
    }
  else if (!strcmp(np->name, "g:c"))
    {
      Node *cp;
      vxa_openers(np, fp);
      fputc('|', fp);
      for (cp = np->kids; cp; cp = cp->next)
	vx_atf_gdl_node(cp, fp);
      fputc('|', fp);      
      vxa_closers(np, fp);
      vxa_g_delim(np, fp);
    } 
  else if ('g' == np->name[0] && simples[(int)np->name[2]] && !np->name[3])
    {
      vx_simples_p(np, fp);
    }
  else if (!strcmp(np->name, "g:x")
	   || (!strcmp(np->name, "g:nonw") && !np->kids))
    {
      if (np->text)
	fputs(np->text, fp);
    }
  else
    {
      if (!strcmp(np->name, "g:gg"))
	{
	  if (prop_find_kv(np->props, "g:type", "diszless"))
	    fputs(np->text, fp);
	  else
	    {
	      Node *mp;
	      vxa_openers(np, fp);
	      for (mp = np->kids; mp; mp = mp->next)
		vx_atf_gdl_node(mp, fp);
	      vxa_closers(np, fp);
	      vxa_g_delim(np, fp);
	    }
	}
      else
	{
	  for (npp = np->kids; npp; npp = npp->next)
	    vx_atf_gdl_node(npp, fp);
	}
    }
}
