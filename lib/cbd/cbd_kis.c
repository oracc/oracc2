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

static void cbd_kis_wrapup(Cbd_fw_type t, Entry *v);

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
  static char id[32], *ins;
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
    ffp[i]->id = cbd_field_id(NULL);
}

Field *
cbd_field(Kis_data k, void *v)
{
  Field *f = memo_new(csetp->fieldsmem);
  f->k = k;
  f->data = v; /* Usually Cform but may be entry if field is periods */
  return f;
}

static void
kis_hdata(Hash *h, const char *k, Cform *f)
{
  if (!hash_find(h, (uccp)k))
    hash_add(h, pool_copy((uccp)k, csetp->pool), cbd_field(kis_data(k), f));
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

/* Handler function used by cbd_key */
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
    }
  else
    {
      int hfield = hfields[field];
      if (hfield >= 0)
	{
#if 1
	  /* We don't collect sense-specific field data yet */
	  if ('e' == context)
	    kis_hdata(((Cform*)v)->e->hshary[hfield], k, v);
#else
	  kis_hdata(context=='e'
		    ?((Cform*)v)->e->hshary[hfield]
	    	    :((Cform*)v)->s->hshary[hfield],
		    k, v);
#endif
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
      break;
    case CBD_FW_EF:
      cbd_key_fields(f, 'e', f->e);
      break;
    case CBD_FW_S:
      cbd_key_cgpse(f, v, NULL);
      break;
    case CBD_FW_SF:
      cbd_key_fields(f, 's', f->s);
      break;
    case CBD_FW_EE:
      cbd_kis_wrapup(t, v);
      break;
    case CBD_FW_SE:
      break;
    case CBD_FW_PE:
      cbd_key_cgp(f, v, NULL);
      cbd_key_fields(f, 'e', f->e);
      break;
    case CBD_FW_PS:
      cbd_key_cgpse(f, v, NULL);
      cbd_key_fields(f, 's', f->s);
      break;
    }
}

Field **
kis_data_h2k(Hash *h, Efield e)
{
  int n;
  Field **kp = (Field**)hash_vals2(h, &n);

  qsort(kp, n, sizeof(Field*), kis_fld_cmp(e));
  
  return kp;
}

void
kisdata_show_one(Kis_data k, FILE *fp)
{
  if (k[0])
    fprintf(stderr, "%s %s\n", k[1], kis_data_debug(k));
  else
    fprintf(stderr, "%s (null) 0x 0%%\n", k[1]);
}

void
kisdata_show_ary(Field **f, FILE *fp)
{
  int i;
  for (i = 0; f[i]; ++i)
    kisdata_show_one(f[i]->k, fp);
}

/** Kis periods are compiled in lib/kis/kis.c immediately after the
 *  Kis is loaded; they are stored in a hash in k->user; hash keys are
 *  CGP/CGPSE and values are Field ** like other field data
 */
static Field **
cbd_kis_periods(Kis *k, unsigned const char *key)
{
  /*fprintf(stderr, "cbd_kis_periods: key = ::%s::\n", key);*/
  Field **kdp = hash_find(k->user, key);
  if (kdp)
    kisdata_show_ary(kdp, stderr);
  return kdp;
}

/* Reduce all the field hashes to arrays and sort them so they are
 * marshalled for further processing.
 */
static void
cbd_kis_wrapup(Cbd_fw_type t, Entry *ep)
{
  if (ep->k)
    {
      kisdata_show_one(ep->k, stderr);
      if (ep->k[0] && my_k)
	ep->hshary[EFLD_PERD] = cbd_kis_periods(my_k, (uccp)ep->k[1]);
      cbd_field_id(ep);
    }

  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      if (sp->k)
	{
	  kisdata_show_one(sp->k, stderr);
	  if (sp->k[0] && my_k)
	    sp->hshary[EFLD_PERD] = cbd_kis_periods(my_k, (uccp)sp->k[1]);
	}
    }
  
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
	  kisdata_show_ary(ep->hshary[i], stderr);
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
