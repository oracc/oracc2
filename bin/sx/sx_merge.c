#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

void
sx_merge(struct sl_signlist *sl)
{
  unsigned const char **k = (uccp*)hash_keys(sl->h_merge);
  int i;
  for (i = 0; k[i]; ++i)
    {
      /* k[i] is the sign that is merging; m is the destination
	 expressed as an Mloc with the sign name in m->user */
      Mloc *m = hash_find(sl->h_merge, k[i]);
      struct sl_sign *s = hash_find(sl->hsentry, m->user);
      if (s)
	{
	  struct sl_sign *m_from = hash_find(sl->hsentry, k[i]);
	  /* the 'to' may be in a corpus-based signlist without the 'from' being in there */
	  if (m_from)
	    m_from->moid = s->oid;
	}
      else if (!strcmp(sl->project, "osl")) /* corpus-based signlists may legit not have the to or from components */
	mesg_verr(m, "@merge %s: merge destination %s does not exist", m->user, m->user);
    }
}
