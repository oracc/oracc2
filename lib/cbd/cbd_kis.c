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

static Kis_data
kis_data(const char *k)
{
  Kis_data kdp = hash_find(my_k->hdata, (uccp)k);
  if (!kdp)
    {
      kdp = memo_new_array(my_c->kisnullmem, 2);
      /* kdp[0] is marker for key-not-found */
      kdp[1] = (char*)pool_copy((uccp)k, csetp->pool);
    }
  return kdp;
}

static void
kis_hdata(Hash *h, const char *k)
{
  if (!hash_find(h, (uccp)k))
    hash_add(h, pool_copy((uccp)k, csetp->pool), kis_data(k));
}

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
	  ((Entry*)v)->u.hfields = calloc(1, sizeof(Hfields));
	  int i;
	  for (i = 0; i < EFLD_TOP; ++i)
	    *(((Entry*)v)->u.hfields)[i] = hash_create(128);
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
	    kis_hdata(*(((Entry*)v)->u.hfields)[hfield], k);
#else
	  kis_hdata(context=='e'
		    ?((Entry*)v)->u.hfields[hfield]
		    :((Sense*)v)->u.hfields[hfield],
		    k);
#endif
	}
      else
	fprintf(stderr, "cbd_kis_key_h: internal error: unknown field '%c'\n", field);
    }
  
  /*fprintf(stderr, "FW_%c field %c: ", context, field);*/
}

/* Handler function passed to cbd_form_walk */
void
cbd_kis_fw_h(Form *f, Cbd_fw_type t, void *v)
{
  switch (t)
    {
    case CBD_FW_E:
      cbd_key_cgp(f, v, NULL);
      break;
    case CBD_FW_EF:
      cbd_key_fields(f, 'e', f->entry);
      break;
    case CBD_FW_S:
      cbd_key_cgpse(f, v, NULL);
      break;
    case CBD_FW_SF:
      cbd_key_fields(f, 's', f->sense_p);
      break;
    case CBD_FW_EE:
      cbd_kis_wrapup(t, v);
      break;
    case CBD_FW_SE:
      break;
    case CBD_FW_PE:
      cbd_key_cgp(f, v, NULL);
      cbd_key_fields(f, 'e', f->entry);
      break;
    case CBD_FW_PS:
      cbd_key_cgpse(f, v, NULL);
      cbd_key_fields(f, 's', f->sense_p);
      break;
    }
}

Kis_data *
kis_data_h2k(Hash *h, Efield e)
{
  int n;
  Kis_data *kp = (Kis_data*)hash_vals2(h, &n);

  /*kis_reset_key(kp, n);*/

  qsort(kp, n, sizeof(Kis_data), kis_cmp(e));
  
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
kisdata_show(Kis_data* k, FILE *fp)
{
  int i;
  for (i = 0; k[i]; ++i)
    kisdata_show_one(k[i], fp);
}

void
cbd_kis_periods(Kis *k, unsigned const char *key)
{
  /*fprintf(stderr, "cbd_kis_periods: key = ::%s::\n", key);*/
  Kis_data *kdp = hash_find((Hash*)k->user, key);
  if (kdp)
    kisdata_show(kdp, stderr);
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
      if (ep->k[1])
	cbd_kis_periods(my_k, (uccp)ep->k[1]);
    }

  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    if (sp->k)
      {
	kisdata_show_one(sp->k, stderr);
	if (sp->k[1])
	  cbd_kis_periods(my_k, (uccp)sp->k[1]);
      }

  int i;
  for (i = 0; i < EFLD_TOP; ++i)
    {
      if (*(ep->u.hfields)[i])
	{
	  (ep->u.kisdata)[i] = kis_data_h2k(*(ep->u.hfields)[i], i);
	  kisdata_show((ep->u.kisdata)[i], stderr);
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
  my_k = k;
  cbd_key_set_action(cbd_kis_key_h);
  cbd_form_walk(c, cbd_kis_fw_h);
}
