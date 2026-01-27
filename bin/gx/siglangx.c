#include <oraccsys.h>

extern struct map *qpnnames(register const char *str, register size_t len);

int lang_mode = 1, qpn_mode = 1;

int
main(int argc, char **argv)
{
  FILE *fp;
  const char *sigfile = NULL;
  char *lp = NULL;
  Hash *hlang = hash_create(32);
  Hash *hqpn = hash_create(32);
  Pool *p = pool_init();
  const char **keys = NULL;
  size_t len = 0;
  int nkeys = 0;

  if (options(argc, argv, "lq"))
    goto error;
  if (!argv[optind])
    {
      sigfile = "01bld/project.sig";
    }
  else
    sigfile = argv[optind];

  if (!strcmp(sigfile, "-"))
    fp = stdin;
  else if (!(fp = fopen(sigfile, "r")))
    {
      fprintf(stderr, "siglangx: unable to open sig file %s. Stop.\n", sigfile);
      goto error;
    }
    
  while (NULL != (lp = (char*)loadoneline(fp, &len)))
    {
      char *pos, *epos;
      pos = strchr(lp, ']');
      if (pos)
	{
	  epos = strchr(++pos, '\'');
	  if (epos)
	    {
	      *epos++ = '\0';
	      char *norm = strchr(epos,'$');
	      if (norm)
		*norm = '\0';
	      if (lang_mode)
		{
		  if ('N' != pos[1] || pos[2]) /* not proper noun, i.e., not /^[A-Z]N$/ */
		    {
		      char *lang = strchr(lp, '%');
		      if (lang)
			{
			  char *form = strchr(++lang, ':');
			  if (form)
			    {
			      char *lang_end = form;
			      *form = '\0';
			      /* trim off script codes, e.g., akk-x-mbperi-949 */
			      while (lang_end > lang && isdigit(lang_end[-1]))
				--lang_end;
			      if (lang_end > lang && '-' == lang_end[-1])
				lang_end[-1] = '\0';
			      if (!hash_find(hlang, (ucp)lang))
				hash_add(hlang, pool_copy((ucp)lang,p), "");
			    }
			}
		    }
		}
	      if (qpn_mode && epos[1] == 'N' && !epos[2])
		{
		  struct map *m = qpnnames(epos, strlen(epos));
		  if (m && !hash_find(hqpn,(ucp)m->v))
		    hash_add(hqpn,(ucp)m->v,"");
		}
	    }
	}
    }
  if (lang_mode)
    {
      keys = hash_keys2(hlang,&nkeys);
      qsort(keys, nkeys, sizeof(const char *), cmpstringp);
      int i = 0;
      while (keys[i])
	{
	  if (i)
	    putchar(' ');
	  fputs(keys[i++], stdout);
	}
    }
  if (qpn_mode)
    {
      if (lang_mode)
	putchar(' ');
      keys = hash_keys2(hqpn,&nkeys);
      if (nkeys)
	{
	  fputs("qpn ", stdout);
	  qsort(keys, nkeys, sizeof(const char *), cmpstringp);
	  int i = 0;
	  while (keys[i])
	    {
	      if (i)
		putchar(' ');
	      fputs(keys[i++], stdout);
	    }
	}
    }
  return 0;
 error:
  return 1;
}

void help(void) { }
int opts(int c, const char *opt)
{
  switch (c)
    {
    case 'l':
      lang_mode = 1;
      qpn_mode = 0;
      break;
    case 'q':
      lang_mode = 0;
      qpn_mode = 1;
      break;
    default:
      /* usage() */
      break;
    }
  return 0;
}
