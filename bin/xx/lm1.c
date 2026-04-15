#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <roco.h>
#include <pool.h>
#include <hash.h>
#include <xml.h>
#include <runexpat.h>
#include <unidef.h>

extern int options(int, char**,const char*);
extern int optind;
int verbose = 0;

int nlines;
int nmorph;
int xcl_mode;
FILE *outf;
Hash *idmap;
Hash *stems;
Pool *p;

#if 0
static const char *
base_norm(const char *b)
{
  size_t l;
  wchar_t *w = utf2wcs(b), &l;
  wchar_t *n = malloc((l+1)*sizeof(wchar_t));
  int i, dest;
  for (dest = i = 0; i < l; ++i)
    {
      if (w[i] >= U_s0 && w[i] <= U_s9)
	; /* drop sub nums */
      else if (w[i] == U_s_x)
	{
	  ++i;
	  /* drop qualifier */
	  if ('('L == w[i])
	    {
	      while (i < l && ')'L != w[i])
		++i;
	    }
	}
      else
	n[dest++] = w[i];
    }
  return (const char *)wcs2utf(n, l);
}
#endif

static void
load_stems(void)
{
  Roco *r = roco_load1("/home/stinney/orc/tra/stems/data.tsv");
  stems = roco_hash_r(r);
}

static char *
normify(const char *c, const char *b, const char *m, const char *e)
{
  const char *s = c;
  if ('V' == *e && strchr(b, '-'))
    {
      const char *s = hash_find(stems, (unsigned const char *)b);
      if (!s)
	{
	  fprintf(stderr, "stem: %s %s\n", c, b);
	  s = c;
	}
    }
  char ret[1024];
  const char *tilde = strchr(m, '~');
  if (tilde > m)
    strncpy(ret, m, tilde-m);
  else
    *ret = '\0';
  strcat(ret, s);
  strcat(ret, tilde+1);
  return strdup(ret);
}

void
morph(const char **atts)
{
  if (nmorph++)
    fputs("; ", outf);
  const char *m = findAttr(atts, "morph");
  const char *c = findAttr(atts, "cf");
  const char *e = findAttr(atts, "epos");
  if (*m)
    {
      const char *b = findAttr(atts, "base");
      if (*c && *b)
	{
	  char *x = normify(c, b, m, e);
	  fputs(x, outf);
	  free(x);
	}
      return;
    }
  fputs(c, outf);
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "xcl"))
    xcl_mode = 1;
  else if (!strcmp(name, "l"))
    {
      /* use of @n assumes #lm1: context will be inherited from ATF */
      if (!xcl_mode)
	hash_add(idmap,
		 pool_copy((unsigned const char *)findAttr(atts, "xml:id"), p),
		 pool_copy((unsigned const char *)findAttr(atts, "n"), p));
    }
  else if (!strcmp(name, "xff:f"))
    morph(atts);
  else if (!strcmp(name, "d"))
    {
      const char *type = findAttr(atts, "type");
      if (!strcmp(type, "line-start"))
	{
	  if (nlines++)
	    fputs("\n", outf);
	  const char *lid = findAttr(atts, "ref");	  
	  fprintf(outf, "%s.\t", (const char*)hash_find(idmap, (unsigned const char *)lid));
	  nmorph = 0;
	}
    }
}

void
ei_eH(void *userData, const char *name)
{
}

void
expat_identity(const char *fname, const char *xml_id, FILE *outfp)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, ei_sH, ei_eH, NULL, outfp);
}

int
main(int argc, char **argv)
{
  load_stems();
  options(argc, argv, "");
  runexpat_omit_rp_wrap();
  p = pool_init();
  idmap = hash_create(256);
  outf = stdout;
  expat_identity(argv[optind], NULL, stdout);
  if (nlines++)
    fputs("\n", outf);
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    default: return 1;
    }
  return 0;
}
void help(void) {}
