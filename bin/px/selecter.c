#include <oraccsys.h>
#include <runexpat.h>

Hash *selected = NULL;
extern int optind;

static void run_selecter(const char *fname, FILE *outfp);

int
selecter(const char *file, char * const*ids)
{
  int i = 0;
  runexpat_omit_rp_wrap();
  selected = hash_create(1);
  while (ids[i])
    hash_add(selected, (ucp)ids[i++], "");
  run_selecter(file, stdout);
  return 0;
}

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
static void
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
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}

static void
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
      else
	fprintf(fp, " class=\"%s\"", class);
    }
  fputc('>', fp);
}

static void
printEnd(FILE *fp, const char *name)
{
  printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

static void
ei_sH(void *userData, const char *name, const char **atts)
{
  printStart(userData, name, atts);
}

static void
ei_eH(void *userData, const char *name)
{
  printEnd(userData, name);
}

static void
run_selecter(const char *fname, FILE *outfp)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, ei_sH, ei_eH, NULL, outfp);
}

#if 0
int verbose = 0;

int
main(int argc, char * const *argv)
{
  options(argc, argv, "");
  const char *file = argv[optind++];
  selecter(file, argv+optind);
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
#endif
