#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include <runexpat.h>
#include <xmlify.h>

extern int options(int, char**,const char*);
extern int optind;
int verbose = 0;

typedef struct data
{
  const char *xmlid;
  const char *label;
  const char *sref;
  const char *wref;
  const char *word;
  const char *base;
  const char *pos;
  const char *line;
  struct data *next;
} Data;

typedef struct refs
{
  const char *value;
  const char *project;
  const char *n;
  const char *clabel;
  const char *xis;
  Data *d;
  Data *last;
} Refs;

static Memo *m;
static Pool *p;
static Refs r;
static FILE *xfp;

const char *
label_collapse_sub(const char *r, int nr, const char *n, int nn, int *nbits)
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

  if (nbits)
    *nbits = i;

  /* return the segments that don't match; if all the segments matched
     (i == nr) return NULL */  
  if (i < nr) /* i can be 0 here if bits are 38 and 39 */
    {
      return n + (np[i] - nbuf);
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
label_collapse(const char *ref_label, const char*nxt_label, int *nbits)
{
  return label_collapse_sub(ref_label, nseg(ref_label), nxt_label, nseg(nxt_label), nbits);
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

static int
group_of_refs(const char *name, const char **atts)
{
  if (!strcmp(name, "lex:group"))
    {
      const char *type = findAttr(atts, "type");
      return !strcmp(type,"refs");
    }
  return 0;
}

static const char *
dAttr(const char **ap, const char *key)
{
  const char *k = findAttr(ap, key);
  if (k && *k)
    return (ccp)pool_copy((ucp)k, p);
  else
    return NULL;
}

void
printStart(FILE *fp, const char *name, const char **atts)
{  
  printText((const char*)charData_retrieve(), fp);

  if (group_of_refs(name, atts))
    {
      if (r.value)
	{
	  fprintf(stderr, "internal error: r.value is set when it should be NULL\n");
	  exit(1);
	}
      m = memo_init(sizeof(Data), 256);
      p = pool_init();
      r.value = (ccp)pool_copy((ucp)findAttr(atts, "value"), p);
      r.project = (ccp)pool_copy((ucp)findAttr(atts, "project"), p);
      r.n = (ccp)pool_copy((ucp)findAttr(atts, "n"), p);
    }
  else if (!strcmp(name, "lex:data"))
    {
      Data *dm = memo_new(m);
      dm->xmlid = (ccp)pool_copy((ucp)get_xml_id(atts), p);
      dm->label = dAttr(atts, "label");
      dm->sref = dAttr(atts, "sref");
      dm->wref = dAttr(atts, "wref");
      dm->word = dAttr(atts, "word");
      dm->base = dAttr(atts, "base");
      dm->line = dAttr(atts, "line");
      if (r.last)
	{
	  r.last->next = dm;
	  r.last = r.last->next;
	}
      else
	r.last = r.d = dm;
    }
  else
   {
     fprintf(fp, "<%s", name);
     if (atts)
       {
	 const char **ap;
	 for (ap = atts; ap[0]; )
	   {	  
	     fprintf(fp, " %s=\"",*ap++);
	     printText(*ap++, fp);
	     fputc('"', fp);
	   }
	 fputc('>', fp);
       }
   }
}

static void
add_word(List *l, const char *word)
{
  char w[strlen(word)+1], *b;
  strcpy(w,word);
  if ((b = strchr(w, ']')))
    *++b = '\0';
  list_add(l, " (");
  list_add(l, pool_copy((ucp)w,p));
  list_add(l, ")");
}

static int
range_true(const char *last_bit, const char *curr_bit, Data *last_dp, Data *this_dp)
{
  const char *lp = last_bit, *cp = curr_bit;
  /* test until the first digit */
  while (!isdigit(*lp) && *lp++ == *cp++)
    ;
  if (isdigit(*lp) && isdigit(*cp))
    {
      /* now test the numbers */
      const char *lpp = lp, *cpp = cp;
      while (isdigit(*lpp))
	++lpp;
      while (isdigit(*cpp))
	++cpp;
      int li = atoi(lp);
      int ci = atoi(cp);
      if (ci == li || ci - li == 1)
	{
	  /* difference is 0 or 1--test anything after the numbers */
	  int ok = 0;
	  if ((!*lpp || isalpha(*lpp)) && isalpha(*cpp))
	    {
	      if (!*lpp && 'a' == cpp[strlen(cpp)-1])
		ok = 1;
	      else
		{
		  /* Try to detect 56a 56b schemes and merge sequences */
		  if (strlen(lpp) == strlen(cpp))
		    {
		      /* strings are same length; see if the last letter
		       * is a differerence-by-one; this is imperfect
		       * because 56az 56ba fails, but that is rare to the
		       * point of possibly never even happening so we just
		       * live with the possibility that such sequences are
		       * not collapsed.
		       */
		      if (cpp[strlen(cpp)-1] - lpp[strlen(lpp)-1] == 1)
			ok = 1;
		    }
		}
	    }
	  else
	    {
	      while (*lpp && *lpp++ == *cpp++)
		;	      
	      if (!*lpp && !*cpp)
		ok = 1;
	    }
	  if (ok)
	    {
	      /* the range itself is good; now check any word
		 annotations; at this point we don't discriminate
		 about the associate base or POS. Since these all have
		 the same reading/spelling, the base should be
		 identical and if it's not it's likely an error.
		 POS-only items are not sufficiently interesting for
		 our purposes. */
	      if (!last_dp
		  || !last_dp->word
		  || !this_dp->word
		  || (last_dp && !strcmp(last_dp->word, this_dp->word)))
		return 1;
	    }
	}
    }
  return 0;
}

static const char *
last_little_bit(const char *b)
{
  const char *sp = strrchr(b, ' ');
  return (ccp)pool_copy((ucp)(sp?++sp:b),p);
}

static void
lex_process_data(void)
{
  if (r.d)
    {
      List *lbits = list_create(LIST_SINGLE);
      List *lxis = list_create(LIST_SINGLE);
      Data *dp = r.d, *last_dp = r.d;
      int range_open = 0;
      const char *r1 = dp->label;
      const char *range_word = NULL;
      const char *last_bit = last_little_bit(r1);
      list_add(lbits, (char*)r1);
      list_add(lxis, (char*)r.project);
      list_add(lxis, ":");
      list_add(lxis, (char*)dp->sref);
      for (dp = dp->next; dp; last_dp = dp, dp = dp->next)
	{
	  list_add(lxis, "+");
	  list_add(lxis, (char*)dp->sref);
	  int nbits;
	  const char *bit = label_collapse(r1, dp->label, &nbits);
	  /* If this is NULL there is no reduction to be done in the new label */
	  if (NULL == bit) /* All the bits matched */
	    {
	      /* This can't happen unless the input data has duplicates */
	      fprintf(stderr, "lex-collapse-refs: duplicate label @%s %s\n", dp->xmlid, dp->label);
	    }
	  else if (!nbits && strchr(r1,' ')) /* None of the bits matched and there's more than one bit */
	    {
	      if (range_open)
		{
		  list_add(lbits, (char*)last_bit);
		  range_open = 0;
		  last_bit = NULL;
		  if (range_word)
		    add_word(lbits, range_word);
		}
	      else
		{
		  /* add any pending word from the last bit added */
		  if (last_dp && last_dp->word)
		    add_word(lbits,last_dp->word);
		}
	      r1 = dp->label; /* reset the reference label */
	      list_add(lbits, ", ");
	      list_add(lbits, (char*)r1);
	      last_bit = last_little_bit(r1);
	    }
	  else if (nbits && strchr(bit, ' ')) /* Some bits matched,
						 but the remnant is
						 more than just the
						 last segment, so
						 reset r1 */
	    {
	      if (range_open)
		{
		  list_add(lbits, (char*)last_bit);
		  range_open = 0;
		  last_bit = NULL;
		  if (!range_word && dp->word)
		    range_word = dp->word;
		  
		}
	      /* add any pending word from the last bit added */
	      if (last_dp && last_dp->word)
		add_word(lbits,last_dp->word);
	      r1 = dp->label;
	      list_add(lbits, ", ");
	      list_add(lbits, (char*)r1);
	      last_bit = last_little_bit(r1);
	    }
	  else
	    {
	      /* some bits matched; if it's only the last bit check to
		 see if we have a range going */
	      if (last_bit && range_true(last_bit, bit, last_dp, dp))
		{
		  if (!range_open)
		    {
		      list_add(lbits, "-");
		      range_open = 1;
		      if (last_dp && last_dp->word)
			range_word = last_dp->word;
		      else if (dp->word)
			range_word = dp->word;
		    }
		}
	      else
		{
		  /* add any pending word from the last bit added */
		  if (last_dp && last_dp->word)
		    add_word(lbits, last_dp->word);
		  list_add(lbits, ", ");
		  list_add(lbits, (char*)bit);
		}
	      last_bit = last_little_bit(bit);
	    }
	}
      if (range_open)
	{
	  list_add(lbits, (char*)last_bit);
	  if (range_word)
	    add_word(lbits, range_word);
	}
      else
	{
	  if (last_dp && last_dp->word)
	    add_word(lbits,last_dp->word);
	}
      r.xis = (ccp)list_to_str2(lxis,"");
      r.clabel = (ccp)list_to_str2(lbits,"");
      list_free(lxis, NULL);
      list_free(lbits, NULL);
    }
}

#define pElem(e)   fprintf(xfp, "<%s", e)
#define pAttr(a,v) if ((v)&&(*v)) fprintf(xfp, " %s=\"%s\"",a,xmlify((ucp)v))
static void
pRefo(void)
{
  pElem("lex:group");
  pAttr("type","refs");
  pAttr("value", r.value);
  pAttr("project", r.project);
  pAttr("n", r.n);
  pAttr("xis", r.xis);
  pAttr("clabel", r.clabel);
  fputc('>', xfp);
}

static void
pRefc(void)
{
  /*fputs("</lex:group>",xfp);*/ /* let printEnd do this */
}

static void
pData(void)
{
  Data *dp;
  for (dp = r.d; dp; dp = dp->next)
    {
      pElem("lex:data");
      pAttr("xml:id",dp->xmlid);
      pAttr("label",dp->label);
      pAttr("sref",dp->sref);
      pAttr("wref",dp->wref);
      pAttr("word",dp->word);
      pAttr("base",dp->base);
      pAttr("pos",dp->pos);
      fputs("/>",xfp);
    }
}

void
printEnd(FILE *fp, const char *name)
{
  if (!strcmp(name, "lex:group") && r.value)
    {
      xfp = fp;
      lex_process_data();
      pRefo();
      pData();
      pRefc();
      memo_term(m);
      pool_term(p);
      r.value = r.project = r.n = r.xis = r.clabel = NULL;
      r.d = r.last = NULL;
    }

  if (strcmp(name, "lex:data"))
    {
      printText((const char *)charData_retrieve(), fp);
      fprintf(fp, "</%s>", name);
    }
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
