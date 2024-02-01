#include <oraccsys.h>
#include <asl.h>
#include <tis.h>

#if 0
static void
tis_strip_iss(char *lp)
{
  char *tab = strrchr(lp, '\t');
  *tab = '\0';
}
#endif

/* Read the corpus statistics from a .tis file into a hash-by-oid, sl->idata.
 */
void
sx_idata_init(struct sl_signlist *sl, const char *idata_file, const char *idata_type)
{
  char *lp;
  char type[4];
  struct tis_data t;
  FILE *fp = NULL;

  t.iss = NULL;
  t.subs = NULL;

  sl->h_idata = hash_create(1024);
  sl->m_idata = memo_init(sizeof(struct tis_data), 256);
  
  if (idata_type && strlen(idata_type) < 3)
    sprintf(type, "%s.", idata_type);
  else
    return; /* this is trapped in option processing in sx.c */

  if (!(fp = fopen(idata_file, "r")))
    {
      perror(idata_file);
      return;
    }

#if 1
  char buf[1024];
  while ((lp = fgets(buf,1024,fp)))
    {
      lp[strlen(lp)-1] = '\0';
      char **f = tab_split((char*)pool_copy((uccp)lp,sl->p));
      t.key = f[1];
      t.ref = f[0];
      t.cnt = f[2];
      t.pct = f[3];
      struct tis_data *tp = memo_new(sl->m_idata);
      *tp = t;
      hash_add(sl->h_idata, (uccp)t.key, tp);
    }
#else
  while ((lp = (char*)loadoneline(fp, NULL)))
    {
      if ('o' == *lp)
	{
	  tis_strip_iss(lp);
	  t.key = (char *)pool_copy((uccp)lp, sl->p);
	  lp = strchr(t.key, '\t');
	  *lp++ = '\0';
	  if (!strncmp(lp, type, 3))
	    {
	      if (!strchr(t.key, '|')) /* may use meta one day */
		{
		  t.ref = lp;
		  lp = strchr(lp, '\t'); *lp++ = '\0';
		  t.cnt = lp;
		  lp = strchr(lp, '\t'); *lp++ = '\0';
		  t.pct = lp;
		  struct tis_data *tp = memo_new(sl->m_idata);
		  *tp = t;
		  hash_add(sl->h_idata, (uccp)t.key, tp);
		}
	    }
	}
    }
  loadoneline(NULL, NULL);
#endif
}

/* Read the corpus statistics from a .tis file into a hash-by-oid, sl->idata.
 */
void
sx_ldata_init(struct sl_signlist *sl, const char *ldata_file)
{
  FILE *fp = NULL;

  sl->h_ldata = hash_create(1024);
  
  if (!(fp = fopen(ldata_file, "r")))
    {
      perror(ldata_file);
      return;
    }

  char buf[1024];
  char *lp;
  while ((lp = fgets(buf,1024,fp)))
    {
      lp[strlen(lp)-1] = '\0';
      char **f = tab_split((char*)pool_copy((uccp)lp,sl->p));
      List *ll = hash_find(sl->h_ldata, (uccp)f[0]);
      if (!ll)
	{
	  ll = list_create(LIST_SINGLE);
	  hash_add(sl->h_ldata, (uccp)f[0], ll);
	}
      list_add(ll, f[1]);
    }
}

const unsigned char *
sx_idata_key(const char *soid, const char *foid, unsigned const char *v)  
{
  static unsigned char *k = NULL;
  static int k_alloced = 0;
  int klen = strlen(soid)+strlen(foid)+strlen((ccp)v) + 3;
  if (klen > k_alloced)
    {
      while (klen > k_alloced)
	k_alloced += 128;
      k = realloc(k, klen);
    }
  sprintf((char*)k, "%s.%s.%s", soid, foid, v);
  return k;
}

void
sx_idata_ctotals(struct sl_signlist *sl)
{
  if (sl->h_idata)
    {
      int i;
      for (i = 0; i < sl->nsigns; ++i)
	{
	  if (sl->signs[i]->oid)
	    {
	      size_t ns = 0, nc = 0;
	      struct tis_data *tip = hash_find(sl->h_idata, (uccp)sx_idata_key(sl->signs[i]->oid, "", (uccp)""));
	      if (tip)
		{
		  ns = atoi(tip->cnt);
		  /* Get the compound data for the current sign */
		  if (sl->signs[i]->hcompounds)
		    {
		      struct sl_compound_digest *cdp = hash_find(sl->signs[i]->hcompounds,
								 (uccp)"#digest_by_oid");
		      if (cdp)
			{
			  /* Now cdp->memb has an OID list for all the
			     compounds that contain the current sign*/
			  int j;
			  for (j = 0; cdp->memb[j]; ++j)
			    {
			      tip = hash_find(sl->h_idata, (uccp)sx_idata_key(cdp->memb[j], "", (uccp)""));
			      if (tip) /* no guarantee that a given compound will be in a corpus */
				nc += atoi(tip->cnt);
			    }
			}
		    }
#if 1
		  sl->signs[i]->count = ns;
		  sl->signs[i]->ctotal = nc;
#else
		  fprintf(stderr,
			  "sx_idata_ctotals: sign %s occurs %ld times; %ld times including compounds\n",
			  sl->signs[i]->name, ns, nc);
#endif
		}
	    }
	}
    }
}

void
sx_idata_sign(struct sl_signlist *sl, struct sl_sign *sp)
{
  if (sl->h_idata && sp->oid)
    sp->inst->tp = hash_find(sl->h_idata, (uccp)sx_idata_key(sp->oid, "", (uccp)""));
}

void
sx_idata_form_inst(struct sl_signlist *sl, struct sl_inst *fip)
{
  if (sl->h_idata && fip->u.f->oid && fip->parent_s->u.s->oid)
    {
      const char *k = (ccp)sx_idata_key(fip->parent_s->u.s->oid,
					fip->u.f->oid,
					(uccp)"");
      fip->tp = hash_find(sl->h_idata, (uccp)k);
      if (fip->tp)
	fprintf(stderr, "sx_idata_form_inst: found key %s\n", k);
    }
}

void
sx_idata_value_inst(struct sl_signlist *sl, struct sl_inst *vip)
{
  if (sl->h_idata)
    {
      struct sl_inst *sip, *fip;

      if (vip->parent_s)
	sip = vip->parent_s;
      else if (vip->parent_f)
	sip = vip->parent_f->parent_s;

      if (vip->parent_f)
	fip = vip->parent_f;
      else
	fip = NULL;
	
      if (fip)
	{
	  if (sip->u.s->oid && fip->u.f->oid)
	    vip->tp = hash_find(sl->h_idata, (uccp)sx_idata_key(sip->u.s->oid,
								fip->u.f->oid,
								(uccp)vip->u.v->name));
	}
      else
	vip->tp = hash_find(sl->h_idata, (uccp)sx_idata_key(sip->u.s->oid,
							    "",
							    (uccp)vip->u.v->name));
    }
}

void
sx_ldata_value_inst(struct sl_signlist *sl, struct sl_inst *vip)
{
  if (sl->h_ldata)
    {
      struct sl_inst *sip, *fip;

      if (vip->parent_s)
	sip = vip->parent_s;
      else if (vip->parent_f)
	sip = vip->parent_f->parent_s;

      if (vip->parent_f)
	fip = vip->parent_f;
      else
	fip = NULL;
	
      if (fip)
	{
	  if (sip->u.s->oid && fip->u.f->oid)
	    vip->lp = hash_find(sl->h_ldata, (uccp)sx_idata_key(sip->u.s->oid,
								fip->u.f->oid,
								(uccp)vip->u.v->name));	  
	}
      else
	vip->lp = hash_find(sl->h_ldata, (uccp)sx_idata_key(sip->u.s->oid,
							    "",
							    (uccp)vip->u.v->name));
    }
}
