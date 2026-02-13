#include <oraccsys.h>
#include "cbd.h"

/* cbd_key.c: generate keys from Cform structures */

static char *buf = NULL;
static int buf_alloced = 0;

static cbdactionfunc cbdact;
void
cbd_key_set_action(cbdactionfunc f)
{
  cbdact = f;
}

/* This function must take Form* not Cform* so it works with
   form->parts */
#define flen(x) (fp->x?strlen((ccp)fp->x):0)
static int
cbd_key_form_len_one(Form *fp)
{
  return flen(cf)+flen(gw)+flen(sense)+flen(pos)+flen(epos)
    +flen(form)+flen(norm)+flen(lang)+flen(base)+flen(cont)
    +flen(morph)+flen(morph2)+flen(stem)
    + strlen("[]' $ % / + # ## *0");
}
#undef flen

static int
cbd_key_form_len(Cform *f)
{
  int l = cbd_key_form_len_one(&f->f);
  if (f->f.parts)
    {
      if (!f->f.cof_id)
	l += 4; /* PSU {}:: */
      int i;
      for (i = 0; f->f.parts[i]; ++i)
	l += cbd_key_form_len_one(f->f.parts[i]) + 2; /* add 2 for each '&&' or '++' */
    }
  return l;
}

/* This routine assumes it has been caller is cbd_key_cgp or
   cbd_key_cgpse */
static void
cbd_key_period(Cform *f, int context, void *v, const char *period)
{
  char *insert = buf + strlen(buf);
  sprintf(insert, "p%s", period);
  cbdact(buf, context, 'p', v);
  *insert = '\0';
}

/***N.B. In cbd_key_cgp, cbd_key_cgspe, and cbd_key_fields, arguments
 ** other than Cform *f must not be used in the body of these
 ** functions. They can safely be passed to the cbdact call.
 **
 ** For CBD, the additional arguments are meaningful.
 **
 ** For tokexl, the additional arguments are ignored and the
 ** Entry/Sense/void * passed to the three functions below are all
 ** NULL.
 ***/

/* This routine assumes it has been called after cbd_key_cgp or
 * cbd_key_cgpse
 *
 * When called from cbd_fw_fields v is the source Cform and f is the temporary Cform.
 */
void
cbd_key_fields(Cform *f, int context, void *v)
{
  char *insert = buf + strlen(buf);
#define fpr(c,x) if(f->f.x){sprintf(insert,"%c%s",c,f->f.x); cbdact(buf,context,c,v);}
  if (f->f.oform)
    {
      sprintf(insert, "=%s~~%s", f->f.form, f->f.oform);
      cbdact(buf, context, '~', v);
    }
  else
    fpr('=',form);
  fpr('$',norm);
  if (f->f.form && f->f.norm)
    {
      sprintf(insert, "$%s=%s", f->f.norm, f->f.form);
      cbdact(buf, context, '^', v);
    }
  fpr('#',morph);
  fpr('/',base);
  fpr('+',cont);
  fpr('*',stem);
  if (f->f.oform)
    {
      sprintf(insert, "=%s~~%s", f->f.form, f->f.oform);
      cbdact(buf, context, '~', v);
    }
  else
    fpr('=',form);
  fpr('$',norm);
  if (f->f.form && f->f.norm)
    {
      sprintf(insert, "$%s=%s", f->f.norm, f->f.form);
      cbdact(buf, context, '^', v);
    }
  fpr('#',morph);
  fpr('/',base);
  fpr('+',cont);
  fpr('*',stem);
  if (f->f.oform)
    {
      sprintf(insert, "=%s~~%s", f->f.form, f->f.oform);
      cbdact(buf, context, '~', v);
    }
  else
    fpr('=',form);
  fpr('$',norm);
  if (f->f.form && f->f.norm)
    {
      sprintf(insert, "$%s=%s", f->f.norm, f->f.form);
      cbdact(buf, context, '^', v);
    }
  fpr('#',morph);
  fpr('/',base);
  fpr('+',cont);
  fpr('*',stem);
  if (f->f.morph2)
    {
      sprintf(insert, "m%s", f->f.morph2);
      cbdact(buf,context,'m',v);
    }
#undef fpr
  *insert = '\0';
}

void
cbd_key_cgp(Cform *f, Entry *e, const char *period)
{
  int len = cbd_key_form_len(f) + 3; /* ^A^A\0 */
  if (buf_alloced < len)
    {
      buf_alloced = len;
      buf = realloc(buf, buf_alloced);
    }
  sprintf(buf, "%%%s:%s[%s]%s%c%c", f->f.lang, f->f.cf, f->f.gw, f->f.pos, 1, 1);
  cbdact(buf, 'e', 'e', e);
  if (period)
    cbd_key_period(f, 'e', e, period);
}

void
cbd_key_cgpse(Cform *f, Sense *s, const char *period)
{
  sprintf(buf,
	  "%%%s:%s[%s]%s%c//%s'%s%c",
	  f->f.lang, f->f.cf, f->f.gw, f->f.pos, 1, f->f.sense, f->f.epos, 1);
  cbdact(buf, 's', 's', s);
  if (period)
    cbd_key_period(f, 's', s, period);
}
