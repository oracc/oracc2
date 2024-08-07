#include <string.h>
#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

extern int sll_trace;
extern Dbi_index *sll_db;
extern const char *oraccd;
extern Pool *sllpool;

Dbi_index *
sll_init_d(const char *project, const char *name)
{
  Dbi_index *dbi = NULL;
  char *db;

  sll_init();
  sll_init_si();
    
  /* Figure out the db and open it */
  if (!project)
    project = "osl";

  /* Not actually used any more because name is always "sl" */
  if (!name)
    name = "sl";

  oraccd = oracc_home();
  db = (char*)pool_alloc(strlen(oraccd)+strlen("/pub/sl/") + strlen(project) + 1, sllpool);
  sprintf(db, "%s/pub/%s/sl", oraccd, project);

  if (!(dbi = dbi_open(name, db)))
    fprintf(stderr, "sllib: failed to open %s (project=%s; name=%s)\n", db, project, name);

  return sll_db = dbi;
}

void
sll_term_d(Dbi_index *dbi)
{
  if (dbi)
    {
      dbi_close(dbi);
      sll_term();
    }
}

unsigned char *
sll_lookup_d(Dbi_index *dbi, unsigned const char *key)
{
  if (dbi && key)
    {
      dbi_find(dbi,key);
      if (dbi->data)
	return pool_copy(dbi->data, sllpool);
    }
  return NULL;
}

