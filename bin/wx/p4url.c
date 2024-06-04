#include <oraccsys.h>
#include "p4url.h"

typedef struct p4bits { const char *project; const char *one; const char *two; } P4bits;

static int p4url_is_glossary(const char *l);
static int p4url_is_glossaryid(const char *i);
static int p4url_is_project(char *p, P4bits *b);
static int p4url_is_textid(const char *i);
static int p4url_q(P4url *p, const char *q);
static int p4url_u(P4url *p, const char *u);

#undef uc
#define uc unsigned char

/**p4url--validate a new P4 url
 *
 * P4 URLs have the data in the URL (u) and aditional parameters in
 * the QUERY_STRING (q).
 *
 * u is:
 *
 *	  PROJECT
 * 	| PROJECT ('/' PQXID)?
 *	| PROJECT '/' LANGUAGE ('/' OXID)?
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
    if (p4url_u(p,u))
      return 1;
  if (q)
    if (p4url_q(p,q))
      return 1;
  return 0;
}

static int
p4url_u(P4url *p, const char *u)
{
  P4bits bits = { NULL , NULL , NULL };
  p->u = strdup(u);
  p4url_is_project(p->u, &bits);
  if (bits.project)
    {
      p->project = bits.project;
      if (bits.two)
	{
	  if (p4url_is_glossary(bits.one))
      	    {
	      if (p4url_is_glossaryid(bits.two))
		{
		  p->glossary = bits.one;
		  p->item = bits.two;
		}
	      else
		{
		  p->status = 1;
		  p->err = "found good PROJECT and GLOSSARY but bad GLOSSARYID";
		}
	    }
	  else
	    {
	      p->status = 1;
	      p->err = "found good PROJECT but bad TEXTID";
	    }
	}
      else if (bits.one)
	{
	  if (p4url_is_textid(bits.one))
	    {
	      p->item = bits.one;
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
  else
    {
      p->status = 1;
      p->err = "no PROJECT found";
    }
  return p->status;
}

static const char *
p4url_arg_tok(char *t)
{
  static char *x = NULL;
  char *ret = NULL;
  if (t)
    x = t;
  if (*x)
    {
      /* a token ends with \0 , '&', or '=' */
      char *y = x;
      while (*y && '&' != *y && '=' != *y)
	++y;
      *y++ = '\0';
      ret = x;
      x = y;      
    }
  return ret;
}

static int
p4url_q(P4url *p, const char *q)
{
  if (q)
    {
      extern int qs_total_keywords(void);
      int optmax = qs_total_keywords();
      p->q = strdup(q);
      p->args = calloc(1+optmax, sizeof(P4arg));
      int i = 0;
      char *qs = p->q;
      const char *tok;
      while ((tok = p4url_arg_tok(qs)))
	{
	  struct urlkeytab *ukey = urlkeys(tok, strlen(tok));
	  if (ukey)
	    {
	      p->args[i].option = ukey->option;
	      p->args[i].value = tok;
	    }
	  else
	    {
	      struct qsoptionstab *qopt = qsoption(tok, strlen(tok));
	      if (qopt)
		{
		  p->args[i].option = tok;
		  p->args[i].value = p4url_arg_tok(NULL);
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
		  else if ('.' == *i)
		    ++i;
		  else
		    break;
		}
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
 * '/' that ends the project portion of the SCRIPT_URL belong to the
 */
static int
p4url_is_project(char *p, P4bits *b)
{
  struct stat st_buf;

  char buf[strlen(docroot)+strlen(p)+2];
  sprintf(buf, "%s/%s", docroot, p);

  if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
    {
      b->project = p;
      return 1;
    }

  /* We recognize at most PROJECT + GLOSSARY + ID; the strchr in this
     function must be on the arg p not on buf */
  char *slash1 = strrchr(p, '/');

  if (slash1)
    {
      *slash1 = '\0'; 
      sprintf(buf, "%s/%s", docroot, p);
      if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
	{
	  b->project = p;
	  b->one = slash1+1;
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
	      return 1;
	    }
	}
    }
  return 0;
}
