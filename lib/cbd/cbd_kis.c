#include <oraccsys.h>
#include <roco.h>
#include "cbd.h"

/* cbd_kis.c: add Kis references to a Cbd by generating keys and
 * looking them up in the Kis.
 *
 * If found, add the Kis entry pointer--in the initial implementation
 * that is a Roco row (char**) and it is unlikely that that will ever
 * change
 *
 * If not found, save the key with pool copy, and add a char *r[2]
 * where r[1] is the key and r[0] is NULL indicating that it was not
 * found in the Kis.
 */

Efield hfields[128];
static Cbd *my_c;
static Kis *my_k;
static Hash *nmfm_e, *nmfm_s;

static Field *curr_field = NULL;

static void cbd_kis_wrapup_e(Cbd_fw_type t, Entry *v);
static void cbd_kis_wrapup_s(Cbd_fw_type t, Sense *v);

/** Kis look up is done via a pointer which is set to kis_data_no_kis
 *  if there is no tokl.kis
 */
static Kis_data (*kis_data)(const char *);

static Kis_data
kis_data_no_kis(const char *k)
{
  Kis_data kdp = memo_new_array(my_c->kisnullmem, 2);
  /* kdp[0] is marker for key-not-found */
  kdp[1] = (char*)pool_copy((uccp)k, csetp->pool);
  return kdp;
}

static Kis_data
kis_data_ye_kis(const char *k)
{
  Kis_data kdp = hash_find(my_k->hdata, (uccp)k);
  if (!kdp)
    return kis_data_no_kis(k);
  else
    return kdp;
}

/****/

/** Kis data is saved in a wrapper structure, Field, which has members
 *  for ID and user data as well as the Kis being saved. Each key
 *  only has one Field data structure; the has contains all the keys
 *  for a given type, e.g., BASE, CONT, etc., so when these are
 *  reduced to an array we have an array with type Field **
 */

const char *
cbd_field_id(Entry *ep)
{
  static char id[32], *ins = NULL;
  static int n = 0;
  if (ep)
    {
      sprintf(id, "%s.", ep->oid);
      ins = id+strlen(id);
      n = 0;
      return NULL;
    }
  else
    {
      sprintf(ins, "%d", n++);
      return (ccp)pool_copy((uccp)id, csetp->pool);
    }
}

void
cbd_field_ids(Field **ffp)
{
  int i;
  for (i = 0; ffp[i]; ++i)
    {
      ffp[i]->id = cbd_field_id(NULL);
      if (strstr(ffp[i]->k[1], "\x01="))
	((Cform*)ffp[i]->data)->f.user = (void*)ffp[i]->id;
    }
}

Field *
cbd_field(Kis_data k, void *v)
{
  Field *f = memo_new(csetp->fieldsmem);
  f->k = k;
  f->data = v; /* Usually Cform but NULL when called from kis_periods */
  curr_field = f;
  return f;
}

static void
kis_hdata(Hash *h, const char *k, Cform *f)
{
  if (!hash_find(h, (uccp)k))
    {
      fprintf(cbd_log_fp, "kis_hdata: hashing k=%s\n", k);
      hash_add(h, pool_copy((uccp)k, csetp->pool), cbd_field(kis_data(k), f));
    }
  else
    {
      fprintf(cbd_log_fp, "kis_hdata: skipping k=%s -- already known\n", k);
    }
}

/****/

static const char *
kis_data_debug(Kis_data kdp)
{
  static char buf[64];
  if (kdp[0])
    sprintf(buf, "%s %sx %s%%", kdp[0], kdp[2], kdp[3]);
  else
    sprintf(buf, "(null) 0x 0%%");
  return buf;
}


/* Remember the Cform for each FORM that occurs with a given NORM by
   making a hash with key=NORM val=NmFm */
static void
norm_nmfm_hash(Hash *nmfm_hash, Cform *c)
{
  Hash *hp = hash_find(nmfm_hash, c->f.norm);
  if (!hp)
    hash_add(nmfm_hash, c->f.norm, (hp = hash_create(10)));
  if (!hash_find(hp, c->f.form))
    {
      NmFm *np = memo_new(csetp->nmfmsmem);
      np->form = c;
      hash_add(hp, c->f.form, np);
      if (cbd_log_fp)
	fprintf(cbd_log_fp, "norm_nmfm_hash: indexing form %s for norm %s\n",
		c->f.form, c->f.norm);
    }
  else if (cbd_log_fp)
    fprintf(cbd_log_fp, "norm_nmfm_hash: form %s already known for norm %s\n",
	    c->f.form, c->f.norm);
}

/* On entry NmFm only has ->form set; add the key from $NORM=FORM */
static NmFm *
norm_nmfm_data(const char *normk, NmFm *nfp)
{
  char nmfmk[strlen(normk)+strlen((ccp)nfp->form->f.form)+2];
  sprintf(nmfmk, "%s=%s", normk, (ccp)nfp->form->f.form);
  if (cbd_log_fp)
    fprintf(cbd_log_fp, "norm_nmfm_data: normk = %s\n", nmfmk);
  nfp->nmfmk = (ccp)pool_copy((uccp)nmfmk, csetp->pool);
  return nfp;
}

/* For each NORM, look up the NmFm in nmfm_hash, augment the NmFm and
 * add it to a list; then reduce the augmented list to an array and
 * store it in the NORM field->user
 */
static void
cbd_nmfm_wrap(Hash *nmfm_hash, Field **f)
{
  int i;
  for (i = 0; f[i]; ++i)
    {
      Hash *lp = hash_find(nmfm_hash, (uccp)((Cform*)f[i]->data)->f.norm);
      if (lp && lp->key_count)
	{
	  List *nmfm_list = list_create(LIST_SINGLE);
	  const char **kk = hash_keys(lp);
	  int j;
	  for (j = 0; kk[j]; ++j)
	    {
	      NmFm *nfp = hash_find(lp, (uccp)kk[j]);
	      /*if (!nfp->nmfmk)*/
		list_add(nmfm_list, norm_nmfm_data(f[i]->k[1], nfp));
	    }
	  f[i]->user = list2array(nmfm_list);
	  if (cbd_log_fp)
	    fprintf(cbd_log_fp, "cbd_nmfm_wrap key %s has f->user = %p\n",
		    f[i]->k[1], f[i]->user);
	}
      else
	fprintf(stderr, "cbd_nmfm_wrap: internal error: %s not found or empty list in nmfm_hash\n",
		((Cform*)f[i]->data)->f.norm);
    }
  /*hash_free(nmfm_hash, NULL);*/
}

/* Handler function used by cbd_key
 *
 * In fields call v is the source Cform.
 */
void
cbd_kis_key_h(const char *k, int context, int field, void *v)
{
  if ('e' == field)
    {
      if (!((Entry*)v)->k)
	{
	  ((Entry*)v)->k = kis_data(k);
	  int i;
	  for (i = 0; i < EFLD_TOP; ++i)
	    ((Entry*)v)->hshary[i] = hash_create(128);
	}
    }
  else if ('s' == field)
    {
      if (!((Sense*)v)->k)
	((Sense*)v)->k = kis_data(k);
      int i;
      for (i = 0; i < EFLD_TOP; ++i)
	((Sense*)v)->hshary[i] = hash_create(128);
    }
  else
    {
      int hfield = hfields[field];
      if (hfield >= 0)
	{
	  if ('e' == context)
	    {
	      kis_hdata(((Cform*)v)->e->hshary[hfield], k, v);
	      if ('=' == field)
		norm_nmfm_hash(nmfm_e, v);
	    }
	  else
	    {
	      kis_hdata(((Cform*)v)->s->hshary[hfield], k, v);
	      if ('=' == field)
		norm_nmfm_hash(nmfm_s, v);
	    }
	}
      else
	fprintf(stderr, "cbd_kis_key_h: internal error: unknown field '%c'\n", field);
    }
  
  /*fprintf(stderr, "FW_%c field %c: ", context, field);*/
}

/* Handler function passed to cbd_form_walk */
void
cbd_kis_fw_h(Cform *f, Cbd_fw_type t, void *v)
{
  switch (t)
    {
    case CBD_FW_E:
      cbd_key_cgp(f, v, NULL);
      nmfm_e = hash_create(10);
      cbd_field_id(v); /* do this early because we do sense wrapup before entry wrapup */
      break;
    case CBD_FW_EF:
      cbd_key_fields(f, 'e', v);
      break;
    case CBD_FW_S:
      cbd_key_cgpse(f, v, NULL);
      nmfm_s = hash_create(10);
      break;
    case CBD_FW_SF:
      {
	/* This is a safe place to make a sig for the sense/form combo */
	Field *fp = memo_new(csetp->fieldsmem);
	fp->id = cbd_field_id(NULL);
	fp->data = form_sig(csetp->pool, &f->f);
	fp->k = kis_data((char*)fp->data);
	hash_add(((Cform*)v)->s->hsigs, (uccp)fp->data, fp);
	fprintf(cbd_log_fp, "sig %s\n", (char*)fp->data);
	cbd_key_fields(f, 's', v);
      }
      break;
    case CBD_FW_EE:
      cbd_kis_wrapup_e(t, v);
      if (((Entry*)v)->hshary[EFLD_NORM])
	cbd_nmfm_wrap(nmfm_e, ((Entry*)v)->hshary[EFLD_NORM]);
      break;
    case CBD_FW_SE:
      cbd_kis_wrapup_s(t, v);
      if (((Sense*)v)->hshary[EFLD_NORM])
	cbd_nmfm_wrap(nmfm_s, ((Sense*)v)->hshary[EFLD_NORM]);
      break;
    case CBD_FW_PE:
      cbd_key_cgp(f, v, NULL);
      break;
    case CBD_FW_PS:
      cbd_key_cgpse(f, v, NULL);
      break;
    }
}

Field **
kis_data_h2k(Hash *h, Efield e)
{
  int n;
  Field **kp = (Field**)hash_vals2(h, &n);

  fprintf(cbd_log_fp, "kis_data_h2k: Efield=%d n=%d key_count=%d\n", e, n, (int)h->key_count);
  qsort(kp, n, sizeof(Field*), kis_fld_cmp(e));
  
  return kp;
}

void
kisdata_show_one(Kis_data k)
{
  if (cbd_log_fp)
    {
      if (k[0])
	fprintf(cbd_log_fp, "%s %s\n", k[1], kis_data_debug(k));
      else
	fprintf(cbd_log_fp, "%s (null) 0x 0%%\n", k[1]);
    }
}

void
kisdata_show_ary(Field **f)
{
  int i;
  for (i = 0; f[i]; ++i)
    kisdata_show_one(f[i]->k);
}

/** Kis periods are compiled in lib/kis/kis.c immediately after the
 *  Kis is loaded; they are stored in a hash in k->user; hash keys are
 *  CGP/CGPSE and values are Field ** like other field data
 */
static Field **
cbd_kis_periods(Kis *k, unsigned const char *key)
{
  Field **kdp = hash_find(k->user, key);
  if (kdp)
    kisdata_show_ary(kdp);
  return kdp;
}

/* Reduce all the field hashes to arrays and sort them so they are
 * marshalled for further processing.
 */
static void
cbd_kis_wrapup_e(Cbd_fw_type t, Entry *ep)
{
  if (ep->k)
    {
      kisdata_show_one(ep->k);
      if (ep->k[0] && my_k)
	ep->hshary[EFLD_PERD] = cbd_kis_periods(my_k, (uccp)ep->k[1]);
      int i;
      /* EFLD_PERD must be Efield == 0 for this to work */
      for (i = 1; i < EFLD_TOP; ++i)
	{
	  if (ep->hshary[i])
	    {
	      void *a = kis_data_h2k(ep->hshary[i], i);
	      hash_free(ep->hshary[i], NULL);
	      cbd_field_ids(a);
	      ep->hshary[i] = a;
	      kisdata_show_ary(ep->hshary[i]);
	    }
	}
    }
}

static void
cbd_kis_wrapup_s(Cbd_fw_type t, Sense *sp)
{
  if (sp->k)
    {
      kisdata_show_one(sp->k);
      if (sp->k[0] && my_k)
	sp->hshary[EFLD_PERD] = cbd_kis_periods(my_k, (uccp)sp->k[1]);
      /* EFLD_PERD must be Efield == 0 for this to work */
      int i;
      for (i = 1; i < EFLD_TOP; ++i)
	{
	  if (sp->hshary[i])
	    {
	      if (i == EFLD_BASE)
		fprintf(cbd_log_fp, "cbd_kis_wrapup sense = %s key_count = %d\n",
			sp->cgspe, (int)((Hash*)sp->hshary[i])->key_count);
	      void *a = kis_data_h2k(sp->hshary[i], i);
	      hash_free(sp->hshary[i], NULL);
	      cbd_field_ids(a);
	      sp->hshary[i] = a;
	      kisdata_show_ary(sp->hshary[i]);
	    }
	}
    } 
}

void
hfields_init(void)
{
  hfields['p'] = EFLD_PERD;
  hfields['='] = EFLD_FORM;
  hfields['$'] = EFLD_NORM;
  hfields['^'] = EFLD_NMFM;
  hfields['~'] = EFLD_FMOF;
  hfields['/'] = EFLD_BASE;
  hfields['*'] = EFLD_STEM;
  hfields['+'] = EFLD_CONT;
  hfields['#'] = EFLD_MRF1;
  hfields['m'] = EFLD_MRF2;
}

void
cbd_kis(Cbd *c, Kis *k)
{
  if (!c->kisnullmem)
    {
      c->kisnullmem = memo_init(sizeof(char *), 256);
      hfields_init();
    }
  my_c = c;
  if (k)
    {
      my_k = k;
      kis_data = kis_data_ye_kis;
    }
  else
    kis_data = kis_data_no_kis;
  cbd_key_set_action(cbd_kis_key_h);
  cbd_form_walk(c, cbd_kis_fw_h);
}
