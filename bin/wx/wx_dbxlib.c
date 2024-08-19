#include <stdio.h>
#include <stdlib.h>
#include <oracclocale.h>
#include <wx_dbxlib.h>

const char *dbidir = NULL, *dbiname = NULL;

Dbi_index *
wx_dbx_proj_oid(const char *project)
{
  char projoiddir[ strlen(oracc_builds()) + strlen(project) + strlen("02pub") + 3];
  sprintf(projoiddir, "%s/%s/02pub", oracc_builds(), project);
  return wx_dbx_init(projoiddir, "oid");
}

Dbi_index *
wx_dbx_init(const char *dir, const char *name)
{
  Dbi_index *dbi;
  if (!(dbi = dbi_open(name, dir)))
    fprintf(stderr, "dbx: failed to open %s/%s\n", dir, name);
  return dbi;
}

void
wx_dbx_term(Dbi_index *dbi)
{
  dbi_close(dbi);
}
	 
const char *
wx_dbx_key(Dbi_index *dbi, const char *key)
{
  if (dbi && key)
    {
      dbi_find(dbi,(unsigned const char *)key);
      if (dbi->data)
	return dbi->data;
    }
  return NULL;
}
