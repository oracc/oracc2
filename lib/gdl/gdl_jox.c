#include <stdio.h>
#include <tree.h>
#include <rnvxml.h>
#include <gdl.h>
#include <joxer.h>

/**This module requires rnvif setup to have been done first
 *
 * The implementation here is very preliminary--just enough to support
 * the GDL that occurs in OSL.
 *
 * oflag gives an opener status
 *
 * nflag is 0 for no processing of np->next; 1 for do processing of np->next
 */
static void
grx_jox_node(Node *np, int oflag, int nflag)
{
  Node *npp;
  int need_closer = 1;
  const char *nodename = np->name;

  if (!strcmp(np->name, "g:l"))
    nodename = "g:s";
  else if (!strcmp(np->name, "g:gp"))
    nodename = "g:g";
  
  if (!strcmp(nodename, "g:w"))
    {
      if (joxer_mode != JOXER_XML)
	joxer_ao("j:w");
      joxer_ea(np->mloc, nodename,
	       rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
      need_closer = 3;
    }
  else if (!strcmp(np->name, "g:d"))
    {
      const char *type = NULL;
      char text[2];
      text[0] = '\0';
      switch (*(unsigned char *)np->text)
	{
	case '-':
	case '\0':
	  type = NULL;
	  break;
	case ':':
	  type = "reordered";
	  break;
	case '.':
	  type = "beside";
	  break;
	case '&':
	  type = "above";
	  break;
	case '@':
	  type = "opposing";
	  break;
	case '%':
	  type = "crossing";
	  break;
	case '+':
	  type = "joining";
	  break;
	case 0xc3:
	  type = "containing";
	  break;
	case '3':
	case '4':
	  type = "repeated";
	  *text = *np->text;
	  text[1] = '\0';
	  break;
	default:
	  fprintf(stderr, "unhandled character '%c' in delimiter node\n", *np->text);
	  break;
	}
      if (type)
	{
	  if (*text)
	    joxer_eto(np->mloc, "g:o", rnvval_aa("x", "g:type", type, NULL), text);
	  else
	    joxer_ec(np->mloc, "g:o", rnvval_aa("x", "g:type", type, NULL));
	}
      need_closer = 0;
    }
  else if (!np->kids && (!strcmp(nodename, "g:s") || !strcmp(nodename, "g:v")
			 || !strcmp(nodename, "g:b") || !strcmp(nodename, "g:m")
			 || !strcmp(nodename, "g:a") || !strcmp(nodename, "g:p")
			 || !strcmp(nodename, "g:r") || !strcmp(nodename, "g:t")
			 ))
    {
      Rats *r = rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO));
      if (1/*oflag*/)
	joxer_eto(np->mloc, nodename, r, np->user ? (ccp)((gvl_g*)np->user)->orig : np->text);
      else
	joxer_et(np->mloc, nodename, r, np->user ? (ccp)((gvl_g*)np->user)->orig : np->text);
      need_closer = 0;
    }
  else if (!strcmp(nodename, "g:s") || !strcmp(nodename, "g:v")
	   || !strcmp(nodename, "g:c") || !strcmp(nodename, "g:g")
	   || !strcmp(nodename, "g:n"))
    /* singleton with kids, i.e., g:b and mods */
    {
      char jname[4]; strcpy(jname,nodename); *jname = 'j';
      joxer_ea(np->mloc, nodename,
	       rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
      joxer_ao(jname);
      need_closer = 4;
    }
  else if (!strcmp(np->name, "g:b"))
    {
      need_closer = 0;
    }
  else
    {      
      joxer_ea(np->mloc, nodename,
	       rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
    }

  for (npp = np->kids; npp; npp = npp->next)
    grx_jox_node(npp, need_closer, 1);

  switch (need_closer)
    {
    case 1:
      joxer_ac();
      joxer_ee(np->mloc, nodename);
      joxer_ac();
      break;
    case 2:
      joxer_ee(np->mloc, nodename);
      break;
    case 3:
      joxer_ee(np->mloc, nodename);
      joxer_ac();
      break;
    case 4:
      joxer_ac();
      joxer_ee(np->mloc, nodename);
      break;
    }

  if (nflag && np->next)
    grx_jox_node(np->next, 0, 1);
}

void
grx_jox(Node *np, const char *gdltag)
{
  if (gdltag)
    joxer_ea(np->mloc, gdltag, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
  if (joxer_mode != JOXER_XML)
    joxer_ao("j:w");
  grx_jox_node(np->kids, 0, 1);
  if (joxer_mode != JOXER_XML)
    joxer_ac();
  if (gdltag)
    joxer_ee(np->mloc, gdltag);
}
