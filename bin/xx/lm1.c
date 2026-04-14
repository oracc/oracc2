#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pool.h>
#include <hash.h>
#include <xml.h>
#include <runexpat.h>

extern int options(int, char**,const char*);
extern int optind;
int verbose = 0;

int nlines;
int nmorph;
int xcl_mode;
FILE *outf;
Hash *idmap;
Pool *p;

void
morph(const char **atts)
{
  if (nmorph++)
    fputs("; ", outf);
  fputs(findAttr(atts, "norm0"), outf);
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
