#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>
#include <tis.h>

static void sx_merge_load(struct sl_signlist *sl);
#if 0
static void sx_merge_marshall(struct sl_signlist *sl);
#endif

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

void
sx_merge_subsl(struct sl_signlist *sl)
{
  if (sxconfig.merge)
    {
      sx_merge_load(sl);
#if 0
      sx_merge_marshall(sl);
#endif
    }
}

/**Mergers are specified as:
 *
 * NI₂ IM
 *
 * h_merges indexes each merge-head to the list of mergers, i.e., NI₂ => IM
 *
 */
static void
sx_merge_load(struct sl_signlist *sl)
{
  unsigned char *fmem, **lp;
  size_t nline;
  sl->h_merges = hash_create(256);

  lp = loadfile_lines3((uccp)sx_script_merge_fn(sl, sxconfig.merge), &nline, &fmem);
  int i;
  for (i = 0; i < nline; ++i)
    {
      if ('o' == lp[i][0]) /* ignore o0000237++      o0000237 o0002827 lines */
	continue;
      unsigned char *v = lp[i];
      while (*v && !isspace(*v))
	++v;
      if (*v)
	{
	  *v++ = '\0';
	  while (isspace(*v))
	    ++v;
	  if (*v)
	    {
	      /*hash_add(sl->h_merges, lp[i], v);*/
	      char *vv = strdup((ccp)v);
	      char **mm = space_split(vv);
	      int j;
	      List *need = list_create(LIST_SINGLE);
	      for (j = 0; mm[j]; ++j)
		{
		  struct sl_sign *sp = hash_find(sl->hsentry, (uccp)mm[j]);
		  if (sp)
		    {
		      char buf[12];
		      sprintf(buf, "%s..", sp->oid);
		      if (hash_find(sl->h_kdata, (uccp)buf))
			list_add(need, mm[j]);
		    }
		  else
		    {
		      mesg_verr(mesg_mloc(sxconfig.merge,i+1), "%s: unknown sign in merge\n", mm[j]);
		    }
		}
	      if (list_len(need))
		hash_add(sl->h_merges, lp[i], list_to_str(need));
	    }
	}
    }
}

#if 0
static void
sx_merge_marshall(struct sl_signlist *sl)
{
  if (sl->h_kdata)
    {
      Hash *m_seen = hash_create(256);
      Hash *mheads = hash_create(256);
      const char **mk = hash_keys(sl->h_merges_cand);
      if (mk)
	{
	  int i;
	  /* go through the signs which need to be merged and keep those which
	     occur in the .key list */
	  for (i = 0; mk[i]; ++i)
	    {
	      char buf[12];
	      sprintf(
	      if (hash_find(sl->hsentry, (uccp)mk[i]))
		{
		  hash_add(m_seen, (uccp)mk[i], "");
		  hash_add(mheads, hash_find(sl->h_merges_cand, (void*)mk[i]), "");
		}
	      /* Error condition: signs must be in signlist */
	    }
      
	  /* go through the merge-heads and reduce their lists to the
	     merge cands that occur; if a merge-head isn't already known
	     in a subset, add it */
	  mk = hash_keys(mheads);
	  for (i = 0; mk[i]; ++i)
	    {
	      /* If we are subsetting (normal case) check that the merge
		 head was included in the .key list and add it if it
		 wasn't */
	      struct sl_sign *sp = hash_find(sl->hsentry, (uccp)mk[i]);
	      char key[12];
	      sprintf(key, "%s..", sp->oid);
	      if (!hash_find(sl->h_kdata, (uccp)key))
		{
		  struct tis_data *tp = memo_new(sl->m_idata);
		  tp->key = (char*)pool_copy((ucp)key, sl->p);
		  hash_add(sl->h_kdata, (uccp)tp->key, tp);
		}

	      /* Check the list of signs that can merge into this head; if
		 there is more than one, reduce the list to the actual
		 signs that occur in the subset */
	      char *m = hash_find(sl->h_merges,(uccp) mk[i]);
	      if (strchr(m, ' '))
		{
		  char **mm = space_split(m);
		  List *ml = list_create(LIST_SINGLE);
		  int i;
		  for (j = 0; mm[j]; ++j)
		    {
		      if (hash_find(sl->h_kdata, (uccp)mm[i]))
			{
			  list_add(ml, mm[j]);
			  hash_add(sl->h_merges_seen, (uccp)mm[j], "");
			}
		    }
		  unsigned char *newm = list_to_str(ml);
		  if (strcmp(m, (ccp)newm))
		    hash_add(sl->h_merges, (uccp)mk[i], newm);
		}
	    }
	}
    }
}

#endif
