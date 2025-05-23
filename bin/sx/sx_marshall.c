#include <hash.h>
#include <list.h>
#include <collate.h>
#include <oraccsys.h>
#include <sll.h>
#include <oid.h>
#include <signlist.h>
#include <sx.h>

Hash *oids;
Hash *oid_sort_keys;
Hash *oid_warned = NULL;
static struct sl_signlist *cmpsl = NULL;

int sx_show_tokens = 0;

static void
add_componly_key(struct sl_signlist *sl, struct sl_inst *ip)
{
  char buf[20];
  if (ip->type == 'f')
    sprintf(buf, "%s.%s.", ip->parent_s->u.s->oid, ip->u.f->oid);
  else
    sprintf(buf, "%s..", ip->u.s->oid);
  ip->key = pool_copy((uccp)buf, sl->p);
}

int oid_char_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  if (cc1 && cc2)
    {
      int a1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)cc1);
      int b1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)cc2);
      if (a1 < b1)
	return -1;
      else if (a1 > b1)
	return 1;
      else
	return 0;
    }
  return 0;
}

static const char **
sx_oid_array(struct sl_sign *s, List *o)
{
  const char **oids = NULL;
  int noids = 0;
  if (s)
    ++noids;
  if (o)
    noids += list_len(o);
  if (noids == 0)
    return NULL;
  else if (noids == 1)
    {
      oids = malloc(2*sizeof(const char *));
      if (s)
	{
	  oids[0] = s->oid;
	}
      else
	{
	  struct sl_inst *ip = list_first(o);
	  oids[0] = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
	  oids[1] = NULL;
	}
    }
  else
    {
      Hash *seen = hash_create(16);
      struct sl_inst *ip = NULL;
      int i = 0;
      oids = calloc(noids+1, sizeof(const char *));
      if (s)
	{
	  hash_add(seen, (uccp)s->oid, "");
	  oids[i++] = s->oid;
	}
      for (ip = list_first(o); ip; ip = list_next(o))
	{
	  const char *oid = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
	  if (oid)
	    {
	      if (!hash_find(seen, (uccp)oid))
		{
		  hash_add(seen, (uccp)oid, "");
		  oids[i++] = oid;
		}
	    }
	  else
	    {
	      unsigned const char *sname = NULL;
	      sname = (ip->type == 's' ? ip->u.s->name : ip->u.f->name);
	      if (!hash_find(oid_warned, sname))
		mesg_verr(&ip->mloc, "strange ... no OID for sign or form %s\n", sname);
	    }
	}
      noids = i;
      oids[noids] = NULL;
      qsort(oids, noids, sizeof(const char *), (cmp_fnc_t)oid_char_cmp);
      hash_free(seen, NULL);
    }
  return oids;
}

static int forms_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_form**)a)->sort;
  int b1 = (*(struct sl_form**)b)->sort;  
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int forms_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.f->sort;
  int b1 = (*(struct sl_inst**)b)->u.f->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

/* fowners is a list of inst where the insts can be either sign or form insts */
static int fowners_cmp(const void *a, const void *b)
{
  struct sl_inst *ai = (*(struct sl_inst**)a);
  struct sl_inst *bi = (*(struct sl_inst**)b);
  if (ai && bi)
    {
      int a1 = ('f'==ai->type) ? ai->u.f->sort : ai->u.s->sort;
      int b1 = ('f'==bi->type) ? bi->u.f->sort : bi->u.s->sort;
      if (a1 < b1)
	return -1;
      else if (a1 > b1)
	return 1;
      else
	return 0;
    }
  else if (ai)
    return bi ? 0 : -1;
  else
    return 1;
}

/* form->owners is a list of sl_sign* */
static int owners_cmp(const void *a, const void *b)
{
  struct sl_sign *ai = (*(struct sl_sign**)a);
  struct sl_sign *bi = (*(struct sl_sign**)b);
  int a1 = ai->sort;
  int b1 = bi->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int lists_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_list**)a)->sort;
  int b1 = (*(struct sl_list**)b)->sort;  
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int lists_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.l->sort;
  int b1 = (*(struct sl_inst**)b)->u.l->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int values_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.v->sort;
  int b1 = (*(struct sl_inst**)b)->u.v->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int signs_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_sign**)a)->sort;
  int b1 = (*(struct sl_sign**)b)->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int signs_inst_cmp(const void *a, const void *b)
{
  int a1 = ((*(struct sl_inst**)a)->type == 's') ? (*(struct sl_inst**)a)->u.s->sort : (*(struct sl_inst**)a)->u.f->sort;
  int b1 = ((*(struct sl_inst**)b)->type == 's') ? (*(struct sl_inst**)b)->u.s->sort : (*(struct sl_inst**)b)->u.f->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int toks_cmp(const void *a, const void *b)
{
#if 1
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  struct sl_token *t1 = hash_find(cmpsl->htoken, (uccp)cc1);
  struct sl_token *t2 = hash_find(cmpsl->htoken, (uccp)cc2);
  return gsort_cmp(&t1->gsh, &t2->gsh);
#else
  unsigned const char *t_a = ((struct sl_token*)a)->t;
  unsigned const char *t_b = ((struct sl_token*)b)->t;
  /* This cast is a hack; collate_cmp_graphemes expects its args to be unsigned char ** */
  return collate_cmp_graphemes((ucp)&t_a, (ucp)&t_b);
#endif
}

static int values_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_value**)a)->sort;
  int b1 = (*(struct sl_value**)b)->sort;  
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

int via_tok_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  struct sl_token *t1 = hash_find(cmpsl->htoken, (uccp)cc1);
  struct sl_token *t2 = hash_find(cmpsl->htoken, (uccp)cc2);
  int a1 = t1->s;
  int b1 = t2->s;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

/* NOTE: oidindexes stores index+1 so that 0 return from hash_find means not-in-hash */
static void
sx_oid_indexes(struct sl_signlist *sl)
{
  int nk;
  const char **k = hash_keys2(oid_sort_keys, &nk);
  qsort(k, nk, sizeof(char*), oid_char_cmp);
  sl->oidindexes = hash_create(1024);
  int i;
  for (i = 0; k[i]; ++i)
    hash_add(sl->oidindexes, (uccp)k[i], (void*)(uintptr_t)(i+1));
}

/* Return a string consisting of the OIDs for each of the NAMES
   in the arg string */
unsigned char *
sx_oids_of(struct sl_signlist *sl, unsigned const char *snames)
{
  List *l = list_create(LIST_SINGLE);
  unsigned char *xsnames = (ucp)pool_copy((uccp)snames,sl->p), *xsname, *x, *ret;
  x = xsnames;
  while (*x)
    {
      xsname = x;
      while (*x && ' ' != *x)
	++x;
      if (*x)
	*x++ = '\0';
      struct sl_sign *s = hash_find(sl->hsentry, xsname);
      if (s)
	list_add(l,(void*)s->oid);
    }
  ret = list_join(l, " ");
  list_free(l,NULL);
  return ret;
}

static List *
sx_uniq_aka(List *aka)
{
  if (list_len(aka) > 1)
    {
      Hash *h = hash_create(5);
      List *l = list_create(LIST_SINGLE);
      Memo_str *a;
      for (a = list_first(aka); a; a = list_next(aka))
	if (!hash_find(h, a->s))
	  {
	    list_add(l, a);
	    hash_add(h, a->s, "");
	  }
      hash_free(h,NULL);
      list_free(aka,NULL);
      return l;
    }
  else
    return aka;
}

static int
sx_form_default(List *lip)
{
  int n = 0;
  struct sl_inst *ip, *default_ip = NULL;
  for (ip = list_first(lip); ip; ip = list_next(lip))
    if (ip->deflt)
      {
	if (!default_ip)
	  default_ip = ip;
	++n;
      }
  if (n > 1)
    mesg_verr(&default_ip->mloc, "FORM %s has %d defaults\n", default_ip->u.f->name, n);

  return n;
}

void
sx_marshall(struct sl_signlist *sl)
{
  const char**keys = NULL;
  int nkeys;
  List *smap_list = list_create(LIST_SINGLE);

  const char**lets = NULL;
  int nlets = 0, i;
  collate_init((ucp)"unicode");

  if (!sl->domain)
    sl->domain = "sl";
  
  if (!sl->oid2ucode)
    sl->oid2ucode = hash_create(2048);
  
  oids = oid_domain_hash(NULL, "oid", sl->domain);
  if (!oids)
    oids = hash_create(1);
  oid_sort_keys = hash_create(2048);
  oid_warned = hash_create(64);

  if (idata_file)
    sx_idata_init(sl, idata_file, idata_type);
  else if (kdata_file)
    sx_kdata_init(sl, kdata_file, idata_type);

  if (ldata_file)
    sx_ldata_init(sl, ldata_file);

  /* Register forms that don't exist as signs as if they were signs */
  keys = hash_keys2(sl->hfentry, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_sign *s;
      struct sl_form *f = hash_find(sl->hfentry, (uccp)keys[i]);

      if (f->aka)
	f->aka = sx_uniq_aka(f->aka);
      
      if (!(s = hash_find(sl->hsentry, (uccp)keys[i])))
	{
	  struct sl_form *fp = hash_find(sl->hfentry, (uccp)keys[i]);
	  int n = (fp->insts ? list_len(fp->insts) : 0);
#if 0
	  fprintf(stderr, "FORM %s is not also a SIGN with %d insts\n", keys[i], n);
#else
	  if (n > 1 && !sx_form_default(fp->insts))
	    {
	      struct sl_inst *ip = list_first(fp->insts);
	      mesg_verr(&ip->mloc, "non-sign FORM %s has %d insts: use @sign or @form+\n", keys[i], n);
	    }
#if 0
	  /* This is not practical; over 1000 @form are not also @sign; implement default with @form+ instead */
	  else
	    {
	      mesg_verr(&ip->mloc, "FORM %s is not also a SIGN; this will soon be an error\n", keys[i]);
	      mesg_status_ignore_one();
	    }
#endif
#endif
	  s = asl_form_as_sign(sl, fp);
	}
      else
	{	  
	  if (f->aka)
	    {
	      Memo_str *ms;
	      for (ms = list_first(f->aka); ms; ms = list_next(f->aka))
		mesg_verr(&ms->m, "@aka is only allowed on @form if the form does not also exist as a @sign\n");
	      list_free(f->aka, NULL);
	      f->aka = NULL;
	    }
	}
      f->sign = s;

      /* Side effect: provide signs which occur as forms a list of instances where they occur */
      if (!s->as_form)
	s->as_form = list_create(LIST_SINGLE);
      list_add_list(s->as_form, f->insts);

#if 0
      /* Unicode normalization is now done in sx_unicode_U_data and
	 it's better to ensure that the formdef is always the first
	 instance */
      /* If this form has Unicode info register it as the defining instance */
      if (f->sign->xref && (f->U.useq || f->U.uname))
	{
	  struct sl_inst *ip;
	  for (ip = list_first(f->insts); ip; ip = list_next(f->insts))
	    {
	      if (ip->upua || ip->utf8  || ip->uname)
		{
		  f->sign->formdef = ip;
		  break;
		}
	    }
	}
#endif      
    }

  /* Force all forms that are not also signs to have a defining instance */
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_form *f = hash_find(sl->hfentry, (uccp)keys[i]);
      if (f->sign->xref && !f->sign->formdef)
	f->sign->formdef = list_first(f->insts);
    }

  /* First compounds phase: gather the data for digesting later */
  sx_compounds(sl);
  
  /* Sort the tokens and set the token sort codes */
  cmpsl = sl;
  keys = hash_keys2(sl->htoken, &nkeys);
  if (nkeys > 1)
    qsort(keys, nkeys, sizeof(char*), (cmp_fnc_t)toks_cmp);
  sl->tokens = malloc((nkeys+1) * sizeof(struct sl_token *));
  if (sx_show_tokens)
    fprintf(stderr, "===sorted tokens===\n");
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = hash_find(sl->htoken, (ucp)keys[i]);
      /* tp->gdl is a g:w node that wraps the grapheme; the first
	 grapheme node is tp->gdl->kids */
      if (tp->gdl && tp->gdl->kids && !strcmp(tp->gdl->kids->name, "g:n"))
	tp->s = i+1;
      else
	tp->s = i+1;
      sl->tokens[i] = tp;
      if (sx_show_tokens)
	{
	  fprintf(stderr, "%d\t", tp->s);
	  gsort_show(tp->gsh);
	}
    }
  sl->tokens[i] = NULL;
  if (sx_show_tokens)
    fprintf(stderr, "===end sorted tokens===\n");

  /* Provide signs with sort codes base on token sort sequence; add oids and uchar while we are at it */
  keys = hash_keys2(sl->hsentry, &nkeys);
  sl->signs = malloc(sizeof(struct sl_sign*) * nkeys);
  sl->nsigns = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      int j;
      sl->signs[i] = hash_find(sl->hsentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, sl->signs[i]->name);
      sl->signs[i]->sort = tp->s;

      if (sl->signs[i]->smap)
	list_add(smap_list, sl->signs[i]);

      if (!sl->signs[i]->xref)
	{
	  if (!(sl->signs[i]->oid = hash_find(oids, sl->signs[i]->name)))
	    {
	      if (sl->signs[i]->aka)
		{
		  Memo_str *a;
		  for (a = list_first(sl->signs[i]->aka); a; a = list_next(sl->signs[i]->aka))
		    if ((sl->signs[i]->oid = hash_find(oids, a->s)))
		      break;
		}
	      if (!sl->signs[i]->oid)
		{
		  if (!parent_sl
		      || !(sl->signs[i]->oid = (ccp)hash_find(parent_sl, sl->signs[i]->name)))
		    {
		      /* Signs that use @smap still need an OID for the parent sign */
		      if (sl->signs[i]->inst->valid && sl->signs[i]->type != sx_tle_lref
			  && sl->signs[i]->type != sx_tle_sref /*&& !sl->signs[i]->smap*/)
			{
			  mesg_verr(&sl->signs[i]->inst->mloc, "OID needed:\n%s\t%s\tsign\t\t",
				    sl->domain, sl->signs[i]->name);
			  hash_add(oid_warned, sl->signs[i]->name, "");
			}
		    }
		}
	      else if (sl->signs[i]->U.uhex)
		hash_add(sl->oid2ucode, (uccp)sl->signs[i]->oid, (ucp)sl->signs[i]->U.uhex);
	    }
	  if (sl->signs[i]->oid)
	    {
	      hash_add(oid_sort_keys, (uccp)sl->signs[i]->oid, (void*)(uintptr_t)sl->signs[i]->sort);
	      if (!strcmp(sl->domain, "pc") && sl->signs[i]->inst->atoid)
		sl->signs[i]->oid = sl->signs[i]->inst->atoid;
	      else if (sl->signs[i]->inst->atoid && strcmp(sl->signs[i]->oid, sl->signs[i]->inst->atoid))
		mesg_verr(&sl->signs[i]->inst->mloc,
			  "@oid %s for sign %s does not match OID tab's %s\n",
			  sl->signs[i]->inst->atoid,
			  sl->signs[i]->name,
			  sl->signs[i]->oid);
	    }
	}

#if 0
      if (!sl->signs[i]->U.uchar && sl->signs[i]->U.ucode)
	sl->signs[i]->U.uchar = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.ucode), sl->p);
#endif
      
      if (sl->signs[i]->hventry)
	{
	  const char **vkeys = hash_keys2(sl->signs[i]->hventry, &sl->signs[i]->nvalues);
	  sl->signs[i]->values = memo_new_array(sl->m_insts_p, sl->signs[i]->nvalues);
	  for (j = 0; j < sl->signs[i]->nvalues; ++j)
	    sl->signs[i]->values[j] = hash_find(sl->signs[i]->hventry, (uccp)vkeys[j]);
	}
    }
  /* Sort the signs */
  qsort(sl->signs, sl->nsigns, sizeof(struct sl_sign*), (cmp_fnc_t)signs_cmp);

  /* Ensure that every sign has full Unicode data and cross-check
     duplicating data in multiple form instances (done here so
     sl->signs/nsigns are set */
  sx_unicode_U_data(sl);

  /* Provide forms with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hfentry, &nkeys);  
  sl->forms = malloc(sizeof(struct sl_form*) * nkeys);
  sl->nforms = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->forms[i] = hash_find(sl->hfentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->forms[i]->sort = tp->s;
      if (!(sl->forms[i]->oid = hash_find(oids, sl->forms[i]->name)))
	{
	  if (sl->forms[i]->aka)
	    {
	      Memo_str *a;
	      for (a = list_first(sl->forms[i]->aka); a; a = list_next(sl->forms[i]->aka))
		if ((sl->forms[i]->oid = hash_find(oids, a->s)))
		  break;
	    }
	  
	  if (!sl->forms[i]->oid && parent_sl)
	    sl->forms[i]->oid = (ccp)hash_find(parent_sl, sl->forms[i]->name);

	  if (sl->forms[i]->oid)
	    {
	      if (sl->forms[i]->sign->xref)
		sl->forms[i]->sign->oid = sl->forms[i]->oid;
	      if (sl->forms[i]->U.uhex)
		hash_add(sl->oid2ucode, (uccp)sl->forms[i]->oid, (ucp)sl->forms[i]->U.uhex);
	    }
	  else
	    {
	      struct sl_inst *inst = list_first(sl->forms[i]->insts);
	      if (inst->valid)
		{
		  mesg_verr(&inst->mloc, "OID needed:\n%s\t%s\tsign\t\t",
			    sl->domain, sl->forms[i]->name);
		  hash_add(oid_warned, sl->forms[i]->name, "");
		}
	    }
	}

      if (sl->forms[i]->oid && !hash_find(oid_sort_keys, (uccp)sl->forms[i]->oid))
	hash_add(oid_sort_keys, (uccp)sl->forms[i]->oid, (void*)(uintptr_t)sl->forms[i]->sort);

      /* While we are iterating over forms, check to see if a form
	 that has Unicode info also has a sign that is not a
	 back-reference; if so we want the Unicode info on the sign,
	 not the form (or the sign could be deleted from the
	 signlist) */
      struct sl_sign *sp = hash_find(sl->hsentry, sl->forms[i]->name);
      if (sp && !sp->xref)
	{
	  if (sl->forms[i]->U.uhex || sl->forms[i]->U.uname)
	    {
	      if (sp->U.uhex || sp->U.uname)
		; /*mesg_verr(&sp->inst->mloc, "Unicode data should only be here; also found on @form %s", sp->name);*/
	      else
		mesg_verr(&sp->inst->mloc, "move Unicode data from @form %s to here or delete @sign %s", sp->name, sp->name);
	    }
	  else
	    ;/*fprintf(stderr, "form %s is also a sign\n", sl->forms[i]->name);*/
	}
#if 0
      if (!sl->forms[i]->U.uchar && sl->forms[i]->U.ucode)
	sl->forms[i]->U.uchar = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.ucode), sl->p);
#endif

#if 0
      /* set the key for all form insts */
      struct sl_inst *ip;
      for (ip = list_first(sl->forms[i]->insts); ip; ip = list_next(sl->forms[i]->insts))
	asl_add_key(&ip->mloc, sl, ip, ip->parent_s->atoid, ip->u.f->oid, NULL);
#endif
    }
  /* Sort the forms */
  qsort(sl->forms, sl->nforms, sizeof(struct sl_form*), (cmp_fnc_t)forms_cmp);

  /* add inherited values to forms: timing of this routine is important--don't move it! */
  sx_inherited(sl);

 /* Sort form owners (sl_sign*) if there are any */
  for (i = 0; i < sl->nforms; ++i)
    {
      if (sl->forms[i]->owners)
	{
	  sl->forms[i]->nowners = list_len(sl->forms[i]->owners);
	  if (sl->forms[i]->nowners == 1)
	    {
	      sl->forms[i]->owners_sort = memo_new(sl->m_signs_p);
	      sl->forms[i]->owners_sort[0] = list_first(sl->forms[i]->owners);
	    }
	  else
	    {
	      struct sl_sign *sp;
	      int j = 0;
	      sl->forms[i]->owners_sort = memo_new_array(sl->m_signs_p, sl->forms[i]->nowners);
	      for (sp = list_first(sl->forms[i]->owners); sp; sp = list_next(sl->forms[i]->owners))
		sl->forms[i]->owners_sort[j++] = sp;
	      qsort(sl->forms[i]->owners_sort, sl->forms[i]->nowners, sizeof(struct sl_sign *), (cmp_fnc_t)owners_cmp);
	    }
	}
    }

  /* Provide lists with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hlentry, &nkeys);  
  sl->lists = malloc(sizeof(struct sl_list*) * nkeys);
  sl->nlists = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->lists[i] = hash_find(sl->hlentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->lists[i]->sort = tp->s;
    }
  /* Sort the lists */
  qsort(sl->lists, sl->nlists, sizeof(struct sl_list*), (cmp_fnc_t)lists_cmp);

  /* Convert lv->hventry to lv->values */
  for (i = 0; i < sl->nforms; ++i)
    {
      if (sl->forms[i]->insts)
	{
	  struct sl_inst *ip;
	  for (ip = list_first(sl->forms[i]->insts); ip; ip = list_next(sl->forms[i]->insts))
	    {
	      if (ip->lv && ip->lv->hventry)
		{
		  int j, k;
		  const char **vkeys = hash_keys2(ip->lv->hventry, &ip->lv->nvalues);
#if 0
		  if (ip->lv->nvalues > 1)
		    qsort(vkeys, ip->lv->nvalues, sizeof(char*), (cmp_fnc_t)via_tok_cmp);
#endif
		  ip->lv->values = memo_new_array(sl->m_insts_p, ip->lv->nvalues);
		  for (j = k = 0; j < ip->lv->nvalues; ++j)
		    {
		      ip->lv->values[k] = hash_find(ip->lv->hventry, (uccp)vkeys[j]);
		      /* ignore NULL entries which can occur when a value is a duplicate */
		      if (ip->lv->values[k])
			++k;
		    }
		  if (k < ip->lv->nvalues)
		    ip->lv->nvalues = k;
		}
	    }
	}
    }
  /*cmpsl = NULL;*/

  /* Create oid arrays for the list entries; used in sx_sll output */
  for (i = 0; i < sl->nlists; ++i)
    sl->lists[i]->oids = sx_oid_array(NULL, sl->lists[i]->insts);

  /* Provide values with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hventry, &nkeys);  
  sl->values = malloc(sizeof(struct sl_value*) * nkeys);
  sl->nvalues = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->values[i] = hash_find(sl->hventry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->values[i]->sort = tp->s;
      /* Sort fowners if there are any */
      if (sl->values[i]->fowners)
	{
	  sl->values[i]->nfowners = list_len(sl->values[i]->fowners);
	  if (sl->values[i]->nfowners == 1)
	    {
	      sl->values[i]->fowners_i_sort = memo_new(sl->m_insts_p);
	      sl->values[i]->fowners_i_sort[0] = list_first(sl->values[i]->fowners);
	    }
	  else
	    {
	      struct sl_inst *ip;
	      int j = 0;
	      sl->values[i]->fowners_i_sort = memo_new_array(sl->m_insts_p, sl->values[i]->nfowners);
	      for (ip = list_first(sl->values[i]->fowners); ip; ip = list_next(sl->values[i]->fowners))
		{
		  if ('s' == ip->type || 'f' == ip->type)
		    sl->values[i]->fowners_i_sort[j++] = ip;
		  else
		    fprintf(stderr, "[asl] internal error: fowner should have type 's' or 'f'\n");
		}
	      assert(j == sl->values[i]->nfowners);
	      qsort(sl->values[i]->fowners_i_sort, sl->values[i]->nfowners, sizeof(struct sl_inst *), (cmp_fnc_t)fowners_cmp);
	    }
	}

    }
  /* Sort the values */
  qsort(sl->values, sl->nvalues, sizeof(struct sl_value*), (cmp_fnc_t)values_cmp);

#if 0
  /* NOT SURE THIS IS USEFUL */
  /* Create oid arrays for the value entries */
  for (i = 0; i < sl->nvalues; ++i)
    sl->values[i]->oids = sx_oid_array(sl->values[i]->sowner, sl->values[i]->fowners);
#endif
  
  /* Second phase of compounds: invert Hashes of sl_compound
     structures into arrays of OIDs for each category */
  sx_compound_digests(sl);
  
  /* Dereference structures created in asl_bld.c--see that file for AB1/AB2/AB3 creation */
  lets = hash_keys2(sl->hletters, &nlets); /* obtain list of letters from AB1 */
  qsort(lets, nlets, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->letters = memo_new_array(sl->m_letters, nlets);
  sl->nletters = nlets;

  /* 0[LN]XXX sort to start so we manually move them to the end */
  if (nlets && *lets[0] == '0')
    {
      int nzero = 0;
      while (nzero < nlets && *lets[nzero] == '0')
	++nzero;
      const char **tmp = malloc(nzero*sizeof(const char*));
      memmove(tmp, lets, nzero*sizeof(const char *));
      memmove(lets, &lets[nzero], (nlets-nzero)*sizeof(const char *));
      memmove(&lets[nlets-nzero], tmp, nzero*sizeof(const char *));
      free(tmp);
    }

  for (i = 0; i < nlets; ++i)
    {
      const char **grps = NULL;
      int ngrps = 0, j;
      struct sl_letter *letterp = hash_find(sl->hletters, (uccp)lets[i]);
      if (letterp)
	{
	  sl->letters[i] = *letterp;
	  grps = hash_keys2(sl->letters[i].hgroups, &ngrps); /* obtain list of groups in letter from AB2 */
	  qsort(grps, ngrps, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
	  sl->letters[i].groups = memo_new_array(sl->m_groups, ngrps);
	  sl->letters[i].ngroups = ngrps;
	  for (j = 0; j < ngrps; ++j)
	    {
	      List *slist = hash_find(sl->letters[i].hgroups, (ucp)grps[j]); /* obtain list of signs in group from AB3 */
	      sl->letters[i].groups[j].name = (ucp)grps[j];
	      sl->letters[i].groups[j].nsigns = list_len(slist);
	      sl->letters[i].groups[j].signs = memo_new_array(sl->m_signs_p,
							      sl->letters[i].groups[j].nsigns);
	      sl->letters[i].groups[j].signs = (struct sl_inst **)list2array(slist);
	      qsort(sl->letters[i].groups[j].signs,
		    sl->letters[i].groups[j].nsigns, sizeof(void*), (cmp_fnc_t)signs_inst_cmp);
	    }
	}
    }

  /* Marshall sign/form values by OID suitable for output in TSV */
  sx_values_by_oid(sl);

  /* Resolve merges to OIDs; this is the merge processing that is
     executed on the output of a subsetted sl */
  sx_merge(sl);

  /* Provide keys for @compoundonly entries */
  struct sl_inst *ip;
  for (ip = list_first(sl->componly); ip; ip = list_next(sl->componly))
    add_componly_key(sl, ip);
  
  /* Sort the lists, values and forms for each sign */
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *sp = sl->signs[i];
      if ('s' == sp->inst->type)
	sx_idata_sign(sl, sp);
      if ('s' == sp->inst->type)
	sx_ldata_sign_inst(sl, sp->inst);
      if (sp->hlentry)
	{
	  int nslsts, j;
	  const char **slsts = hash_keys2(sp->hlentry, &nslsts);
	  sp->lists = memo_new_array(sl->m_insts, nslsts);
	  sp->nlists = nslsts;
	  for (j = 0; j < sp->nlists; ++j)
	    sp->lists[j] = hash_find(sp->hlentry, (uccp)slsts[j]);
	  qsort(sp->lists, sp->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
	}
      if (sp->hventry)
	{
	  int nsvals, j;
	  const char **svals = hash_keys2(sp->hventry, &nsvals);
	  sp->values = memo_new_array(sl->m_insts, nsvals);
	  sp->nvalues = nsvals;
	  for (j = 0; j < sp->nvalues; ++j)
	    {
	      sp->values[j] = hash_find(sp->hventry, (uccp)svals[j]);
	      sx_idata_value_inst(sl, sp->values[j]);
	      sx_ldata_value_inst(sl, sp->values[j]);
	    }
	  qsort(sp->values, sp->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
	}
      if (sp->hfentry)
	{
	  int nsfrms, j;
	  const char **sfrms = hash_keys2(sp->hfentry, &nsfrms);
	  sp->forms = memo_new_array(sl->m_insts, nsfrms);
	  sp->nforms = nsfrms;
	  for (j = 0; j < sp->nforms; ++j)
	    {
	      struct sl_inst *fp;
	      fp = sp->forms[j] = hash_find(sp->hfentry, (uccp)sfrms[j]);
	      sx_idata_form_inst(sl, fp);
	      if (fp->lv && fp->lv->hlentry)
		{
		  int nslsts, m;
		  const char **slsts = hash_keys2(fp->lv->hlentry, &nslsts);
		  fp->lv->lists = memo_new_array(sl->m_insts, nslsts);
		  fp->lv->nlists = nslsts;
		  for (m = 0; m < fp->lv->nlists; ++m)
		    fp->lv->lists[m] = hash_find(fp->lv->hlentry, (uccp)slsts[m]);
		  qsort(fp->lv->lists, fp->lv->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
		}
	      if (fp->lv && fp->lv->hventry)
		{
		  int nsvals, k, l;
		  const char **svals = hash_keys2(fp->lv->hventry, &nsvals);
		  fp->lv->values = memo_new_array(sl->m_insts, nsvals);
		  fp->lv->nvalues = nsvals;
		  for (k = l = 0; k < fp->lv->nvalues; ++k)
		    if (svals[k])
		      {
			if (!(fp->lv->values[l++] = hash_find(fp->lv->hventry, (uccp)svals[k])))
			  --l;
		      }
		    else
		      fprintf(stderr, "skipping NULL entry in %s::%s hventry\n",
			      sp->forms[j]->u.f->sign->name, sp->forms[j]->u.f->name);
		  fp->lv->nvalues = l;
		  qsort(fp->lv->values, fp->lv->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
		  for (k = 0; fp->lv->values[k]; ++k)
		    {
		      sx_idata_value_inst(sl, fp->lv->values[k]);
		      sx_ldata_value_inst(sl, fp->lv->values[k]);
		    }
		}
	    }
	  qsort(sp->forms, sp->nforms, sizeof(void*), (cmp_fnc_t)forms_inst_cmp);
	}
    }

  /* now that forms have oids and s->forms is set, resolve any smaps to oids */
  struct sl_sign *s;
  for (s = list_first(smap_list); s; s = list_next(smap_list))
    {
      struct sl_form *smapf = NULL;
      int i;
      for (i = 0; i < s->nforms; ++i)
	if (!strcmp(s->smap, (ccp)s->forms[i]->u.f->name))
	  {
	    smapf = s->forms[i]->u.f;
	    break;
	  }
      if (smapf)
	s->smoid = smapf->oid;
      else
	mesg_verr(&s->inst->mloc, "%s in @smap does not point to a child-form of @sign %s", s->smap, s->name);
    }
  list_free(smap_list, NULL);

  sx_oid_indexes(sl);
  
  sx_values_parents(sl);

  sx_homophones(sl);

  sx_qualified(sl);

#if 1
  sx_unicode(sl);
#else
  /* deprecated because unicode_from_parent was a hack to work around
     deficiencies in the old tok2asl subsetting */
  if (!unicode_from_parent)
    sx_unicode(sl);
  else
    sx_unicode_p(sl);  
#endif

  sx_kdata_useq(sl);  

  /* This is the merge processing that is done when creating a subsl */
  if (sxconfig.merge)
    sx_merge_subsl(sl);
  
  sx_images(sl);

  sx_idata_ctotals(sl);
  
  /*collate_term();*/
}
