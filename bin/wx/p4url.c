#include <oraccsys.h>
#include "p4url.h"

Typedef struct p4arg
{
  const char *option;
  const char *value;
} P4arg;

Typedef struct p4url
{
  char *u;	/* copy of URL to chop up */
  char *q;	/* copy of QUERY_STRING to chop up */
  const char *project;
  const char *glossary;
  const char *id;
  P4arg *args;
  const char *err;
  int status;
} P4url;

static void p4url_u(P4url *p, const char *u);
static void p4url_q(P4url *p, const char *q);

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

typedef struct p4bits { const char *one; const char *two; } P4bits;

static void
p4url_u(P4url *p, const char *u)
{
  static P4bits bits = { NULL , NULL };
  p->u = strdup(u);
  p4url_project(u, &p4bits);
  if (p4bits.project)
    {
      p->p = p4bits.project;
      if (p4bits.two)
	{
	  if (p4url_is_glossary(p4bits.one))
      	    {
	      if (p4url_is_glossary_id(p4bits.two))
		{
		  p->glossary = p4bits.one;
		  p->id = p4bits.two;
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
      else if (p4bits.one)
	{
	  if (p4url_is_text_id(p4bits.one))
	    {
	      p->id = p4bits.one;
	    }
	  else if (p4url_is_glossary(p4bits.one))
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
}

static void
p4url_q(P4url *p, const char *q)
{
  
}

static int
p4url_is_id(const char *i)
{
  if (i)
    {
      if ('P' == i[0] || 'Q' == i[0] || 'X' == i[0])
	{
	  ++i;
	  char *dot = strchr(i, '.');
	  if (dot)
	    {
	      if (dot - i == 6)
		{
		  while (1)
		    {
		      while (*i && i >= '0' && i <= '9')
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
		  while (*i && i >= '0' && i <= '9')
		    ++i;
		  if (!*i)
		    return 1;
		}
	    }
	}
      else if ('o' == i[0] || 'x' == i[0])
	{
	  ++i;
	  while (*i && i >= '0' && i <= '9')
	    ++i;
	  if (!*i)
	    return 1;
	}
    }
  return 0;
}

static int
p4url_is_lang(const char *l)
{
  if (l)
    {
      char *sentinel = l + 3;
      while (*l && l < sentinel && *l >= 'a' && *l <= 'z')
	++l;
      if (*l)
	{
	  if ('-' == l[0] && 'x' == l[1] && 'x' == l[2])
	    {
	      l += 3;
	      sentinel = l + 6;
	      while (*l && l < sentinel && *l >= 'a' && *l <= 'z')
		++l;
	      if (*l)
		{
		  if ('-' == l[0])
		    {
		      ++l;
		      sentinel = l + 3;
		      while (*l && l < sentinel && *l >= '0' && *l <= '9')
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
static char *
p4url_is_project(char *p, P4bits *b)
{
  struct stat st_buf;

  char buf[strlen(docroot)+strlen(p)+2];
  sprintf(buf, "%s/%s", docroot, p);

  if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
    return 1;

  /* We recognize at most PROJECT + GLOSSARY + ID */
  char *slash1 = strrchr(buf, '/');

  if (slash)
    {
      *slash1 = '\0';      
      if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
	{
	  b->one = slash1+1;
	  return 1;
	}
      char *slash2 = strrchr(slash1-1, '/');
      if (slash2)
	{
	  *slash2 = '\0';
	  if (!stat(buf, &st_buf) && S_ISDIR(st_buf.st_mode))
	    {
	      b->one = slash2+1;
	      b->two = slash1+1;
	      return 1;
	    }
	}
    }
  return 0;
}

