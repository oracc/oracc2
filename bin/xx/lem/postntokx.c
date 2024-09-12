#include <oraccsys.h>
#include "runexpat.h"

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

int verbose = 0;

List *phr = NULL;/* gather phrase here */
Pool *hp = NULL; /* hash-pool because many times same small strings */
Hash *ht = NULL; /* phrase hash with count-ptr value */
Memo *mi = NULL; /* mem for ints */
int postn = 0;   /* state variable: 1 = adding to phr */

static void
phrase(void)
{
  if (phr && list_len(phr))
    {
      int phr_len;
      const char **p = (const char **)list2array_c(phr, &phr_len);
      const char *s = vec_to_str((char**)p, phr_len, " ");
      const uchar *k = pool_copy((uccp)s, hp);
      free(p);
      free((void*)s);
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
    }
  
  if (!phr)
    {
      phr = list_create(LIST_SINGLE);
      postn = 0;
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
      fprintf(stderr, "cgp=%s\n", buf);
      return hpool_copy(buf, hp);
    }
  return NULL;
}

static void
process_l(const char **atts)
{
  const char *square = NULL, *inst = NULL;;
  const char *sig = NULL;
  while (*atts)
    {
      if (!strcmp(atts[0],"sig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0],"exosig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0],"newsig"))
	sig = (char*)atts[1];
      else if (!strcmp(atts[0],"inst"))
	inst = (char*)atts[1];
      atts += 2;
    }

  if (sig)
    {
      if ('{' == *sig)
	sig = strchr(sig, '}');
      if (sig)
	{
	  square = strchr(sig,']');
	  if (square)
	    {
	      ++square;
	      if (square[1] == 'N'/* && square[2] == '\''*/) /* at present we don't need to test for NN because we ignore CNJ anyway */
		{
		  phrase();
		}
	      else if (postn)
		{
		  const uchar *s = cgp((uccp)sig);
		  if (sig)
		    list_add(phr, (void*)s);
		  else /* non-signature is a stopper */
		    phrase();
		}
	    }
	  else
	    phrase(); /* non-sig is stop */
	}
    }
  else if (inst)
    {
      phrase();
      postn = 1;
    }    
}

void
startHandler(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|l"))
    process_l(atts);
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
  runexpat_omit_rp_wrap();
  hp = hpool_init();
  ht = hash_create(1024);
  mi = memo_init(sizeof(int), 1024);
  runexpatNS(i_stdin, NULL, startHandler, endHandler, "|");
  const char **k = hash_keys(ht);
  int i;
  for (i = 0; k[i]; ++i)
    {
      int *pi = hash_find(ht, (uccp)k[i]);
      printf("%d\t%s\n", *pi, k[i]);
    }
}
