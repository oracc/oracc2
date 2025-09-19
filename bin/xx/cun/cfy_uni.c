#include <oraccsys.h>
#include <runexpat.h>
#include "cfy.h"

static void
cfy_uni_check_one(Cfy *c, uccp u)
{
  if (!hash_find(c->c->fntp->uni_seen, u))
    {
      hash_add(c->c->fntp->uni_seen, u, "");
      if (!hash_find(c->c->fntp->uni, u))
	{
	  wchar_t w = utf1char(u, NULL);
	  if (c->cov_list)
	    fprintf(stderr,
		    "%s:%d: list %s missing U+%X=%s\n",
		    pi_file, pi_line, c->cov_list, (unsigned)w, u);
	  else
	    fprintf(stderr,
		    "%s:%d: font %s missing U+%X=%s\n",
		    pi_file, pi_line, c->c->fntp->full, (unsigned)w, u);
	}
    }
}

void
cfy_uni_check(Cfy *c, uccp u)
{
  if (strlen((ccp)u) == 4)
    {
      cfy_uni_check_one(c, u);
    }
  else
    {
      if (!hash_find(c->c->fntp->uni_seen, (uccp)u))
	{
	  size_t ulen;
	  wchar_t *w = utf2wcs(u, &ulen);

	  /* Add sequence as well as individual units */
	  hash_add(c->c->fntp->uni_seen, (uccp)u, "");

	  int i;
	  wchar_t ww[2]; ww[1] = '\0';
	  for (i = 0; i < ulen; ++i)
	    {
	      ww[0] = w[i];
	      cfy_uni_check_one(c, wcs2utf(ww, 1));
	    }
	}
    }
}

Hash *
cfy_uni_load(Cfy *c, const char *unifile)
{
  if (!access(unifile, R_OK))
    {
      Hash *u = hash_create(4096);
      size_t nl;
      uchar **l = loadfile_lines((uccp)unifile, &nl);
      int i;
      for (i = 0; i < nl; ++i)
	hash_add(u, l[i], "");
      return u;
    }
  else
    {
      fprintf(stderr, "cfy_uni_load: coverage checking deactivated; %s not found\n", unifile);
      c->coverage = 0;
      return NULL;
    }
}
