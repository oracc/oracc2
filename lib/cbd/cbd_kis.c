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
 * where r[0] is the key and r[1] is NULL indicating that it was not
 * found in the Kis.
 */

static Cbd *my_c;
static Kis *my_k;

static void cbd_kis_wrapup(Cbd_fw_type t, void *v);

static Kis_data
kis_data(const char *k)
{
  Kis_data kdp = hash_find(my_k->hdata, (uccp)k);
  if (!kdp)
    {
      kdp = memo_new(my_c->kisnullmem);
      kdp[0] = (char*)pool_copy((uccp)k, csetp->pool);
    }
  return kdp;
}

static const char *
kis_data_debug(Kis_data kdp)
{
  static char buf[64];
  if (kdp[1])
    sprintf(buf, "%s %sx %s%%", kdp[0], kdp[2], kdp[3]);
  else
    sprintf(buf, "(null) 0x 0%%");
  return buf;
}

/* Handler function used by cbd_key */
void
cbd_kis_key_h(const char *k, int context, int field, void *v)
{
  Kis_data kdp = NULL;
  
  switch (field)
    {
    case 'e':
      if (!((Entry*)v)->k)
	kdp = ((Entry*)v)->k = kis_data(k);
      break;
    case 's':
      if (!((Sense*)v)->k)
	kdp = ((Sense*)v)->k = kis_data(k);
      break;
    case 'p':
      break;
    case '=':
      break;
    case '$':
      break;
    case '/':
      break;
    case '+':
      break;
    case '#':
      break;
    case 'm':
      break;
    case '*':
      break;
    default:
      break;
    }
  
  /*fprintf(stderr, "FW_%c field %c: ", context, field);*/
  if (kdp)
    fprintf(stderr, "%s %s\n", k, kis_data_debug(kdp));
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

/* Reduce all the field hashes to arrays and sort them so they are
 * marshalled for further processing.
 */
static void
cbd_kis_wrapup(Cbd_fw_type t, void *v)
{
}

void
cbd_kis(Cbd *c, Kis *k)
{
  if (!c->kisnullmem)
    c->kisnullmem = memo_init(sizeof(Kis_null), 128);
  my_c = c;
  my_k = k;
  cbd_key_set_action(cbd_kis_key_h);
  cbd_form_walk(c, cbd_kis_fw_h);
}
