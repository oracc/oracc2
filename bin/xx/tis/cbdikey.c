#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runexpat.h"

#include "ikeysub.h"

/* Add @ikey to an oracc1 cbd which must have @oid on word and sense
   for this to work properly; primarily intended for neo while we are
   transitioning to oracc2 IS architecture */

extern int options(int, char**,const char*);
extern int optind;
int verbose = 0;

const char *curr_oid;

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '>')
	fputs("&gt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
#if 0
      else if (*s == '\'')
	fputs("&apos;",frag_fp);
#endif
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
  const char **ap = atts, *xis = NULL, *n = NULL;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);

  if (!strcmp(name, "entry") || !strcmp(name, "sense"))
    {
      if (curr_oid)
	{
	  free((void*)curr_oid);
	  curr_oid = NULL;
	}
    }
  
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  if (!strcmp(ap[0], "oid"))
	    curr_oid = strdup(ap[1]);
	  else if (!strcmp(ap[0], "xis"))
	    xis = ap[1];
	  else if (!strcmp(ap[0], "n"))
	    n = ap[1];
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
	}
    }

  if (xis && curr_oid)
    {
      if (n)
	{
	  struct ikeysubtab *ip = ikeysub(name, strlen(name));
	  if (ip)
	    {
	      if (*ip->sub)
		{
		  char kbuf[strlen(curr_oid)+strlen(ip->sub)+strlen(n)+strlen("...0")];
		  sprintf(kbuf, "%s.%s.%s", curr_oid, ip->sub, n);
		  fprintf(fp, " ikey=\"%s\"", kbuf);
		}
	    }
	  else
	    fprintf(stderr, "cbdikey: name %s not in ikeysub table\n", name);
	}
      else
	fprintf(stderr, "cbdikey: NULL @n when oid=%s name=%s xis=%s\n", curr_oid, name, xis);
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
  printStart(userData, name, atts);
}

void
ei_eH(void *userData, const char *name)
{
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
