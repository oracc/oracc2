#include <oraccsys.h>
#include <asl.h>
#include <oid.h>
#include <tok.h>
#include <tis.h>
#include "sx.h"

/* Read the corpus statistics from a .tis file into a hash-by-key, sl->h_idata.
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
  sl->h_mdata = hash_create(100);
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
      if (strstr(t.key, "++"))
	{
	  int len = strlen(t.key) - 2;
	  char buf[len+1];
	  strncpy(buf, t.key, len);
	  buf[len] = '\0';
	  hash_add(sl->h_mdata, pool_copy(buf, sl->p), tp);
	}
      else
	{
	  hash_add(sl->h_idata, (uccp)t.key, tp);
	}
    }
}

/* Read the signlist keys from a .key file into a hash-by-key, sl->h_idata.
 *
 * Subsetting via keys uses most of the same data structures as
 * subsetting by idata/adding idata, but the statistics are all NULL.
 */
void
sx_kdata_init(struct sl_signlist *sl, const char *kdata_file, const char *idata_type)
{
  char *lp;
  FILE *fp = NULL;
  sl->h_kdata = hash_create(1024);
  sl->m_idata = memo_init(sizeof(struct tis_data), 256);
  sl->kdata_cpds = list_create(LIST_SINGLE);
  
  if (!(fp = fopen(kdata_file, "r")))
    {
      perror(kdata_file);
      return;
    }

  char buf[1024];
  while ((lp = fgets(buf,1024,fp)))
    {
      int len = strlen(lp);
      lp[--len] = '\0';
      struct tis_data *tp = memo_new(sl->m_idata);
      tp->key = (char*)pool_copy((ucp)lp, sl->p);
      hash_add(sl->h_kdata, (uccp)tp->key, tp);
      /* tokex takes care of creating parent keys but if a key is
	 S.F.V it only creates the S.F. key because the form is
	 attested not the sign; we need to add the S.. key to make
	 sure the S.F. is output later */
      if ('.' == lp[len-1] && '.' != lp[len-2]) /* we have o0.o0. not o0.. */
	{
	  /* this trashes lp but we are done with it */
	  char *dot = strchr(lp, '.');
	  dot[1] = '.';
	  dot[2] = '\0';
	  if (!hash_find(sl->h_kdata, (uccp)lp))
	    {
	      tp = memo_new(sl->m_idata);
	      tp->key = (char*)pool_copy((ucp)lp, sl->p);
	      hash_add(sl->h_kdata, (uccp)tp->key, tp);
	    }
	}
    }
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

  Hash *lsort = oid_load_sort("sux");
  
  char buf[1024];
  char *lp;
  struct cbdex_header *chp = cbdex_init();
  while ((lp = fgets(buf,1024,fp)))
    {
      lp[strlen(lp)-1] = '\0';
      struct cbdex *cp = cbdex_new(chp);
      cbdex_parse((char*)pool_copy((uccp)lp,chp->p), cp);
      char sortbuf[10];
      sprintf(sortbuf, "%d", (int)(uintptr_t)hash_find(lsort, (uccp)cp->oid));
      cp->sort = (ccp)pool_copy((ucp)sortbuf, sl->p);
      List *ll = hash_find(sl->h_ldata, (uccp)cp->tok);
      if (!ll)
	{
	  ll = list_create(LIST_SINGLE);
	  hash_add(sl->h_ldata, (uccp)cp->tok, ll);
	}
      list_add(ll, cp);
    }
  /*cbdex_term(chp);*/ /* need to do this after cbd info is printed */
}

void
sx_kdata_componly(struct sl_signlist *sl, const uchar *sname)
{
  struct sl_sign *sp = hash_find(sl->hsentry, sname);
  struct sl_inst *ip = NULL;
  if (!sp && (ip = hash_find(sl->haka, sname)))
    {
      if (ip->type == 's')
	sp = ip->u.s;
      else
	sp = ip->u.f->sign;
    }
  if (!sp)
    {
      if ('|' == sname[0] && '(' == sname[1])
	{
	  int len = strlen((ccp)sname);
	  char buf[len];
	  *buf = '|';
	  strncpy((char*)&buf[1], (ccp)&sname[2], len-4);
	  buf[len-3] = '|';
	  buf[len-2] = '\0';
	  sp = hash_find(sl->hsentry, (uccp)buf);
	}
      if (!sp)
	{
	  fprintf(stderr, "sx_kdata_componly: no sl_sign* for %s\n", sname);
	  return;
	}
    }

  const char *oid = sp->inst->atoid;
  if (!oid)
    oid = hash_find(oids, sp->name);
  if (oid)
    {
      char buf[strlen((ccp)oid)+3];
      sprintf(buf, "%s..", oid);
#if 0
      fprintf(stderr, "sx_kdata_componly: checking sign %s with key %s\n", sp->name, buf);
#endif
      if (!hash_find(sl->h_kdata, (uccp)buf))
	{
#if 0
	  if (strstr(buf, "o0037926"))
	    fprintf(stderr, "found SUR&SUR\n");
#endif
	  struct tis_data *tip = memo_new(sl->m_idata);
	  tip->key = (char*)pool_copy((uccp)buf, sl->p);
	  tip->cnt = SX_IDATA_COMPONLY;
	  hash_add(sl->h_kdata, (uccp)tip->key, tip);
	}
    }
  else
    fprintf(stderr, "sx_kdata_componly: no OID for sign %s\n", sp->name);
}

static void
useq_check(struct sl_signlist *sl, const char *useq)
{
  char u[strlen(useq)+2];
  *u = 'U';
  strcpy(&u[1], useq);
  char *uu = u;
  while (*uu++) /* first increment is past leading 'U' */
    {
      if ('x' == *uu)
	*uu = '+';
    }
  uu = u;
  while (1)
    {
      char *dot = strchr(uu, '.');
      int more = 0;
      if (dot)
	{
	  more = 1;
	  *dot = '\0';
	}
      const char *oid = hash_find(sl->oid2ucode, (uccp)uu);
      if (oid)
	{
	  char buf[12];
	  sprintf(buf, "%s..", oid);
	  if (!hash_find(sl->h_kdata, (uccp)buf))
	    {
	      struct tis_data *tip = memo_new(sl->m_idata);
	      tip->key = (char*)pool_copy((uccp)buf, sl->p);
	      hash_add(sl->h_kdata, (uccp)tip->key, tip);
	      fprintf(stderr, "useq_check adding %s as %s\n", (uccp)uu, tip->key);
	    }
	}
      else if (strcmp(uu, "UX"))
	fprintf(stderr, "useq_check no OID for %s component of %s\n", uu, useq);

      if (more)
	{
	  *dot = 'U';
	  uu = dot;
	}
      else
	break;
    }
}

void
sx_kdata_useq(struct sl_signlist *sl)
{
  const uchar *k;
  for (k = list_first(sl->kdata_cpds); k; k = list_next(sl->kdata_cpds))
    {
      struct sl_sign *sp = hash_find(sl->hsentry, k);
      if (sp->U.useq)
	useq_check(sl, (ccp)sp->U.useq);
    }
}

const unsigned char *
sx_idata_key(const char *soid, const char *foid, unsigned const char *v)  
{
  if (soid)
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
  else
    return NULL;
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
	      if (tip && tip->cnt && tip->cnt != SX_IDATA_COMPONLY)
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
			      const unsigned char *k = (uccp)sx_idata_key(cdp->memb[j], "", (uccp)"");
			      if (k)
				{
				  tip = hash_find(sl->h_idata, k);
				  if (tip) /* no guarantee that a given compound will be in a corpus */
				    nc += atoi(tip->cnt);
				}
			    }
			}
		    }
#if 1
		  if (sl->signs[i]->msign)
		    {
		      sl->signs[i]->msign->mcount += ns;
		      sl->signs[i]->msign->mctotal += nc;
		    }
		  else
		    {
		      sl->signs[i]->count = ns;
		      sl->signs[i]->ctotal = nc;
		      if (sl->signs[i]->merge)
			{
			  sl->signs[i]->mcount += ns;
			  sl->signs[i]->mctotal += nc;
			}
		    }
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
    {
      sp->inst->tp = hash_find(sl->h_idata, (uccp)sx_idata_key(sp->oid, "", (uccp)""));
      if (sl->h_mdata)
	sp->inst->mtp = hash_find(sl->h_mdata, sp->oid);
    }	
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
#if 0
      if (fip->tp)
	fprintf(stderr, "sx_idata_form_inst: found key %s\n", k);
#endif
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
sx_ldata_sign_inst(struct sl_signlist *sl, struct sl_inst *sip)
{
  sip->lp = hash_find(sl->h_ldata, (uccp)sx_idata_key(sip->u.s->oid,
						      "",
						      (uccp)""));
}

void
sx_ldata_form_inst(struct sl_signlist *sl, struct sl_inst *fip)
{
  fip->lp = hash_find(sl->h_ldata, (uccp)sx_idata_key(fip->parent_s->u.s->oid,
						      fip->u.f->oid,
						      (uccp)""));
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
