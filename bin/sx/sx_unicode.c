#include <ctype128.h>
#include <oraccsys.h>
#include <signlist.h>
#include <gvl.h>
#include <sll.h>
#include <sx.h>
#include <pcre2if.h>

static Hash *ucode;
static Hash *unames;
static Hash *uneeds;
static Hash *uoids;
static Hash *urem;
static Hash *useqs;
static Hash *usigns;
static Hash *utf8s;

static const char *last_deep;
static const char *last_uoid;

static const char *sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g, int *multi);
static const char *sx_unicode_useq(const char *m, Pool *p);
static const char *sx_unicode_useq_m(const char *m, struct pcre2if_m *mp, Pool *p);
static const char *sx_unicode_useq_r(const char *m, int from, int to, Pool *p);

static int trace_mangling = 0;

/* Longer strings sort first */
static int cmp_by_len(const void *a, const void *b)
{
  return strlen(*(char**)b) - strlen(*(char**)a);
}

#define cppstr(s) #s
#define set_check_U(field) \
    if (fup->field) \
    { \
      if (sp->U.field) \
	{ \
	  if (strcmp((ccp)sp->U.field, (ccp)fup->field))		\
	    mesg_verr(&fip->mloc, "discrepant field: %s vs %s\n", fup->field, sp->U.field); \
	  else if (set_check_verbose) \
	    fprintf(stderr, "sp->U.%s %s == fp->U.%s %s\n", cppstr(field), sp->U.field, cppstr(field), fup->field); \
	} \
      else \
	{ \
          if (set_check_verbose) \
	    fprintf(stderr, "updating sp->U.%s to fp->U.%s %s\n", cppstr(field), cppstr(field), fup->field); \
	  sp->U.field = fup->field; \
	} \
    }

static void
set_and_check_U_data(struct sl_sign *sp, struct sl_inst *fip)
{
  struct sl_unicode *fup = &fip->u.f->U;
  int set_check_verbose = 0;
  set_check_U(uhex);
  set_check_U(useq);
  set_check_U(upua);
  set_check_U(utf8);
  set_check_U(umap);
  set_check_U(urev);
  set_check_U(uname);
  if (sp->U.unotes && fup->unotes)
    mesg_verr(&fip->mloc, "@unote on multiple @form entries; remove duplicates\n");
  else
    sp->U.unotes = fup->unotes;
}

void
sx_unicode_U_data(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *sp = sl->signs[i];
      List *lip = NULL;
      if (sp->xref)
	lip = sp->formdef->u.f->insts;
      else
	lip = sp->as_form;
      /* Now lip is a list of sl_inst * which is all the form-insts
	 that have sp as their form->sign */
      struct sl_inst *ip;
      for (ip = list_first(lip); ip; ip = list_next(lip))
	set_and_check_U_data(sp, ip);
    }
}

void
sx_unicode(struct sl_signlist *sl)
{
  ucode = hash_create(1024);
  unames = hash_create(1024);
  uneeds = hash_create(512);
  uoids = hash_create(1024);
  urem = hash_create(10);
  useqs = hash_create(512);
  usigns = hash_create(1024);
  utf8s = hash_create(512);

  if (sl->config->domain && !strcmp(sl->config->domain,"pc"))
    adding_useq_messages = 0;
  
  /* Index the signs that have unames -- those are encoded so we treat
     them as atoms even if they are compounds */
  int i;  
  for (i = 0; i < sl->nsigns; ++i)
    {
      if (sl->signs[i]->inst->valid)
	{
	  struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
	  if (Up->upua)
	    Up->uhex = Up->upua; /* Treat PUA and main encoding as the
				    same from here on; this could
				    actually be done at parse time */
	  if (Up->uname)
	    {
	      if (Up->uhex)
		{
		  hash_add(usigns, sl->signs[i]->name, (ucp)Up->uhex);
		  hash_add(sl->oid2ucode, (uccp)Up->uhex, (void*)sl->signs[i]->oid);
		  if (sl->signs[i]->aka)
		    {
		      Memo_str *msp;
		      for (msp = list_first(sl->signs[i]->aka); msp; msp = list_next(sl->signs[i]->aka))
			{
			  hash_add(usigns, msp->s, (ucp)Up->uhex);
			  /* Non-aka signs are added in mangled form
			     later on but it's complicated to add the
			     aka signs down there so we just force
			     them into the record here */
			  (void)sx_unicode_rx_mangle(sl, (ccp)msp->s, NULL);
			}
		    }
		  hash_add(uoids, (uccp)Up->uhex, (void*)sl->signs[i]->oid);
		  hash_add(unames, (uccp)Up->uhex, (ucp)Up->uname);
		  if (hash_find(ucode, (uccp)Up->uhex))
		    mesg_verr(&sl->signs[i]->inst->mloc, "uhex %s used with more than one sign\n", Up->uhex);
		  else
		    hash_add(ucode, (uccp)Up->uhex, (ucp)sl->signs[i]->name);
		}
	      else if (Up->urev && strchr(Up->urev, '.'))
		mesg_verr(&sl->signs[i]->inst->mloc, "sign %s has uname %s but no uhex\n", sl->signs[i]->name, Up->uname);
	      /* don't warn about uname but no uhex if @uage is not a Unicode age but is 0, 1, or 9 */
	    }
	  else if (Up->uhex)
	    {
	      if (!Up->upua) /* PUA doesn't require a Uni name */
		mesg_verr(&sl->signs[i]->inst->mloc, "sign %s has uhex %s but no uname\n", sl->signs[i]->name, Up->uhex);
	      if (hash_find(ucode, (uccp)Up->uhex))
		mesg_verr(&sl->signs[i]->inst->mloc, "uhex %s used with more than one sign\n", Up->uhex);
	      else
		{
		  hash_add(usigns, sl->signs[i]->name, (ucp)Up->uhex);
		  hash_add(ucode, (uccp)Up->uhex, (ucp)sl->signs[i]->name);
		  hash_add(uoids, (uccp)Up->uhex, (void*)sl->signs[i]->oid);
		}
	    }
	  else
	    hash_add(usigns, sl->signs[i]->name, "X"); /* Add components that aren't in Unicode yet as X */
	}
      else
	{
	  struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
	  if (Up->uhex)
	    {
	      hash_add(urem, (uccp)Up->uhex, (ucp)sl->signs[i]->name);
	      hash_add(unames, (uccp)Up->uhex, (ucp)Up->uname);
	    }
	}
    }

  /* Create a pattern to use for PCRE2 matching of multi-component encoded signs like U.U.U */
  int nk;
  const char **k = hash_keys2(usigns, &nk);
  qsort(k, nk, sizeof(const char *), cmp_by_len);

  List *kl = list_create(LIST_SINGLE);
  for (i = 0; i < nk; ++i)
    {
      const char *val = hash_find(usigns, (uccp)k[i]);
      if ('X' != *val)
	{
	  int multi = 0;
	  const char *m = sx_unicode_rx_mangle(sl, k[i], &multi);
	  if (multi)
	    {
	      if (list_len(kl))
		list_add(kl, "|");
	      list_add(kl, (void*)m);
	    }
	}
      else
	(void)sx_unicode_rx_mangle(sl, k[i], NULL); /* call for side effect of registering mangled form in usigns */
    }

  if (trace_mangling)
    fprintf(stderr, "###END OF MANGLE HASH BUILDING###\n");
  
  unsigned const char *pat = list_concat(kl);

  pcre2_code *comp_pat = NULL;

  if (list_len(kl))
    {

      if (trace_mangling)
	{
	  fputs("sx_unicode mangle pat = \n", stderr);
	  fputs((ccp)pat, stderr);
	  fputc('\n', stderr);
	}
      comp_pat = pcre2if_set_pattern(pat);
      if (!comp_pat)
	return;
    }
  
  /* check the compounds by building ucode sequences for them:
   *   if there is one in the signlist already, check that the newly built ucode is the same
   *   else set the sign's U.ucode appropriately
   */
  struct sl_inst *ip;
  for (ip = list_first(sl->compounds); ip; ip = list_next(sl->compounds))
    {
      if (ip->valid
	  && ((ip->type == 'f' && !ip->u.f->compoundonly)
	      || (ip->u.s->type != sx_tle_componly)))
	{
	  unsigned const char *name = ip->type == 's' ? ip->u.s->name : ip->u.f->name;
	  struct sl_unicode *Up = ip->type == 's' ? &ip->u.s->U : &ip->u.f->U;

	  last_uoid = NULL;
	  last_deep = NULL;

	  if ((useq_force || !Up->uhex) && (!Up->urev || strcmp(Up->urev, "0")))
	    {
	      int multi = 0;
	      const char *m = sx_unicode_rx_mangle(sl, (ccp)name, &multi);

#if 0
	      const char *test = "|1(DIŠ).1(BAN₂)|";
	      if (!strcmp((ccp)name, test))
		{
		  fprintf(stderr, "found useq subject %s\n", test);
		}
#endif

	      if (multi)
		{
		  /*struct sl_token *tp = hash_find(sl->htoken, name);*/
		  if (trace_mangling)
		    {
		      if (Up->useq)
			fprintf(stderr, "sx_unicode: checking @useq %s for %s mangled to %s\n", Up->useq, name, m);
		      else
			fprintf(stderr, "sx_unicode: building @useq for %s mangled to %s\n", name, m);
		    }
		  List *ml = comp_pat ? pcre2if_match(comp_pat, (PCRE2_SPTR)m, 1, sl->p, '#'): NULL;
		  if (ml)
		    {
		      List *bits = list_create(LIST_SINGLE);
		      List *bitoids = list_create(LIST_SINGLE);

		      struct pcre2if_m *mp;
		      int sofar = 0;
		      if (trace_mangling)
			fprintf(stderr, "sx_unicode: found %d match%s\n", (int)list_len(ml), list_len(ml)!=1 ? "es" : "");
		      for (mp = list_first(ml); mp; mp = list_next(ml))
			{
			  /* for each match add material that precedes the match */
			  if (mp->off > sofar)
			    {
			      list_add(bits, (void*)sx_unicode_useq_r(m, sofar, mp->off, sl->p));
			      list_add(bitoids, (void*)(last_uoid ? last_uoid : "x"));
			      sofar = mp->off;
			    }
			  /* then add the material belonging to the match */
			  list_add(bits, (void*)sx_unicode_useq_m(m, mp, sl->p));
			  list_add(bitoids, (void*)(last_uoid ? last_uoid : "x"));
			  /* mp->off + mp->len is the character after
			     the match so we need to back up by 1 so
			     we include the final sentinel of the
			     match as the initial sentinel following
			     the match */
			  sofar += (mp->len - 1);
			}
		      /* add anything that follows the last match but -1 for the final sentinel */
		      if (sofar < (strlen(m)-1))
			{
			  list_add(bits, (void*)sx_unicode_useq_r(m, sofar, strlen(m), sl->p));
			  list_add(bitoids, (void*)(last_uoid ? last_uoid : "x"));
			}
		      const char *useq = (ccp)list_join(bits, ".");
		      last_deep = (ccp)list_join(bitoids, ".");
		      if (trace_mangling)
			fprintf(stderr, "sx_unicode: %s => (via bits) %s => useq %s\n", name, m, useq);
		      if (Up->useq && *Up->useq)
			{
			  if (strcmp(Up->useq, useq))
			    {
			      mesg_verr(&ip->mloc, "%s: generated useq %s != %s\n", name, useq, Up->useq);
			      hash_add(useqs, (uccp)name, (void*)pool_copy((uccp)useq, sl->p));
			    }
			  else
			    hash_add(useqs, (uccp)name, (void*)Up->useq);
			}
		      else if (useq && *useq && !isspace(*useq))
			{
			  hash_add(useqs, (uccp)name, (ucp)(Up->useq = (ccp)pool_copy((uccp)useq, sl->p)));
			  if (adding_useq_messages)
			    mesg_vnotice(ip->mloc.file, ip->mloc.line, "%s: adding useq %s\n", (ccp)name, Up->useq);
			}
		      free((void*)useq);
		      useq = NULL;
		    }
		  else
		    {
		      const char *useq = sx_unicode_useq(m, sl->p);
		      if (trace_mangling)
			fprintf(stderr, "sx_unicode: %s => %s => useq %s\n", name, m, useq);
		      if (Up->useq)
			{
			  if (strcmp(Up->useq, useq))
			    {
			      mesg_verr(&ip->mloc, "%s: generated @useq %s != %s\n", name, useq, Up->useq);
			      hash_add(useqs, (uccp)name, (void*)pool_copy((uccp)useq, sl->p));
			    }
			  else
			    hash_add(useqs, (uccp)name, (void*)Up->useq);
			}
		      else
			{
			  if (useq && *useq && !isspace(*useq))
			    {
			      Up->useq = useq;
			      hash_add(useqs, (uccp)name, (ucp)(Up->useq = (ccp)pool_copy((uccp)useq, sl->p)));
			      if (adding_useq_messages)
				mesg_vnotice(ip->mloc.file, ip->mloc.line, "%s: adding useq %s\n", (ccp)name, Up->useq);
			    }
			}
		    }
		}
	      else
		{
		  if (trace_mangling)
		    fprintf(stderr, "sx_unicode: %s is a singleton not in Unicode\n", name);
		  hash_add(uneeds, name, "");
		}
	    }
#if 1
	  const char *sf_oid = ip->type=='s'?ip->u.s->oid:ip->u.f->oid;
	  if (sf_oid)
	    hash_add(sl->h_compoids, (uccp)sf_oid, (void*)last_deep);
	  else
	    mesg_verr(&ip->mloc, "unregistered compound %s has no OID\n",
		      ip->type=='s'?ip->u.s->name:ip->u.f->name);
#else
	  if (last_deep)
	    {
	      if (ip->type == 's')
		ip->u.s->deep = (ccp)pool_copy((uccp)last_deep, sl->p);
	      else
		ip->u.f->deep = (ccp)pool_copy((uccp)last_deep, sl->p);
	    }
#endif
	}
    }

  /* Set the utf8 fields for signs and forms as necessary */
  for (i = 0; i < sl->nsigns; ++i)
    {
      if (!sl->signs[i]->U.utf8)
	{
	  if (sl->signs[i]->U.uhex)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.uhex), sl->p);
	  else if (sl->signs[i]->U.useq)
	    {
	      sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.useq), sl->p);
	      if (hash_find(useqs, (uccp)sl->signs[i]->name))
		hash_add(utf8s, (uccp)sl->signs[i]->name, (void*)sl->signs[i]->U.utf8);
	    }
	}
    }

  for (i = 0; i < sl->nforms; ++i)
    {
      if (!sl->forms[i]->U.utf8)
	{
	  if (sl->forms[i]->U.uhex)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.uhex), sl->p);
	  else if (sl->forms[i]->U.useq)
	    {
	      sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.useq), sl->p);
	      if (hash_find(useqs, (uccp)sl->forms[i]->name))
		hash_add(utf8s, (uccp)sl->forms[i]->name, (void*)sl->forms[i]->U.utf8);
	    }
	}
    }

}

/**Prepare a sign-name for use in pattern-matching:
 *
 *   - remove enclosing |...|
 *   - map ./+/: to # if top-level; to , if within (...)
 *   - map ( and ) to < and >
 *   - adding a # at beginning and end
 *
 * The mangled string is added as a hash key to usigns--there's no
 * possibility of conflict because of the bracketing with #...# so #A#
 * is mangled to #A#.
 *
 */
static const char *
sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g, int *multi)
{
  char *res = NULL;
  int vbar = 0;
  if ('|' == *g)
    {
      res = (char*)pool_alloc(strlen(g)+1, sl->p);
      ++g;
      ++vbar;
    }
  else
    {
      res = (char*)pool_alloc(strlen(g)+3, sl->p);
    }
  const char *src = g;
  char *dst = res;
  int nest = 0;
  *dst++ = ','; 	/* place-holder because we want to check for
			   real '#' to see if there are multiple
			   elements in this sign */
  while (*src)
    {
      if ('.' == *src || '+' == *src || ':' == *src)
	{
	  if (nest)
	    *dst++ = *src++;
	  else
	    {
	      *dst++ = '#';
	      ++src;
	    }
	}
      else if ('|' == *src)
	{
	  ++src;
	  *dst = '\0';
	  break;
	}
      else if ('(' == *src)
	{
	  *dst++ = '<';
	  ++src;
	  ++nest;
	}
      else if (')' == *src)
	{
	  *dst++ = '>';
	  ++src;
	  --nest;
	}
      else
	*dst++ = *src++;
    }

  if (multi)
    {
      if (strchr(res,'#'))
	*multi = 1;
      else
	*multi = 0;
    }
  
  *res = '#';		/* fix the place-holder to be a sentinel */
  *dst++ = '#';
  *dst = '\0';

  const char *uhex = hash_find(usigns, (uccp)(g-vbar));
  if (!hash_find(usigns, (uccp)res))
    {
      if (trace_mangling)
	fprintf(stderr, "%s mangled to %s with hashval %s\n", g-vbar, res, uhex);
      hash_add(usigns, (uccp)res, (void*)uhex);
    }
  else
    {
      if (trace_mangling)
	fprintf(stderr, "refusing to rehash %s previously mangled as %s with hashval %s\n", g-vbar, res, uhex);
    }
  
  return res;
}

const char *
sx_unicode_useq(const char *m, Pool *p)
{
  unsigned char *m2 = (ucp)strdup(m), *s;
  unsigned char save = '\0';
  char *u = NULL;
  List *uoidl = list_create(LIST_SINGLE);
  int nseg = 0;
  for (s = m2; *s; ++s)
    if ('#' == *s)
      ++nseg;
  u = (char*)pool_alloc(nseg*8, p);
  *u = '\0';
  for (s = m2; *s;)
    {
      /* s should always point at opening '#'; set e to char after and look for closing '#' */
      unsigned char *e = s+1;

      if (!*e)
	break;

      while (*e && '#' != *e) /* should always find closing '#' */
	++e;
      if ('#' == *e)
	{
	  ++e;
	  if (*e)
	    save = *e;
	  *e = '\0';
	}
      else
	{
	  abort();
	}

      const unsigned char *x = NULL;
      /* X in the sign-name is mapped to O in the dotted-hex */
      if (!strcmp((ccp)s, "#X#"))
	{
	  x = (uccp)"O";
	  list_add(uoidl, (void*)x);
	}
      else
	{
	  x = hash_find(usigns, s);
	  if (!x)
	    {
	      if ('<' == s[1] && '>' == s[strlen((ccp)s)-2])
		{
		  char ss[strlen((ccp)s)];
		  *ss = '#';
		  strcpy(ss+1,(ccp)s+2);
		  ss[strlen(ss)-2] = '#';
		  ss[strlen(ss)-1] = '\0';
		  char *h = ss+1;
		  while (*h)
		    {
		      if ('.' == *h)
			*h = '#';
		      ++h;
		    }
		  if ((x = hash_find(usigns, (uccp)ss)))
		    s = pool_copy((uccp)ss, p);
		}
	    }
	  if (!x)
	    {
	      fprintf(stderr, "sx_unicode: element %s not found in usigns while processing mangled %s\n", s, m);
	      x = (uccp)"X";
	      list_add(uoidl, (void*)x);
	    }
	  else
	    {
	      if (trace_mangling)
		fprintf(stderr, "sx_unicode: element %s => %s\n", s, x);
	      const char *u = hash_find(uoids, x);
	      if (u)
		list_add(uoidl, (void*)u);
	      else
		list_add(uoidl, "x");
	    }
	}
      /* append x to u */
      if ('O' == *x)
	{
	  if (strlen(u))
	    strcat(u, ".O");
	  else
	    strcpy(u, "O");
	}
      else if ('X' == *x)
	{
	  if (strlen(u))
	    strcat(u, ".X");
	  else
	    strcpy(u, "X");
	}
      else
	{
	  if (strlen(u))
	    strcat(u, ".x");
	  else
	    strcpy(u, "x");
	  strcat(u, (const char *)x+2);
	}
      
      if (save)
	{
	  *e = save;
	  save = '\0';
	}
      s = &e[-1];
    }
  free(m2);

  last_deep = (ccp)pool_copy(list_join(uoidl, "."), p);
  list_free(uoidl, NULL);

  return u;
}

/* Process the sign belonging to a match */
static const char *
sx_unicode_useq_m(const char *m, struct pcre2if_m *mp, Pool *p)
{  
  return sx_unicode_useq_r(m, mp->off, mp->off + mp->len, p);
}

/* Process the signs that fall within a range of the subject string
   that is outside of the matches */
static const char *
sx_unicode_useq_r(const char *m, int from, int to, Pool *p)
{
  /* excise the range-string from the subject */
  char *tmp = NULL; 
  int off = to - from;

  tmp = strdup(m+from);
  
  if (off && to < strlen(m) && tmp[off-1] != '#')
    ++off;	/* extend range by 1 to include a sentinel # if necessary */

  tmp[off] = '\0';

  /* return the results of processing it */
  char *res = NULL;

  /* avoid putting a matched multipart sign through the useq parser again */
  if ((res = hash_find(usigns, (uccp)tmp)) && strcmp(res, "X"))
    {
      /* we have, e.g., U+12301 -- return x12301 */
#if 0
      last_uoid = hash_find(uoids, (void*)res);
#endif
      res = (char*)pool_copy((uccp)res+1, p);
      *res = 'x';
    }
  else
    res = (char*)sx_unicode_useq(tmp, p);

  free(tmp);
  return res;
}

#if 0
void
sx_unicode_compoids(FILE *f, struct sl_signlist *sl)
{
  const char **k = NULL;
  int nkeys = 0;
  k = hash_keys2(sl->h_compoids, &nkeys);
  qsort(k, nkeys, sizeof(char*), cmpstringp);
  int i;
  for (i = 0; i < nkeys; ++i)
    {
      const char *v = hash_find(sl->h_compoids, (uccp)k[i]);
      if (v)
	fprintf(f, "%s\t%s\n", k[i], v);
    }
}
#endif

void
sx_unicode_table(FILE *f, struct sl_signlist *sl)
{
  const char **u, *x;
  int nu;
  int i;

  u = hash_keys2(ucode, &nu);
  qsort(u, nu, sizeof(const char *), cmpstringp);
  for (i = 0; u[i]; ++i)
    {
      uccp sn = hash_find(ucode, (uccp)u[i]);
      struct sl_sign *sp = hash_find(sl->hsentry, sn);
      if (!sp->oid)
	{
	  if (sp->xref)
	    sp->oid = sp->xref->oid;
	  if (!sp->oid)
	    fprintf(stderr, "no code oid\n");
	}
      fprintf(f, "code\t%s\t%s\t%s\t%s\t%d\n", u[i], sn,
	      sp->oid, (char*)hash_find(unames, (uccp)u[i]), sp->sort);
    }

  u = hash_keys2(urem, &nu);
  qsort(u, nu, sizeof(const char *), cmpstringp);
  for (i = 0; u[i]; ++i)
    {
      if (!hash_find(ucode, (uccp)u[i]))
	fprintf(f, "depr\t%s\t%s\t%s\t\n", u[i], (char*)hash_find(urem, (uccp)u[i]), (char*)hash_find(unames, (uccp)u[i]));
      else
	{
	  struct sl_sign *sp = hash_find(sl->hsentry, hash_find(urem, (uccp)u[i]));
	  mesg_verr(&sp->inst->mloc, "Unicode value %s is in a valid sign and also an invalid one", u[i]);
	}
    }

  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)"U+");
  if (ldp)
    {
      /* sort the list's entries if necessary */
      if (!ldp->sorted++)
	sx_listdefs_sort(ldp);
      int i;
      for (i = 0; i < ldp->nnames; ++i)
	if (!hash_find(ldp->seen, (uccp)ldp->names[i]) && ldp->names[i][2]) /* ignore U+ */
	  fprintf(f,"miss\t%s\t\t\t\n", ldp->names[i]);
    }
  else
    mesg_verr(&sl->mloc, "can't find U+ list while making unicode data table");

  List *xseq = list_create(LIST_SINGLE);
  u = hash_keys2(useqs, &nu);
  qsort(u, nu, sizeof(const char *), cmpstringp);
  
  for (i = 0; u[i]; ++i)
    {
      x = hash_find(useqs, (uccp)u[i]);
      if (strchr(x, 'X'))
	list_add(xseq, (void*)u[i]);
      else
	{
	  struct sl_sign *sp = hash_find(sl->hsentry, (uccp)u[i]);
	  if (!sp->oid)
	    {
	      if (sp->xref)
		sp->oid = sp->xref->oid;
	      if (!sp->oid)
		fprintf(stderr, "no useq oid\n");
	    }
	  const char *coid = "";
	  if (sp->oid)
	    coid = hash_find(sl->h_compoids, (uccp)sp->oid);

	  fprintf(f, "useq\t%s\t%s\t%s\t%s\t%d\n", x, u[i], sp->oid, coid, sp->sort);
	}
    }

  for (x = list_first(xseq); x; x = list_next(xseq))
    fprintf(f, "xseq\t%s\t%s\t\n", x, (char*)hash_find(useqs, (uccp)x));

  list_free(xseq, NULL);
  
  u = hash_keys2(uneeds, &nu);
  qsort(u, nu, sizeof(const char *), via_tok_cmp);
  for (i = 0; u[i]; ++i)
    fprintf(f, "need\t%s\n", u[i]);
}

void
sx_unicode_p(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      if (sl->signs[i]->inst->valid)
	{
	  struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
	  const char *oid = (ccp)(sl->signs[i]->xref ? sl->signs[i]->xref->oid : sl->signs[i]->oid);
	  if (!Up->utf8)
	    {
	      Up->utf8 = hash_find(parent_sl, sll_tmp_key((uccp)oid, "uchar"));
#if 0
	      if (gvl_script_type)
		{
		  int nivs = 0;
		  unsigned const char *ren = NULL;
		  unsigned const char *ivs = gvl_ivs(Up->utf8, &nivs);
		  if (nivs)
		    {
		      Up->ivs = ivs;
		      /* Up->ident = ... ; Up->ren = ... ? */
		    }
		  else
		    Up->ivs = Up->utf8;
		}
#endif
	      if (Up->utf8)
		{
		  const char *ucode = hash_find(parent_sl, sll_tmp_key((uccp)oid, "ucode"));
		  if (ucode)
		    {
		      if ('U' == *ucode)
			Up->uhex = ucode;
		      else
			Up->useq = ucode;
		    }
		}
	    }
	}
    }
}
