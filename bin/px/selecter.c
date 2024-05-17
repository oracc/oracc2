#include <oraccsys.h>
#include <runexpat.h>

Hash *selected = NULL;

extern int optind;
int verbose = 0;

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
  int selectme = 0;
  const char *class = NULL;
  const char **ap = atts;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  if (!strcmp(*ap, "class"))
	    {
	      class = *++ap;
	      ++ap;
	    }
	  else
	    {
	      if (!strcmp(*ap, "id") && hash_find(selected, (ucp)ap[1]))
		selectme = 1;
	      fprintf(fp, " %s=\"",*ap++);
	      printText(*ap++, fp);
	      fputc('"', fp);
	    }
	}
      if (selectme)
	{
	  fputs(" class=\"", fp);
	  if (class)
	    fprintf(fp, "%s ", class);
	  fputs("selected\"", fp);
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
  const char *file = argv[optind++];
  selected = hash_create(1);
  while (argv[optind])
    hash_add(selected, (ucp)argv[optind++], "");
  expat_identity(file, NULL, stdout);
  
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    default: return 1;
    }
  return 0;
}
void help() {}
