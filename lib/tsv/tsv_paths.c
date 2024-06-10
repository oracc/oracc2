#include <oraccsys.h>
#include "tsv.h"

/**tsv_paths
 * call as
 *
 *   tsv_paths(tp, FNAME, NULL, NULL)
 *
 * to set name/dir from tsv file
 *
 * or as
 *
 *   tsv_paths(tp, NULL, name, dir)
 *
 * to set db paths explicitly
 *
 */

void
tsv_paths(Tsv *tp, const char *tsvfn, const char *name, const char *dir)
{
  char *tmp;
  tp->tsv_fn = tsvfn;
  if (name && dir)
    {
      tp->tdb_name = name;
      tp->tdb_dir = dir;
      tp->free_dir = 0;
    }
  else if (tsvfn)
    {
      tp->tdb_dir = malloc(strlen(tsvfn)+1);
      strcpy((char*)tp->tdb_dir, tsvfn);
      if ((tmp = strrchr(tp->tdb_dir, '/')))
	{
	  *tmp++ = '\0';
	  tp->tdb_name = tmp;
	  tp->free_dir = 1;
	}
      else
	{
	  tp->tdb_name = tp->tdb_dir;
	  tp->free_name = 1;
	  tp->free_dir = 0;
	  if (dir)
	    tp->tdb_dir = dir;
	  else
	    tp->tdb_dir = ".";
	}
    }
}

