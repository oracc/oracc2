#include <oraccsys.h>
#include <gvl.h>

const char *gvl_uname_prefix = "CUNEIFORM";
int gvl_uname_compound = 0;
int gvl_uname_grouped = 0;

static void gvl_uname_descend(Node *np, List *lp);
static void gvl_uname_node(Node *np, List *lp);

static void
gvl_uname_node(Node *np, List *lp)
{
  if (!strncmp(np->name, "g:", 2))
    {
      switch (np->name[2])
	{
	case 'c':
	  break;
	case 'b':
	case 'l':
	case 'p':
	case 's':
	case 'v':
	  if (!np->kids)
	    list_add(lp, (void*)np->text);
	  break;
	case 'd':
	  {
	    const char *d = NULL;
	    switch (*((unsigned char *)np->text))
	      {
	      case '.':
		if (gvl_uname_grouped)
		  d = "PLUS";
		else
		  d = "BESIDE";
		break;
	      case '+':
		d = "JOINING";
		break;
	      case '%':
		d = "CROSSING";
		break;
	      case '&':
		d = "OVER";
		break;
	      case '@':
		d = "OPPOSING";
		break;
	      case 0xC3:
		d = "TIMES";
		break;
	      default:
		fprintf(stderr, "d text %s not handled\n", np->text);
		break;
	      }
	    if (d)
	      list_add(lp, (void*)d);
	  }
	  break;
	case 'a':
	  list_add(lp, "\b-");
	  char *s = malloc(strlen(np->text)+2), *t;
	  s[0] = '\b';
	  int i;
	  for (i = 1, t = np->text; *t; ++t)
	    s[i++] = toupper(*t);
	  s[i] = '\0';
	  list_add(lp, s);
	  break;
	case 'm':
	  {
	    const char *m = NULL;
	    switch (*np->text)
	      {
	      case 'g':
		m = "GUNU";
		break;
	      case 's':
		m = "SHESHIG";
		break;
	      case 't':
		m = "TENU";
		break;
	      default:
		fprintf(stderr, "d text %s not handled\n", np->text);
		break;
	      }
	    if (m)
	      list_add(lp, (void*)m);
	  }
	  break;
	case 'n':
	  break;
	case 'q':
	  break;
	default:
	  break;
	}
    }
}

static void
gvl_uname_descend(Node *np, List *lp)
{
  Node *npp;
  for (npp = np->kids; npp; npp = npp->next)
    {
      if (!strcmp(npp->name, "g:c"))
	{
	  gvl_uname_compound = 1;
	  gvl_uname_descend(npp, lp);
	  gvl_uname_compound = 0;
	}
      else if (!strcmp(npp->name, "g:gp"))
	{
	  gvl_uname_grouped = 1;
	  gvl_uname_descend(npp, lp);
	  gvl_uname_grouped = 0;
	}
      else
	{
	  gvl_uname_node(npp, lp);
	  if (npp->kids)
	    gvl_uname_descend(npp, lp);
	}
    }
}

const char *
gvl_uname_tree(Tree *tp)
{
  if (tp && tp->root && tp->root->kids)
    {
      const char *uname = NULL;
      Node *np;
      List *lp = list_create(LIST_SINGLE);
      list_add(lp, (void*)gvl_uname_prefix);
      if (!strcmp(tp->root->kids->name, "g:n"))
	list_add(lp, (void*)"NUMBER");
      else
	list_add(lp, (void*)"SIGN");
      for (np = tp->root->kids; np; np = np->next)
	{
	  if (!strcmp(np->name, "g:c"))
	    {
	      gvl_uname_compound = 1;
	      gvl_uname_descend(np, lp);
	      gvl_uname_compound = 0;
	    }
	  else if (!strcmp(np->name, "g:gp"))
	    {
	      gvl_uname_grouped = 1;
	      gvl_uname_descend(np, lp);
	      gvl_uname_grouped = 0;
	    }
	  else
	    {
	      gvl_uname_node(np, lp);
	      if (np->kids)
		gvl_uname_descend(np, lp);
	    }
	}
      uname = (const char *)list_join(lp, " ");
      list_free(lp, NULL);
      return uname;
    }
  return NULL;
}
