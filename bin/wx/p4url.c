#include <oraccsys.h>
#include "px.h"
#include "p4url.h"
#include "ccgi/ccgi.h"

typedef struct p4bits
{
  const char *project;
  const char *file;
  const char *one;
  const char *two;
  const char *bad;
} P4bits;

static int p4url_is_glossary(const char *l);
static int p4url_is_glossaryid(const char *i);
static int p4url_is_project(char *p, P4bits *b);
static int p4url_is_textid(const char *i);
static int p4url_q(P4url *p);
static int p4url_u(P4url *p);
static int p4url_move_to_qs(P4bits *bp, P4url *p);
static int p4url_is_urlkey(const char *k);
static int p4url_file(const char *f, P4bits *b);

static int p4url_arg_equals = 0;

#undef uc
#define uc unsigned char

/**p4url--validate a new P4 url
 *
 * P4 URLs have the data in the URL (u) and aditional parameters in
 * the QUERY_STRING (q).
 *
 * A GDFID is a string of digits used as an ID in an ood project.
 *
 * u is:
 *
 *	  PROJECT
 * 	| PROJECT ('/' (PQXID|GDFID))?
 *	| PROJECT ('/' LANGUAGE)? ('/' OXID)?
 *	| PROJECT '/' FILE
 *
 *
 * q is:
 *
 *	(ARG ('&' ARG)*)?
 *
 * ARG is:
 *
 *	  OPTION
 *	| OPTION=VALUE
 *
 */
int
p4url(P4url *p, const char *u, const char *q)
{
  if (u)
    {
      p->u = strdup(u);
      if (q)
	p->q = strdup(q);
      if (p4url_u(p))
	return 1;
    }
  if (p->adhoc && !p->q)
    p->q = "adhoc";
  if (p->q)
    if (p4url_q(p))
      return 1;
  return 0;
}

static int
p4url_u(P4url *p)
{
  P4bits bits = { NULL , NULL , NULL , NULL };
  p4url_is_project(p->u, &bits);
  if (bits.file)
    {
      p->project = bits.project;
      p->file = bits.file;      
      if (bits.one && _is_ncname(bits.one))
	{
	  p->frag = bits.one;
	  bits.one = NULL;
	}
    }
  else if (bits.project)
    {
      int ret = 0;
      p->project = bits.project;
      if (bits.two)
	{
	  if (p4url_is_glossary(bits.one))
      	    {
	      if (p4url_is_glossaryid(bits.two))
		{
		  p->glossary = bits.one;
		  p->oxid = bits.two;
		}
	      else
		{
		  p->status = 1;
		  p->err = "found good PROJECT and GLOSSARY but bad GLOSSARYID";
		}
	    }
	  else if ((ret = p4url_is_textid(bits.one)))
	    {
	      /* This also returns true for a GDFID so we disambiguate here */
	      if (isdigit(*bits.one))
		p->ood = 1;
	      if (2 == ret)
		p->adhoc = 1;
	      bits.bad = bits.two;
	      p->pxid = bits.one;
	      bits.two = NULL;
	      p4url_move_to_qs(&bits, p);
	    }
	  else
	    {
	      p->status = 1;
	      p->err = "found good PROJECT but bad TEXTID";
	    }
	}
      else if (bits.one)
	{
	  if (!strcmp(bits.one,"pager"))
	    {
	      bits.one = bits.two;
	      bits.two = NULL;
	    }
	  else
	    {
	      if ((ret = p4url_is_textid(bits.one)))
		{
		  /* This also returns true for a GDFID so we disambiguate here */
		  if (isdigit(*bits.one))
		    p->ood = 1;
		  if (2 == ret)
		    p->adhoc = 1;
		  p->pxid = bits.one;
		}
	      else if (p4url_is_glossaryid(bits.one))
		{
		  p->oxid = bits.one;
		}
	      else if (p4url_is_urlkey(bits.one))
		{
		  /* /rimanum/sig is not a glossary; check if bits.one is a urlkey */
		  bits.bad = bits.one;
		  bits.one = NULL;
		  p4url_move_to_qs(&bits, p);
		}
	      else if (p4url_is_glossary(bits.one))
		{
		  p->glossary = bits.one;
		}
	      else
		{
		  p->status = 1;
		  p->err = "found good PROJECT but rest of URL is neither GLOSSARY nor TEXTID";
		}
	    }
	}
    }
  else
    {
      p->status = 1;
      p->err = "no PROJECT found";
    }
  return p->status;
}

static int
p4url_is_urlkey(const char *k)
{
  return NULL != urlkeys(k, strlen(k));
}

static int
p4url_move_to_qs(P4bits *bp, P4url *p)
{
  if (bp->bad)
    {
      char *tmp = strdup(bp->bad);
      char *s = tmp;
      while (*s)
	if ('/' == *s)
	  *s++ = '&';
	else
	  ++s;
      if (p->q)
	{
	  char buf[strlen(tmp)+strlen(p->q)+2];
	  sprintf(buf, "%s&%s", tmp, p->q);
	  free(tmp);
	  free(p->q);
	  p->q = strdup(buf);
	}
      else
	p->q = tmp;
    }
  return 0;
}

static const char *
p4url_arg_tok(const char *name, char *t, const char *allow)
{
  static char *x = NULL;
  char *ret = NULL;
  p4url_arg_equals = 0;
  if (t)
    x = t;
  if (*x)
    {
      /* a token ends with \0 , '&', or '=' */
      char *y = x;
      while (*y && '&' != *y && '=' != *y)
	++y;
      if (*y)
	{
	  if ('=' == *y)
	    p4url_arg_equals = 1;
	  *y++ = '\0';
	}
      ret = x;
      x = y;      
    }
  if (name)
    return p4url_validate(name, CGI_decode_url(ret), allow);
  else
    return CGI_decode_url(ret);
}

static int
p4url_q(P4url *p)
{
  if (p->q)
    {
      extern int qs_total_keywords(void);
      int optmax = qs_total_keywords();
      p->args = calloc(1+optmax, sizeof(P4arg));
      int i = 0;
      char *qs = p->q;
      const char *tok;
      int what_index = -1;
      while ((tok = p4url_arg_tok(NULL,qs,NULL)))
	{
	  struct urlkeytab *ukey = urlkeys(tok, strlen(tok));
	  if (ukey)
	    {
	      p->args[i].option = ukey->option;
	      if (!strcmp(tok, "corpus"))
		p->args[i].value = "pager";
	      else
		p->args[i].value = tok;
	      
	      if (!strcmp(ukey->option, "form"))
		{
		  char Wopt[strlen(tok)+2];
		  strcpy(Wopt, "W");
		  strcat(Wopt, tok);
		  struct urlkeytab *wkey = urlkeys(Wopt, strlen(Wopt));
		  if (wkey)
		    {
		      ++i;
		      p->args[i].option = "what";
		      p->args[i].value = wkey->option;
		      what_index = i;
		    }
		}
	      else if (!strcmp(ukey->option, "what"))
		what_index = i;
#if 0
	      /* 2025-12-21 urlkeys are standalone option terms so they shouldn't
		 take an argument */
	      if (p4url_arg_equals)
		{
		  ++i;
		  p->args[i].option = ukey->name;
		  p->args[i].value = p4url_arg_tok(ukey->name, NULL, NULL);
		}
#endif
	    }
	  else
	    {
	      struct qsoptionstab *qopt = qsoption(tok, strlen(tok));
	      if (qopt)
		{
		  p->args[i].option = tok;
		  p->args[i].value = p4url_arg_tok(tok, NULL, qopt->value);
		  if (!strcmp(tok, "q"))
		    p->exe = QX_EXE;
		}
	      else if (i && !strcmp(p->args[i-1].option, "what"))
		{
		  p->args[i].option = p->args[i-1].value;
		  p->args[i].value = tok;
		  what_index = i;
		}
	      else if (p->file && _is_ncname(tok))
		{
		  p->frag = tok;
		}
	      else
		{
		  p->status = 1;
		  p->err = "invalid option in QUERY_STRING";
		  break;
		}
	    }
	  if (qs)
	    qs = NULL;
	  ++i;
	}
      if (p->adhoc)
	{
	  if (what_index >= 0)
	    p->args[what_index].value = "adhoc";
	  else
	    {
	      p->args[i].option = "what";
	      p->args[i].value = "adhoc";
	      ++i;
	    }
	}
      else if (p->file)
	{
	  if (p->frag)
	    {
	      if (what_index == -1)
		{
		  ++i;
		  p->args[i].option = "what";
		  p->args[i].value = "file";
		  what_index = i;
		}
	    }
	  else
	    {
	      p->status = 1;
	      p->err = "file requested but no legal frag ID found in QUERY_STRING";
	    }
	}
      p->nargs = i;
    }
  
  return p->status;
}

static int
p4url_is_glossaryid(const char *i)
{
  if (i && ('o' == i[0] || 'x' == i[0]))
    {
      ++i;
      while (*i && isdigit((uc)*i))
	++i;
      if (!*i)
	return 1;
    }
  return 0;
}

static int
p4url_is_textid(const char *i)
{
  static char pqx[256] = { ['P']=1, ['Q']=1, ['X']=1 };
  if (pqx[(uc)*i])
    {
      ++i;
      char *dot = strchr(i, '.');
      if (dot)
	{
	  if (dot - i == 6)
	    {
	      while (1)
		{
		  while (isdigit((uc)*i))
		    ++i;
		  if (!*i)
		    return 1;
		  else
		    {
		      while ('l' == *i) /* add other possible ID suffix characters here */
			++i;		      
		    }
		  if (!*i)
		    return 1;
		  else if ('.' == *i)
		    ++i;
		  else if (',' == *i)
		    return 2;
		  else
		    break;
		}
	    }
	  else
	    {
	      char *uscore = strchr(i, '_');
	      if (uscore)
		return 1;
	    }
	}
      else if ((dot = strchr(i, ',')))
	{
	  while (1)
	    {
	      while (isdigit((uc)*i))
		++i;
	      if (!*i)
		return 1;
	      else if (',' == *i)
		return 2;
	      else
		break;
	    }
	}
      else
	{
	  if (strlen(i) == 6)
	    {
	      while (isdigit((uc)*i))
		++i;
	      if (!*i)
		return 1;
	    }
	}
    }
  else
    {
      while (isdigit(*i))
	++i;
      if (!(*i))
	return 1;
    }
  return 0;
}

static int
p4url_is_glossary(const char *l)
{
  if (l)
    {
      const char *sentinel = l + 3;
      while (l < sentinel && islower((uc)*l))
	++l;
      if (*l)
	{
	  if ('-' == l[0] && 'x' == l[1] && '-' == l[2])
	    {
	      l += 3;
	      sentinel = l + 6;
	      while (l < sentinel && islower((uc)*l))
		++l;
	      if (*l)
		{
		  if ('-' == l[0])
		    {
		      ++l;
		      sentinel = l + 3;
		      while (l < sentinel && isdigit((uc)*l))
			++l;
		      if (!*l)
			return 1;
		    }
		}
	      else
		return 1;
	    }
	}
      else
	return 1;
    }
  return 0;
}

/* Return a pointer to the end of string or the character after the
 * '/' that ends the project portion of the SCRIPT_URL
 */
static int
p4url_is_project(char *p, P4bits *b)
{
  struct stat st_buf;

  if ('/' == *p)
    ++p;
  
  char buf[strlen(docroot)+strlen(p)+2];
  sprintf(buf, "%s/%s", docroot, p);

  if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
    {
      b->project = p;
      return 1;
    }

  /* We recognize at most PROJECT + GLOSSARY + ID; the strchr in this
   * function must be on the arg p not on buf
   *
   * 2025-03-11: add support to recognize .xml or .html file name
   * after project; note that this is only exercised if there is a
   * QUERY_STRING--otherwise the apache config is set up to return the
   * file directly.
   *
   * In the current implementation files can have at most one
   * directory beyond project, e.g.,
   *
   *   easl/sltab.html or easl/files/sltab.html
   *
   * only files ending with .html or .xml are allowed.
   *
   */
  char *slash1 = strrchr(p, '/');

  if (slash1)
    {
      *slash1 = '\0'; 
      sprintf(buf, "%s/%s", docroot, p);
      if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
	{
	  b->project = p;
	  b->one = slash1+1;
	  if (p4url_file(b->one, b))
	    b->one = NULL;
	  return 1;
	}
      char *slash2 = strrchr(p, '/');
      if (slash2)
	{
	  *slash2 = '\0';
	  sprintf(buf, "%s/%s", docroot, p);
	  if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
	    {
	      b->project = p;
	      b->one = slash2+1;
	      b->two = slash1+1;
	      if (p4url_file(b->one, b))
		{
		  b->one = b->two;
		}
	      else
		{
		  *slash2 = '/';
		  if (p4url_file(b->one, b))
		    b->one = b->two = NULL;
		  else
		    *slash2 = '\0';
		}
	      return 1;
	    }
	}
    }
  return 0;
}

static int
p4url_file(const char *f, P4bits *b)
{
  char fbuf[strlen("/home/oracc/www//0")+strlen(b->project)+strlen(f)];
  sprintf(fbuf, "/home/oracc/www/%s/%s", b->project, f);
  if (!access(fbuf, R_OK))
    {
      const char *dot = strrchr(fbuf,'.');
      if (dot && (!strcmp(dot,".xml") || !strcmp(dot,".html") || !strcmp(dot,".xhtml")))
	{
	  b->file = strdup(fbuf);
	  return 1;
	}
    }
  return 0;
}
