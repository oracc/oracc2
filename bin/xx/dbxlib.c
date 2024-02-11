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
	 
const void *
dbx_key(Dbi_index *dbi, const char *key, Unsigned32 *n)
{
  if (dbi && key)
    {
      dbi_find(dbi,(unsigned const char *)key);
      if (dbi->data)
	{
	  if (n)
	    *n = dbi->nfound;
	  return dbi->data;
	}
    }
  return NULL;
}

void
dbx_vido(Dbi_index *dp)
{
  if (dp && dp->h.vids[0])
    {
      char vido_fn[_MAX_PATH];
      if ('/' != dp->h.vids[0])
	strcat(strcpy(vido_fn, dp->dir), (ccp)dp->h.vids);
      else
	strcpy(vido_fn, (ccp)dp->h.vids);
      if (!access(vido_fn, R_OK))
	dp->vp = vido_load_data(vido_fn, 0);
      else
	dp->vp = NULL;
    }
  else
    dp->vp = NULL;
}
