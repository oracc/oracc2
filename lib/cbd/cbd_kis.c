#include <oraccsys.h>
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

static Kis *my_k;

/* Handler function used by cbd_key */
void
cbd_kis_key_h(const char *k, int context, int field, void *v)
{
  switch (field)
    {
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
    case 'p':
      break;
    default:
      break;
    }
      
  fprintf(stderr, "%s\n", k);
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
      cbd_key_fields(f, 'e', f->entry, NULL);
      break;
    case CBD_FW_S:
      cbd_key_cgpse(f, v, NULL);
      break;
    case CBD_FW_SF:
      cbd_key_fields(f, 's', f->sense_p, NULL);
      break;
    case CBD_EE:
      cbd_kis_wrapup(t, v);
      break;
    case CBD_SE:
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

  cbd_form_walk(c, cbd_kis_fw_h);
}
