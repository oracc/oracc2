#include <oraccsys.h>
#include "roco.h"
#include "osl_unicode.h"

Osl_unicode *
osl_unicode(void)
{
  Osl_unicode *o = calloc(1, sizeof(Osl_unicode));
  char buf[strlen(oracc())+strlen("/osl/02pub/0")];
  sprintf(buf, "%s/osl/02pub/unicode.tsv", oracc());
  o->r = roco_load(buf, 0, NULL, NULL, NULL);
  if (o->r)
    {
      o->e = calloc(o->r->nlines, sizeof(Osl_uentry));
      o->h = hash_create(o->r->nlines*2);
      int i;
      for (i = 0; i < o->r->nlines; ++i)
	{
	  o->e[i].u = (ccp)o->r->rows[i][0];
	  o->e[i].n = o->r->rows[i][1];
	  o->e[i].o = (ccp)o->r->rows[i][2];
	  o->e[i].s = atoi((ccp)o->r->rows[i][3]);
	  hash_add(o->h, (uccp)o->e[i].u, &o->e[i]);
	  hash_add(o->h, o->e[i].n, &o->e[i]);
	  hash_add(o->h, (uccp)o->e[i].o, &o->e[i]);
	}
    }
  return o;
}

#if 0
int
main(int argc, char **argv)
{
  osl_unicode();
}
#endif
