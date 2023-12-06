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
  struct data *next;
} Data;

typedef struct refs
{
  const char *value;
  const char *project;
  const char *n;
  const char *xis;
  const char *clabel;
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
  if (i && i < nr)
    {
      int j;
      for (j = i; j < nr; ++j)
	{
	  char *t = np[j];
	  while (*t)
	    ++t;
	  *t = '\0';
	}
      return strdup(np[i]);
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
  int nrefseg = nseg(ref_label);
  int nnxtseg = nseg(nxt_label);
  if (nrefseg == nnxtseg)
    return label_collapse_sub(ref_label, nrefseg, nxt_label, nnxtseg, nbits);
  else
    return NULL;
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
      dm->label = (ccp)pool_copy((ucp)findAttr(atts, "label"), p);
      dm->sref = (ccp)pool_copy((ucp)findAttr(atts, "sref"), p);
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

static int
range_true(const char *last_bit, const char *curr_bit)
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
      if (ci - li == 1)
	{
	  /* difference is 1--test anything after the numbers */
	  while (*lpp && *lpp++ == *cpp++)
	    ;
	  if (!*lpp && !*cpp)
	    return 1;
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
      Data *dp = r.d;
      int range_open = 0;
      const char *r1 = dp->label;
      const char *last_bit = last_little_bit(r1);
      list_add(lbits, (char*)r1);
      list_add(lxis, (char*)dp->sref);
      for (dp = dp->next; dp; dp = dp->next)
	{
	  list_add(lxis, "+");
	  list_add(lxis, (char*)dp->sref);
	  int nbits;
	  const char *bit = label_collapse(r1, dp->label, &nbits);
	  /* If this is NULL there is no reduction in label */
	  if (NULL == bit) /* All the bits matched */
	    {
	      /* This can't happen unless the input data has duplicates */
	      abort();
	    }
	  else if (!nbits) /* None of the bits matched */
	    {
	      if (range_open)
		{
		  list_add(lbits, (char*)last_bit);
		  range_open = 0;
		  last_bit = NULL;
		}
	      r1 = dp->label; /* reset the reference label */
	      list_add(lbits, ", ");
	      list_add(lbits, (char*)r1);
	      last_bit = last_little_bit(r1);
	    }
	  else
	    {
	      /* some bits matched; if it's only the last bit check to
		 see if we have a range going */
	      if (last_bit && range_true(last_bit, bit))
		{
		  if (!range_open)
		    {
		      list_add(lbits, "-");
		      range_open = 1;
		    }
		}
	      else
		{
		  list_add(lbits, (char*)last_bit);
		}
	      last_bit = last_little_bit(bit);
	    }
	}
      if (range_open)
	list_add(lbits, (char*)last_bit);
      r.xis = (const char *)list_to_str2(lxis,"");
      r.clabel = (const char *)list_to_str2(lbits,"");
    }
}

#define pElem(e)   fprintf(xfp, "<%s", e)
#define pAttr(a,v) fprintf(xfp, " %s=\"%s\"",a,xmlify((ucp)v))
static void
pRefo(void)
{
  pElem("lex:group");
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
