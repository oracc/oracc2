#include <stdio.h>
#include <stdlib.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <dbxlib.h>

const char *dbidir = NULL, *dbiname = NULL;

Dbi_index *
dbx_init(const char *dir, const char *name)
{
  Dbi_index *dbi;
  if (!(dbi = dbi_open(name, dir)))
    fprintf(stderr, "dbx: failed to open %s/%s\n", dir, name);
  return dbi;
}

void
dbx_term(Dbi_index *dbi)
{
  dbi_close(dbi);
}
	 
const char *
dbx_key(Dbi_index *dbi, const char *key)
{
  if (dbi && key)
    {
      dbi_find(dbi,(unsigned const char *)key);
      if (dbi->data)
	return dbi->data;
    }
  return NULL;
}
