#include <oraccsys.h>
#include "vx.h"

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
vx_atf_node(Node *np, FILE *fp)
{
  Node *npp;
  const char *nodename = np->name;

  if (!strcmp(nodename, "g:w"))
    {
      if (s_words++)
	fputc(' ', fp);
      for (npp = np->kids; npp; npp = npp->next)
	vx_atf_node(npp, fp);
    }
  else if (!strcmp(np->name, "g:c"))
    {
      Node *cp;
      vxa_openers(np, fp);
      fputc('|', fp);
      for (cp = np->kids; cp; cp = cp->next)
	vx_atf_node(cp, fp);
      fputc('|', fp);      
      vxa_closers(np, fp);
      vxa_g_delim(np, fp);
    } 
  else if ('g' == np->name[0] && simples[(int)np->name[2]] && !np->name[3])
    {
      vxa_openers(np, fp);
      if (np->text)
	fputs(np->text, fp);
      Node *mp;
      for (mp = np->kids; mp; mp = mp->next)
	vx_atf_node(mp, fp);
      vxa_closers(np, fp);
      vxa_g_delim(np, fp);
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
		vx_atf_node(mp, fp);
	      vxa_closers(np, fp);
	      vxa_g_delim(np, fp);
	    }
	}
      else
	{
	  for (npp = np->kids; npp; npp = npp->next)
	    vx_atf_node(npp, fp);
	}
    }
}
