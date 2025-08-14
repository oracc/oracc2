#include <oraccsys.h>
#include "roco.h"
#include "osl_unicode.h"

static Osl_uentry entryX = { .u="U+0000" , .n=(uccp)"X"    , .o=NULL , .s=-1 };
static Osl_uentry u200C =  { .u="U+200C" , .n=(uccp)"ZWNJ" , .o=NULL , .s=-1 };
static Osl_uentry u200D =  { .u="U+200D" , .n=(uccp)"ZWJ"  , .o=NULL , .s=-1 };

static Osl_unicode *
osl_unicode_tsv(const char *utsv)
{
  Osl_unicode *o = calloc(1, sizeof(Osl_unicode));
  o->r = roco_load(utsv, 0, NULL, NULL, NULL, NULL);
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
	  if (o->r->rows[i][3])
	    o->e[i].s = atoi((ccp)o->r->rows[i][3]);
	  else
	    o->e[i].s = -1;
	  hash_add(o->h, (uccp)o->e[i].u, &o->e[i]);
	  hash_add(o->h, o->e[i].n, &o->e[i]);
	  if (o->e[i].o)
	    hash_add(o->h, (uccp)o->e[i].o, &o->e[i]);
	  if (!o->e[i].seq)
	    hash_add(o->h, (uccp)o->e[i].u+2, &o->e[i]);
	}
    }
  hash_add(o->h, (uccp)"200C", &u200C);
  hash_add(o->h, (uccp)"200D", &u200D);
  return o;
}

Osl_unicode *
osl_unicode(const char *project)
{
  char buf[strlen(oracc())+strlen("/02pub/0")+strlen(project)];
  sprintf(buf, "sl/unicode.tsv");
  if (access(buf, R_OK))
    sprintf(buf, "%s/%s/02pub/unicode.tsv", oracc(), project);
  return osl_unicode_tsv(buf);
}

Osl_uentry *
osl_autocreate(const char *u)
{
  Osl_uentry *up = calloc(1, sizeof(Osl_uentry));
  const char *h = u;
  if (('0' == h[0] && 'x' == h[1])
      || ('_' == h[0] && 'u' == h[1])
      || ('U' == h[0] && '+' == h[1]))
    h += 2;
  char name[3+strlen(h)];
  sprintf(name, "U+%s", h);
#if 0
  char *hu = name;
  while (*hu)
    {
      if (islower(*hu))
	  *hu = toupper(*hu);
      ++hu;
    }
#endif
  up->n = (uccp)strdup(name);
  up->o = "";
  up->s = strtoul(h, NULL, 16);
  return up;
}

/* Take a sequence in OSL (x12345.x12345) or font format
 * (u12345_u12345) and return a NULL-terminated array of Osl_unicode structs
 */
Osl_uentry **
osl_sequence(Osl_unicode *op, const char *seq, int *nseq)
{
  if (!op || !seq)
    return NULL;
  
  char sep = (*seq == 'u' ? '_' : '.');
  const char *next = seq;
  int n = 1;
  while ((next = strchr(next, sep)))
    {
      ++n;
      ++next;
    }
  Osl_uentry **oo = calloc(n, sizeof(Osl_uentry*));
  int i;
  for (i = 0, next = seq+1; i < n; ++i)
    {
      char buf[6];
      char *nxtsep = strchr(next, sep);
      if (nxtsep)
	{
	  strncpy(buf,next,nxtsep-next);
	  buf[nxtsep-next] = '\0';
	}
      else
	strcpy(buf, next);
      /*fprintf(stderr, "osl_sequence looking for '%s'\n", buf);*/
      Osl_uentry *e = hash_find(op->h, (uccp)buf);
      if (e)
	oo[i] = e;
      else
	oo[i] = &entryX;
      if (nxtsep)
	{
	  if (!strncmp(nxtsep, "_uni", 4))
	    next = nxtsep + 4;
	  else
	    next = nxtsep + 2;
	}
    }
  if (nseq)
    *nseq = i;
  return oo;
}

unsigned char *
osl_seq_name(Osl_unicode *op, const char *seq, Pool *p)
{
  Osl_uentry**oup;
  int noup;
  oup = osl_sequence(op, seq, &noup);
  unsigned char *ret = osl_ou_seq_name(oup, noup, p);
  free(oup);
  return ret;
}

unsigned char *
osl_ou_seq_name(Osl_uentry **oup, int noup, Pool *p)
{
  int len = 0;
  int i;
  for (i = 0; i < noup; ++i)
    len += strlen((ccp)oup[i]->n);
  len += noup + 1;
  char buf[len]; *buf = '\0';
  for (i = 0; i < noup; ++i)
    {
      if (i)
	strcat(buf,"-");
      strcat(buf, (ccp)oup[i]->n);
    }
  if (p)
    return pool_copy((uccp)buf, p);
  else
    return (ucp)strdup(buf);      
}

#if 0
int
main(int argc, char **argv)
{
  osl_unicode();
}
#endif
