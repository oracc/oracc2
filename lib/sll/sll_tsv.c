#include <string.h>
#include <ctype128.h>
#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

extern int sll_trace;
extern Hash *sll_sl;
extern const char *oraccd;
extern Pool *sllpool;
static Hash *h = NULL;

Hash *
sll_init_t(const char *project, const char *name)
{
  char *tsv_file;
  unsigned char *tsv_data = NULL, *p;
  ssize_t fsiz;

  sll_init();
  sll_init_si();

  /* Figure out the db and open it */
  if (!project)
    project = "osl";

  /* Not actually used any more because name is always "sl" */
  if (!name)
    name = "sl";

  oraccd = oracc_home();
  tsv_file = (char *)pool_alloc(strlen(oraccd)+strlen("/pub/sl/") + strlen(project) + strlen("/sl.tsv") + 1, sllpool);
  sprintf(tsv_file, "%s/pub/%s/sl/sl.tsv", oraccd, project);

  tsv_data = slurp("sll", tsv_file, &fsiz);
  if (tsv_data)
    {
      sllpool = pool_init();
  
      if (sll_trace)
	fprintf(stderr, "sll: slurped %s\n", tsv_file);
      h = hash_create(1024);
      for (p = tsv_data; *p; )
	{
	  unsigned char *k = p, *v = NULL;

	  /* The #letters appendix in sl.tsv is not used in the
	     internalized signlist only in the dbi */
	  if ('#' == *p && !strncmp((ccp)p, "#letters", strlen("#letters")))
	    break;

	  while (*p && '\t' != *p)
	    ++p;
	  if (*p)
	    {
	      *p++ = '\0';
	      v = p;
	      while (*p && '\n' != *p)
		++p;
	      if ('\n' == *p)
		*p++ = '\0';
	      if (v)
		{
		  if (sll_trace)
		    fprintf(stderr, "sll: adding %s = %s\n", k, v);
		  hash_add(h, k, v);
		}
	    }
	}
    }
  else
    {
      fprintf(stderr, "sll_init_t: failed to load %s\n", tsv_file);
    }

  return sll_sl = h;
}

void
sll_term_t(Hash *h)
{
  hash_free(h, NULL);
  sll_term();
}

unsigned char *
sll_lookup_t(unsigned const char *key)
{
  return key ? hash_find(sll_sl, key) : NULL;
}
