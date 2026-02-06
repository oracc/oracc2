#include <oraccsys.h>
#include "cbd.h"

/* cbd_key.c: generate keys from Form structures */

static char *buf = NULL;
static int buf_alloced = 0;

static cbdactionfunc cbdact;
void
cbd_key_set_action(cbdactionfunc f)
{
  cbdact = f;
}

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
cbd_key_form_len(Form *f)
{
  int l = cbd_key_form_len_one(f);
  if (f->parts)
    {
      if (!f->cof_id)
	l += 4; /* PSU {}:: */
      int i;
      for (i = 0; f->parts[i]; ++i)
	l += cbd_key_form_len_one(f->parts[i]) + 2; /* add 2 for each '&&' or '++' */
    }
  return l;
}

/* This routine assumes it has been caller is cbd_key_cgp or
   cbd_key_cgpse */
static void
cbd_key_period(Form *f, int context, void *v, const char *period)
{
  char *insert = buf + strlen(buf);
  sprintf(insert, "p%s", period);
  cbdact(buf, context, 'p', v);
  *insert = '\0';
}

/* This routine assumes it has been caller is cbd_key_cgp or
   cbd_key_cgpse */
void
cbd_key_fields(Form *f, int context, void *v)
{
  char *insert = buf + strlen(buf);
#define fpr(c,x) if(f->x){sprintf(insert,"%c%s",c,f->x); cbdact(buf,context,c,v); } /*(t,vp,qid)*/
  if (f->oform)
    {
      sprintf(insert, "=%s~~%s", f->form, f->oform);
      cbdact(buf, context, '~', v);
    }
  else
    fpr('=',form);
  fpr('$',norm);
  if (f->form && f->norm)
    {
      sprintf(insert, "$%s=%s", f->norm, f->form);
      cbdact(buf, context, '^', v);
    }
  fpr('#',morph);
  fpr('/',base);
  fpr('+',cont);
  fpr('*',stem);
  if (f->morph2)
    {
      sprintf(insert, "##%s", f->morph2);
      cbdact(buf,context,'m',v);
    }
#undef fpr
  *insert = '\0';
}

void
cbd_key_cgp(Form *f, Entry *e, const char *period)
{
  int len = cbd_key_form_len(f) + 3; /* ^A^A\0 */
  if (buf_alloced < len)
    {
      buf_alloced = len;
      buf = realloc(buf, buf_alloced);
    }
  sprintf(buf, "%%%s:%s[%s]%s%c%c", f->lang, f->cf, f->gw, f->pos, 1, 1);
  cbdact(buf, 'e', 'e', e);
  if (period)
    cbd_key_period(f, 'e', e, period);
}

/* This routine assumes it has been called immediately after
   cbd_key_cgp */
void
cbd_key_cgpse(Form *f, Sense *s, const char *period)
{
  sprintf(buf, "%%%s:%s[%s]%s%c//%s'%s%c", f->lang, f->cf, f->gw, f->pos, 1, f->sense, f->epos, 1);
  cbdact(buf, 's', 's', s);
  if (period)
    cbd_key_period(f, 's', s, period);
}
