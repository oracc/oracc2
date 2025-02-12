#include <oraccsys.h>
#include "runexpat.h"
#include "lemmer.h"

/* postntokx : pick up all the tokens that follow an 'n' and
   concatenate them into a phrase; count the occurrences of the phrase
   and print a table of the counts and multi-token sequences.

   Input processing is basic:

   xcl:l with pos=n restarts harvesting
   xcl:c end stops harvesting
   xcl:d with type=end* stops harvesting
   xcl:l with pos=NN (PN, DN, etc.) or n stops harvesting
   sequences of xcl:l with pos=N|AJ|V are collected together to make the phrase
   	(V because most AJ in Sum are V)

 */

extern const uchar **runexinputs(const char *f);

FILE *f_ref;
int verbose = 0;
char curr_ref[32];/*P123456.1234.1234 is reasonable maximum expectation*/
List *phr = NULL;/* gather phrase here */
List *ref = NULL;/* gather @ref to wid here */
Pool *hp = NULL; /* hash-pool because many times same small strings */
Hash *ht = NULL; /* phrase hash with count-ptr value */
Memo *mi = NULL; /* mem for ints */
int waiting_for_gur = 0;

static void
phrase(void)
{
  if (phr && list_len(phr))
    {
      int len;
      const char **p = (const char **)list2array_c(phr, &len);
      const char *s = vec_to_str((char**)p, len, " ");
      const uchar *k = pool_copy((uccp)s, hp);
      free(p);
      free((void*)s);
      const char **rr = (const char **)list2array_c(ref, &len);
      const char *r = vec_to_str((char**)rr, len, "+");
      fprintf(f_ref, "%s\t%s\n", r, k);
      free(rr);
      free((void*)r);
      int *pi = hash_find(ht, k);
      if (pi)
	++*pi;
      else
	{
	  pi = memo_new(mi);
	  *pi = 1;
	  hash_add(ht, k, pi);
	}
      list_free(phr, NULL);
      phr = NULL;
      list_free(ref, free);
      ref = NULL;
      waiting_for_gur = 0;
    }
  
  if (!phr)
    {
      phr = list_create(LIST_SINGLE);
      ref = list_create(LIST_SINGLE);
    }
}

static const uchar *
cgp(const unsigned char *sig)
{
  uchar *end = (ucp)strchr((ccp)sig, '\'');
  if (end)
    {
      uchar buf[(end-sig)+1];
      const uchar *s = sig;
      uchar *b = buf;
      s = (uccp)strchr((ccp)sig, '='); /* skip everything up to CF */
      if (s)
	{
	  ++s;
	  while (s < end)
	    {
	      if ('/' == s[0] && '/' == s[1])
		{
		  s = (ucp)strchr((ccp)s, ']');
		  if (!s)
		    return NULL; /* brook no incompetence */
		  else
		    *b++ = *s++;
		}
	      else
		*b++ = *s++;
	    }
	  *b = '\0';
	  /*fprintf(stderr, "cgp=%s\n", buf);*/
	  return hpool_copy(buf, hp);
	}
    }
  return NULL;
}

static void
process_f(const char **atts)
{
  const char *pos = NULL, *form = NULL;
  while (*atts)
    {
      if (!strcmp(atts[0],"form"))
	form = (char*)atts[1];
      if (!strcmp(atts[0],"pos"))
	pos = (char*)atts[1];
      atts += 2;
    }
  if (form && *form && pos && *pos && !strcmp(pos, "n"))
    {
      list_add(phr, hpool_copy((ucp)form, hp));
      list_add(ref, strdup(curr_ref));
    }
}

static void
process_l(const char **atts)
{
  const char *square = NULL, *sig = NULL;
  while (*atts)
    {
      if (!strcmp(atts[0],"sig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0],"exosig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0],"newsig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0], "ref"))
	strcpy(curr_ref, atts[1]);
      atts += 2;
    }

  if (sig && *sig)
    {
      if ('{' == *sig)
	sig = strchr(sig, '}');
      if (sig)
	{
	  square = strchr(sig,']');
	  if (square)
	    {
	      const uchar *s = cgp((uccp)sig);
	      struct numtab *nw = numword((ccp)s, strlen((ccp)s));
	      if (nw)
		{
		  list_add(phr, (void*)s);
		  list_add(ref, strdup(curr_ref));
		  if (nw->gur)
		    waiting_for_gur = 1;
		}
	      else if (!waiting_for_gur)
		phrase();
	    }
	  else
	    phrase(); /* non-sig is stop */
	}
    }
}

void
startHandler(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|l"))
    process_l(atts);
  else if (!strcmp(name,"http://oracc.org/ns/xff/1.0|f"))
    process_f(atts);
  else if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|d"))
    phrase();
  /* can ignore open 'c' because we wrap on close 'c' */
}

void
endHandler(void *userData, const char *name)
{
  if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|c"))
    phrase();
}

int
main(int argc, char **argv)
{
  hp = hpool_init();
  ht = hash_create(1024);
  mi = memo_init(sizeof(int), 1024);
  f_ref = fopen("num.ref", "w");
  if (!f_ref)
    {
      fprintf(stderr, "numtokx: unable to open num.ref. Stop.\n");
      exit(1);
    }
  const uchar **files = runexinputs(NULL);
  runexpatNS(i_list, files, startHandler, endHandler, "|");
  const char **k = hash_keys(ht);
  int i;
  for (i = 0; k[i]; ++i)
    {
      int *pi = hash_find(ht, (uccp)k[i]);
      printf("%d\t%s\n", *pi, k[i]);
    }
  fclose(f_ref);
}
