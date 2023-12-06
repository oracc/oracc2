#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "runexpat.h"

extern int options(int, char**,const char*);
extern int optind;
int verbose = 0;

const char *
label_collapse_sub(const char *r, int nr, const char *n, int nn)
{
  char rbuf[strlen(r)+1], nbuf[strlen(n)+1], *rp[nr+1], *np[nn+1];
  strcpy(rbuf, r);
  strcpy(nbuf, n);
  char *p;
  int i;
  for (i = 0, p = rbuf; *p; )
    {
      rp[i++] = p;
      while (*p && !isspace(*p))
	++p;
      if (isspace(*p))
	{
	  *p++ = '\0';
	  while (isspace(*p))
	    ++p;
	}
    }
  rp[i] = NULL;
  for (i = 0, p = nbuf; *p; )
    {
      np[i++] = p;
      while (*p && !isspace(*p))
	++p;
      if (isspace(*p))
	{
	  *p++ = '\0';
	  while (isspace(*p))
	    ++p;
	}
    }
  np[i] = NULL;
  for (i = 0; rp[i]; ++i)
    {
      if (strcmp(rp[i], np[i]))
	break;
    }
  /* return the segments that don't match */
  if (i)
    {
      int j;
      for (j = i; j < nr; ++j)
	{
	  char *t = np[j];
	  while (*t)
	    ++t;
	  *t = '\0';
	}
      return strdup(np[i]);
    }
  else
    return NULL;
}

static int
nseg(const char *s)
{
  int i = 1;
  while (*s)
    {
      if (isspace(*s))
	{
	  ++i;
	  do
	    ++s;
	  while (isspace(*s));
	}
      else
	++s;
    }
  return i;
}

const char *
label_collapse(const char *ref_label, const char*nxt_label)
{
  int nrefseg = nseg(ref_label);
  int nnxtseg = nseg(nxt_label);
  if (nrefseg == nnxtseg)
    return label_collapse_sub(ref_label, nrefseg, nxt_label, nnxtseg);
  else
    return NULL;
}

void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}

void
printStart(FILE *fp, const char *name, const char **atts)
{
  static int in_refs = 0;
  static const char *r1 = NULL;
  const char **ap = atts, *short_label = NULL;
  printText((const char*)charData_retrieve(), fp);
  if (!strcmp(name, "lex:group"))
    {
      if (findAttr(atts, "refs"))
	in_refs = 1;
      else
	in_refs = 0;
    }
  else if (!strcmp(name, "lex:data"))
    {
      const char *l = findAttr(atts, "label");
      if (l)
	{
	  if (in_refs == 1)
	    {
	      r1 = strdup(l);
#if 0
	      fprintf(stderr, "ref%d: %s = %s\n", in_refs, x, l);
#endif
	    }
	  else
	    {
	      short_label = label_collapse(r1, l);
#if 0
	      if (strcmp(r1, short_label))
		fprintf(stderr, "ref%d: %s = %s => %s\n", in_refs, x, r1, short_label);
	      else
		fprintf(stderr, "ref%d: %s = %s ~~ %s\n", in_refs, x, r1, short_label);
#endif
	    }
	  ++in_refs;
	}
    }
  fprintf(fp, "<%s", name);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
	}
      if (short_label)
	{
	  fprintf(fp, " slabel=\"");
	  printText(short_label, fp);
	  fputc('"', fp);
	  free((char*)short_label);
	}
    }
  fputc('>', fp);
}

void
printEnd(FILE *fp, const char *name)
{
  printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  /*  if (strcmp(name, "rp-wrap")) */
    printStart(userData, name, atts);
}

void
ei_eH(void *userData, const char *name)
{
  /*  if (strcmp(name, "rp-wrap"))*/
    printEnd(userData, name);
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
  expat_identity(argv[optind], NULL, stdout);
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
void help() {}
