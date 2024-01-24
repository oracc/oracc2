#include <oraccsys.h>
#include <asl.h>
#include <tis.h>

static void
tis_strip_iss(char *lp)
{
  char *tab = strrchr(lp, '\t');
  *tab = '\0';
}

/* Read the corpus statistics from a .tis file into a hash-by-oid, sl->idata.
 */
void
sx_idata_init(struct sl_signlist *sl, const char *idata_file, const char *idata_type)
{
  char *lp;
  char type[4];
  struct tis_data t;
  FILE *fp = NULL;

  t.iss = NULL;
  t.subs = NULL;

  sl->h_idata = hash_create(1024);
  sl->m_idata = memo_init(sizeof(struct tis_data), 256);
  
  if (idata_type && strlen(idata_type) < 3)
    sprintf(type, "%s.", idata_type);
  else
    return; /* this is trapped in option processing in sx.c */

  if (!(fp = fopen(idata_file, "r")))
    {
      perror(idata_file);
      return;
    }
  
  while ((lp = (char*)loadoneline(fp, NULL)))
    {
      if ('o' == *lp)
	{
	  tis_strip_iss(lp);
	  t.key = pool_copy(lp, sl->p);
	  lp = strchr(t.key, '\t');
	  *lp++ = '\0';
	  if (!strncmp(lp, type, 3))
	    {
	      if (!strchr(t.key, '|')) /* may use meta one day */
		{
		  t.ref = lp;
		  lp = strchr(lp, '\t'); *lp++ = '\0';
		  t.cnt = lp;
		  lp = strchr(lp, '\t'); *lp++ = '\0';
		  t.pct = lp;
		  struct tis_data *tp = memo_new(sl->m_idata);
		  *tp = t;
		  hash_add(sl->h_idata, (uccp)t.key, tp);
		}
	    }
	}
    }
  loadoneline(NULL, NULL);
}

const unsigned char *
sx_idata_key(const char *soid, const char *foid, unsigned const char *v)  
{
  static unsigned char *k = NULL;
  static int k_alloced = 0;
  int klen = strlen(soid)+strlen(foid)+strlen((ccp)v) + 3;
  if (klen > k_alloced)
    {
      while (klen > k_alloced)
	k_alloced += 128;
      k = realloc(k, klen);
    }
  sprintf((char*)k, "%s.%s.%s", soid, foid, v);
  return k;
}
