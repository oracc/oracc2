#include <oraccsys.h>
#include "runexpat.h"
#include "bx.h"

/* Simple program based on expat-identity.c to extract b:cite
 * attributes from inputs; a future version may format the cites in
 * the output and/or add a reference to the .bib data to support cite
 * formatting.
 */

extern int optind;

int append_cites = 0;
const char *bibsfile = NULL; /* file of paths to .bib files */
int keys_mode = 0;
int no_output = 0;
int verbose = 0;

const char *keyfile = "bibkey.ltx";
const char *output = NULL;

Hash *keys = NULL;
Pool *p = NULL;

void
bx_cite(const char **atts)
{
  const char *k = findAttr(atts, "key");
  if (!hash_find(keys, (uccp)k))
    hash_add(keys, pool_copy((uccp)k, p), "");
}

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
  const char **ap = atts;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
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
  
  if (!strcmp(name, "b:cite"))
    bx_cite(atts);
}

void
ei_eH(void *userData, const char *name)
{
  printEnd(userData, name);
}

void
no_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "b:cite"))
    bx_cite(atts);
}

void
no_eH(void *userData, const char *name)
{
}

void
expat_identity(const char *fname, const char *xml_id, FILE *outfp)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist,
	       no_output ? no_sH : ei_sH,
	       no_output ? no_eH : ei_eH,
	       NULL, outfp);
}

void
dump_keys(void)
{
  int i, nk;
  const char **kk = hash_keys2(keys, &nk);
  qsort(kk, nk, sizeof(const char *), cmpstringp);
  FILE *keyfp = fopen(keyfile, append_cites ? "a" : "w");
  for (i = 0; i < nk; ++i)
    fprintf(keyfp, "\\nocite{%s}\n", kk[i]);
  fclose(keyfp);
}

int
main(int argc, char * const*argv)
{
  options(argc, argv, "ab:kno:v");
  keys = hash_create(1024);
  p = pool_init();
  mesg_init();
  if (keys_mode)
    {
      const char **bibs;
      if (argv[optind])
	bibs = (const char **)&argv[optind];
      else if (bibsfile)
	bibs = bx_bibs_file(bibsfile);
      else
	bibs = bx_bibs_file("-");
      if (verbose)
	{
	  int i;
	  for (i = 0; bibs[i]; ++i)
	    fprintf(stderr, "using .bib file %s\n", bibs[i]);
	}
      bx_keys(bibs, output);
      mesg_print(stderr);
    }
  else
    {
      runexpat_omit_rp_wrap();
      expat_identity(argv[optind], NULL, stdout);
      dump_keys();
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'a':
      append_cites = 1;
      break;
    case 'b':
      bibsfile = arg;
      break;
    case 'k':
      keys_mode = 1;
      break;
    case 'n':
      no_output = 1;
      break;
    case 'o':
      output = arg; /* in keys_mode a file; otherwise a directory */
      break;
    case 'v':
      ++verbose;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void) {}
