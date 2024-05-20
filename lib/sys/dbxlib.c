#include <oraccsys.h>
#include <dbxlib.h>

int
dbx_access(const char *dir, const char *name)
{
  char fn[strlen(dir)+strlen(name)+2];
  sprintf(fn, "%s/%s.dbh", dir, name);
  return access(fn, R_OK);
}

Dbi_index *
dbx_init(const char *dir, const char *name)
{
  Dbi_index *dbi;
  if (!(dbi = dbi_open(name, dir)))
    fprintf(stderr, "dbx: failed to open %s/%s\n", dir, name);
  dbx_vido(dbi);
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
  if (dp)
    {
      if (dp->h.vids[0])
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
}

void
dbx_wids(Dbi_index *dp, const Loc8 *l8p, int n, FILE *o)
{
  int i;
  for (i = 0; i < n; ++i)
    {
      const Loc8 *l8 = &l8p[i];
      fprintf(o,"%s.%d.%d",
	      vido_get_id(dp->vp,l8->text_id),
	      l8->unit_id, 
	      l8->word_id
	      );
      if (dp->h.sep_char == ' ')
	{
	  if ((n-i) > 1)
	    fputc(' ', o);
	}
      else
	fputc(dp->h.sep_char, o);
    }
}

void
dbx_l8s(Dbi_index *dp, Loc8 *l8p, int n, FILE *o)
{
  int i;
  fputc('\n', o);
  for (i = 0; i < n; ++i)
    {
      Loc8 *l8 = &l8p[i];
      fprintf(o,
	      "         t=%s;u=%d;w=%d\n",
	      vido_get_id(dp->vp,l8->text_id),
	      l8->unit_id, 
	      l8->word_id
	      );
    }
}
