#include <oraccsys.h>

/* read signatures and identify which languages are represented.
 * with -s option slice langs into 01bld/LANG/xtf.sig
 * with -t option read 02pub/tokl.tpc
 */
extern struct map *qpnnames(register const char *str, register size_t len);

int lang_mode = 1, qpn_mode = 1, slicing = 0, tokl_mode = 0;
const char *sigfile = NULL;
const char *tokl_hdr;
Hash *hlang;
Hash *hqpn;
Pool *p;

FILE *
lang_init(const char *lang)
{
  if (slicing)
    {
      char d[strlen("01bld/0")+strlen(lang)];
      strcpy(d, "01bld/");
      strcat(d, lang);
      if (-1 == mkdir(d, 0700) && EEXIST != errno)
	{
	  perror(lang);
	  exit(1);
	}
      char lang_fn[strlen(d)+strlen("/xtf.tpc0")];
      strcpy(lang_fn, d);
      strcat(lang_fn, "/xtf.");
      if (tokl_mode)
	strcat(lang_fn, "tpc");
      else
	strcat(lang_fn, "sig");
      FILE *fp = fopen(lang_fn, "w");
      if (!fp)
	{
	  perror(lang_fn);
	  exit(1);
	}
      hash_add(hlang, pool_copy((ucp)lang,p), fp);
      if (tokl_mode)
	fputs(tokl_hdr, fp);
      return fp;
    }
  else
    {
      hash_add(hlang, pool_copy((ucp)lang,p), "");
      return NULL;
    }
}

int
main(int argc, char **argv)
{
  FILE *fp;
  FILE *slice_fp = NULL;
  char *lp = NULL;
  const char **keys = NULL;
  size_t len = 0;
  int nkeys = 0;

  hlang = hash_create(32);
  hqpn = hash_create(32);
  p = pool_init();

  
  if (options(argc, argv, "lqst"))
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
      if (tokl_mode)
	{
	  if ('t' == *lp)
	    {
	      tokl_hdr = strdup(lp);
	      continue;
	    }
	  else if (strchr(lp, 0x01))
	    continue;
	}

      char orig[strlen(lp)+1];
      strcpy(orig, lp);
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
			      if (slicing)
				{
				  if (!(slice_fp = hash_find(hlang, (ucp)lang)))
				    slice_fp = lang_init(lang);
				  fputs(orig,slice_fp);
				  fputc('\n',slice_fp);
				}
			      else
				{
				  if (!hash_find(hlang, (ucp)lang))
				    (void)lang_init(lang);
				}
			    }
			}
		    }
		}
	      if (qpn_mode && epos[1] == 'N' && !epos[2])
		{
		  struct map *m = qpnnames(epos, strlen(epos));
		  if (m)
		    {
		      const char *lang = m->v;
		      if (slicing)
			{
			  if (!(slice_fp = hash_find(hlang, (ucp)lang)))
			    slice_fp = lang_init(lang);
			  fputs(orig,slice_fp);
			  fputc('\n',slice_fp);
			}
		      else
			{
			  if (!hash_find(hlang, (ucp)lang))
			    (void)lang_init(lang);
			}
		    }
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
	  if (slicing)
	    {
	      FILE *fp = hash_find(hlang, (uccp)keys[i]);
	      fclose(fp);
	    }
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
	      if (slicing)
		{
		  FILE *fp = hash_find(hqpn, (uccp)
				       keys[i]);
		  fclose(fp);
		}
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
    case 's':
      slicing = 1;
      break;
    case 't':
      sigfile = "02pub/tokl.tpc";
      tokl_mode = 1;
      break;
    default:
      /* usage() */
      break;
    }
  return 0;
}
