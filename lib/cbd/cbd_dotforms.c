#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include "cbd.h"

static unsigned char *fmem = NULL;
static Hash *hcbd_df;
static Hash *hcbd_df_seen;

static Cform *
cbd_df_parse(unsigned char *e, unsigned char *f, const char *fn, int i)
{
  Cform *cfp;
  cfp = memo_new(csetp->cformsmem);
  cfp->f.file = (ucp)fn;
  cfp->f.lnum = i;
  cfp->f.form = f;
  while (*f && !isspace(*f))
    ++f;
  if (*f)
    {
      *f++ = '\0';
      while (isspace(*f))
	++f;
      while (*f)
	{
	  switch (*f)
	    {
	    case '%':
	      cfp->f.lang = ++f;
	      break;
	    case '/':
	      cfp->f.base = ++f;
	      break;
	    case '+':
	      cfp->f.cont = ++f;
	      break;
	    case '#':
	      if ('#' == f[1])
		cfp->f.morph2 = f+2, ++f, ++f;
	      else
		cfp->f.morph = ++f;
	      break;
	    case '$':
	      cfp->f.norm = ++f;
	      {
		unsigned char *f2 = f;
		while (f2 == (ucp)strstr((ccp)f2, "$("))
		  {
		    f = f2;
		    ++f2;
		  }
	      }
	      break;
	    case '*':
	      cfp->f.stem = f;
	      break;
	    default:
	      fprintf(stderr, "%s:%d: bad key character `%c' in @form\n", fn, i, *f);
	      return NULL;
	    }
	  while (*f && !isspace(*f))
	    ++f;
	  if (*f)
	    while (isspace(*f))
	      ++f;
	}
    }
    
  return cfp;
}

static void
cbd_df_load_one(unsigned char *f, const char *fn, int i)
{
  unsigned char *atform = (ucp)strstr((ccp)f, "@form");
  if (atform)
    {
      unsigned char *preform = atform, *pstform = atform;
      while (preform > f && isspace(preform[-1]))
	--preform;
      if (preform == f)
	fprintf(stderr, "%s:%d: dot form line has no CF [GW] POS\n", fn, i);
      else
	*preform = '\0';
      while (*pstform && !isspace(*pstform))
	++pstform;
      if (*pstform)
	{
	  while (isspace(*pstform))
	    ++pstform;
	  List *lp = hash_find(hcbd_df, f);
	  if (!lp)
	    hash_add(hcbd_df, f, (lp = list_create(LIST_SINGLE)));
	  Cform *cfp = cbd_df_parse(f, pstform, fn, i);
	  if (cfp)
	    list_add(lp, cfp);
	}
      else
	fprintf(stderr, "%s:%d: dot form line has no form data\n", fn, i);
    }
  else
    fprintf(stderr, "%s:%d: no @form in dot forms line\n", fn, i);
}

int
cbd_df_load(const char *fn)
{
  size_t n;

  hcbd_df = hash_create(1024);
  hcbd_df_seen = hash_create(1024);
  
  unsigned char **dotforms = loadfile_lines3((uccp)fn, &n, &fmem);
  if (dotforms)
    {
      int i;
      for (i = 0; i < n; ++i)
	cbd_df_load_one(dotforms[i], fn, i);
      return 0;
    }
  return 1;
}

void
cbd_df_free(void)
{
  hash_free(hcbd_df, (hash_free_func*)list_free);
  hash_free(hcbd_df_seen, NULL);
}

List *
cbd_df_get(unsigned const char *cgp)
{
  List *lp = hash_find(hcbd_df, cgp);
  if (lp)
    hash_add(hcbd_df_seen,
	     hash_exists(hcbd_df, cgp),
	     ""); 			/* hash_exists returns the
					   hash table key which we
				           know is non-volatile memory
	       				 */
  return lp;
}

List *
cbd_df_unused(void)
{
  const char **keys = hash_keys(hcbd_df);
  int i;
  List *lp = list_create(LIST_SINGLE);
  for (i = 0; keys[i]; ++i)
    if (!hash_exists(hcbd_df_seen, (uccp)keys[i]))
      list_add(lp, (void*)keys[i]);
  return lp;
}
