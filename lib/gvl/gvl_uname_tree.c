#include <oraccsys.h>
#include <unidef.h>
#include <gvl.h>

const char *gvl_uname_curr = NULL;
const char *gvl_uname_prefix = "CUNEIFORM";
int gvl_uname_compound = 0;
int gvl_uname_grouped = 0;

static void gvl_uname_descend(Node *np, List *lp);
static void gvl_uname_node(Node *np, List *lp);

static char *
upify(const char *t)
{
  char *s = malloc(strlen(t)+2), *orig;
  orig = s;
  while (*t)
    *s++ = toupper(*t++);
  *s = '\0';
  return orig;
}

static char *
gvl_uname_ascii(const char *t)
{
  char *ret = malloc(strlen(t)+1);
  wchar_t *w;
  size_t len;
  
  if ((w = utf2wcs((uccp)t, &len)))
    {
      int i;
      char *dest = ret;
      for (i = 0; w[i]; ++i)
	{
	  switch (w[i])
	    {
	    case U_SZ:
	      *dest++ = 'S';
	      *dest++ = 'H';
	      break;
	    case U_s0:
	    case U_s1:
	    case U_s2:
	    case U_s3:
	    case U_s4:
	    case U_s5:
	    case U_s6:
	    case U_s7:
	    case U_s8:
	    case U_s9:
	      *dest++ = (w[i] - 0x2080) + '0';
	      break;
	    case U_s_x:
	      *dest++ = 'X';
	      break;
	    case U_aleph:
	      /* drop */
	      break;
	    default:
	      *dest++ = (char)w[i];
	      break;
	    }
	}
      *dest = '\0';
    }
  return ret;
}

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
	    list_add(lp, gvl_uname_ascii(np->text));
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
		fprintf(stderr, "%s: d text %s not handled\n", gvl_uname_curr, np->text);
		break;
	      }
	    if (d)
	      list_add(lp, (void*)d);
	  }
	  break;
	case 'a':
	  {
	    if (np->prev && !strcmp(np->prev->name, "g:gp"))
	      {
		list_add(lp, "FORM");
		list_add(lp, upify(np->text));
	      }
	    else
	      {
		const char *t;	  
		if ('v' != *np->text || !np->text[0])
		  {
		    list_add(lp, "\b-");
		    char *s = malloc(strlen(np->text)+2);
		    s[0] = '\b';
		    int i;
#if 1
		    for (i = 1, t = np->text; *t; ++t)
		      s[i++] = toupper(*t);
#else
		    for (i = 1, t = np->text; *t; ++t)
		      if (isdigit(*t) && !t[1])
			break;
		      else
			s[i++] = toupper(*t);
#endif
		    s[i] = '\0';
		    list_add(lp, s);
		  }
		else
		  {
		    t = np->text + 1;
		  }
		if (*t)
		  {
		    list_add(lp, "VARIANT");
		    list_add(lp, (void*)t);
		  }
	      }
	  }
	  break;
	case 'm':
	  {
	    const char *m = NULL;
	    switch (*np->text)
	      {
	      case 'g':
		m = "GUNU";
		break;
	      case 'n':
		m = "NUTILLU";
		break;
	      case 'r':
		m = "REVERSED";
		break;
	      case 's':
		m = "SHESHIG";
		break;
	      case 't':
		m = "TENU";
		break;
	      default:
		fprintf(stderr, "%s: m text %s not handled\n", gvl_uname_curr, np->text);
		break;
	      }
	    if (m)
	      list_add(lp, (void*)m);
	  }
	  break;
	case 'n':
	  /* for sign names we can assume this is a well-formed g:r and g:s */
	  {
	    char *nn[] = { "ZERO" , "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE" };
	    Node *r = np->kids;
	    const char *s = np->kids->next->text;
	    if ('N' == *s) {
	      ++s;
	      if ('0' == *s)
		++s;
	    }
	    char *n = malloc(strlen(s)+strlen("NTHREE-0"));
	    sprintf(n, "%s-N%s", isdigit(*r->text)?nn[*r->text - '0']:r->text, s);
	    list_add(lp, n);
	  }
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
	  if (strcmp(npp->name, "g:n"))
	    {
	      if (npp->kids)
		gvl_uname_descend(npp, lp);
	    }
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
      gvl_uname_curr = tp->root->text;
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
	      if (strcmp(np->name, "g:n"))
		{
		  if (np->kids)
		    gvl_uname_descend(np, lp);
		}
	    }
	}
      uname = (const char *)list_join(lp, " ");
      list_free(lp, NULL);
      return uname;
    }
  return NULL;
}
