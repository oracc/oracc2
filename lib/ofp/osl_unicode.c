#include <oraccsys.h>
#include "roco.h"
#include "osl_unicode.h"

static Osl_unicode *ucode;
static Osl_uentry entryX = { .u="U+0000" , .n=(uccp)"X"    , .o=NULL , .s=-1 };
static Osl_uentry u200C =  { .u="U+200C" , .n=(uccp)"ZWNJ" , .o=NULL , .s=-1 };
static Osl_uentry u200D =  { .u="U+200D" , .n=(uccp)"ZWJ"  , .o=NULL , .s=-1 };

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
	  if ('x' == o->e[i].u[0])
	    o->e[i].seq = 1;
	  o->e[i].n = o->r->rows[i][1];
	  o->e[i].o = (ccp)o->r->rows[i][2];
	  o->e[i].s = atoi((ccp)o->r->rows[i][3]);
	  hash_add(o->h, (uccp)o->e[i].u, &o->e[i]);
	  hash_add(o->h, o->e[i].n, &o->e[i]);
	  hash_add(o->h, (uccp)o->e[i].o, &o->e[i]);
	  if (!o->e[i].seq)
	    hash_add(o->h, (uccp)o->e[i].u+2, &o->e[i]);
	}
    }
  hash_add(o->h, (uccp)"200C", &u200C);
  hash_add(o->h, (uccp)"200D", &u200D);
  return o;
}

/* Take a sequence in OSL (x12345.x12345) or font format
 * (u12345_u12345) and return a NULL-terminated array of Osl_unicode structs
 */
Osl_uentry **
osl_sequence(const char *seq, int *nseq)
{
  if (!ucode || !seq)
    return NULL;
  
  char sep = (*seq == 'u' ? '_' : '.');
  const char *next = NULL;
  int n = 1;
  while ((next = strchr(seq, sep)))
    {
      ++n;
      ++next;
    }
  Osl_uentry **oo = calloc(n, sizeof(Osl_unicode*));
  int i;
  for (i = 0, next = seq; i < n; ++i)
    {
      char buf[6];
      strncpy(buf,seq+1,5);
      buf[5] = '\0';
      Osl_uentry *e = hash_find(ucode->h, (uccp)buf);
      if (e)
	oo[i] = e;
      else
	oo[i] = &entryX;
      next = strchr(next+1, sep);
    }
  if (nseq)
    *nseq = i;
  return oo;
}

#if 0
int
main(int argc, char **argv)
{
  osl_unicode();
}
#endif
