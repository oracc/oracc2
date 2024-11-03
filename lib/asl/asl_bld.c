#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>
#include <sll.h>
#include <gdl.h>
#include <oid.h>
#include <asl.tab.h>
#include "signlist.h"

extern int asl_literal_flag;

static void check_flags(Mloc* locp, char *n, int *q, int *l);
static void asl_register_list_item(Mloc *locp, struct sl_signlist *sl, struct sl_list *l);

struct sl_signlist *curr_asl = NULL;

static struct sl_inst *
new_inst(struct sl_signlist *sl)
{
  struct sl_inst *ip = memo_new(sl->m_insts);
  char iid[8];
  sprintf(iid,"i%06d",++sl->ninsts);
  ip->iid = (ccp)pool_copy((ucp)iid,sl->p);
  return ip;
}

static Hash *oids;
static unsigned const char *asl_oid_lookup(unsigned const char *key)
{
  return hash_find(oids, key);
}

unsigned const char *
asl_make_key(Mloc *locp, struct sl_signlist *sl, 
	     const char *isp, const char *ifp, const char *ivp)
{
  const char *s = isp ? (ccp)isp : "";
  const char *f = ifp ? (ccp)ifp : "";
  const char *v = ivp ? (ccp)ivp : "";
  if (*s)
    {
      char buf[strlen(s)+strlen(f)+strlen(v)+3];
      sprintf(buf, "%s.%s.%s", s, f, v);
      if (hash_find(sl->hkeys, (uccp)buf))
	mesg_verr(locp, "duplicate sign/form/value %s", buf);
      else
	return pool_copy((ucp)buf, sl->p);
    }
  return NULL;
}

void
asl_add_key(Mloc *locp, struct sl_signlist *sl, struct sl_inst *hval,
	    const char *s, const char *f, const char *v)
{
  unsigned const char *k = asl_make_key(locp, sl, s, f, v);
  if (k)
    hash_add(sl->hkeys, (hval->key = k), hval);
}

Hash *
asl_get_oids(void)
{
  return oids;
}

static int
asl_sign_guard(Mloc *locp, struct sl_signlist *sl, const char *tag)
{
  if (!sl->curr_sign)
    {
      mesg_verr(locp, "misplaced @%s--no @sign in effect", tag);
      return 0;
    }
  return 1;
}

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = calloc(1, sizeof(struct sl_signlist));
  sl->htoken = hash_create(4192);
  sl->hkeys = hash_create(4192);
  sl->linkdefs = hash_create(3);
  sl->listdefs = hash_create(7);
  sl->sysdefs = hash_create(3);
  sl->hsentry = hash_create(2048);
  sl->hfentry = hash_create(1024);
  sl->haka = hash_create(128);
  sl->hventry = hash_create(2048);
  sl->hlentry = hash_create(1024);
  sl->hsignvvalid = hash_create(1024);
  sl->hletters = hash_create(32);
  sl->h_merge = hash_create(64);
  sl->h_scripts = hash_create(4);
  sl->h_compoids = hash_create(1024);
  sl->m_tokens = memo_init(sizeof(struct sl_token), 1024);
  sl->m_letters = memo_init(sizeof(struct sl_letter), 32);
  sl->m_groups = memo_init(sizeof(struct sl_letter), 128);
  sl->m_signs = memo_init(sizeof(struct sl_sign),512);
  sl->m_signs_p = memo_init(sizeof(struct sl_sign*),512);
  sl->m_forms = memo_init(sizeof(struct sl_form),512);
  sl->m_lists = memo_init(sizeof(struct sl_value),256);
  sl->m_values = memo_init(sizeof(struct sl_value),1024);
  sl->m_insts = memo_init(sizeof(struct sl_inst),1024);
  sl->m_insts_p = memo_init(sizeof(struct sl_inst*),512);
  sl->m_lv_data = memo_init(sizeof(struct sl_lv_data),512);
  sl->m_split_v = memo_init(sizeof(struct sl_split_value),512);
  sl->m_compounds = memo_init(sizeof(struct sl_compound), 512);
  sl->m_digests = memo_init(sizeof(struct sl_compound_digest), 512);
  sl->m_parents = memo_init(sizeof(struct sl_parents), 1024);
  sl->m_notes = memo_init(sizeof(struct sl_note), 512);
  sl->m_memostr = memo_init(sizeof(Memo_str), 512);
  sl->m_syss = memo_init(sizeof(struct sl_sys), 512);
  sl->m_scriptdefs = memo_init(sizeof(struct sl_scriptdef), 8);
  sl->m_scriptdata = memo_init(sizeof(struct sl_scriptdata), 128);
  sl->m_links = memo_init(sizeof(Link), 512);
  sl->m_ligas = memo_init(sizeof(Link), 8);
  sl->p = pool_init();
  sl->compounds = list_create(LIST_SINGLE);
  sl->componly = list_create(LIST_SINGLE);
  sl->hnums = hash_create(1000);

  sl->notes = new_inst(sl);
  sl->notes->type = 'S';
  sl->notes->u.S = sl;

  /* Preload the current set of SL oids */
  oids = oid_domain_hash(NULL, "oid", "sl");

  (void)gvl_setup(NULL, NULL, NULL);
  gvl_set_lookup_ptr(asl_oid_lookup);
  
  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->htoken, NULL);
      hash_free(sl->listdefs, free);
      hash_free(sl->sysdefs, free);
      hash_free(sl->hsentry, NULL);
      hash_free(sl->hfentry, NULL);
      hash_free(sl->haka, NULL);
      hash_free(sl->hventry, NULL);
      hash_free(sl->hlentry, NULL);
      hash_free(sl->hsignvvalid, NULL);
      hash_free(sl->hletters, NULL);
      memo_term(sl->m_tokens);
      memo_term(sl->m_letters);
      memo_term(sl->m_groups);
      memo_term(sl->m_signs);
      memo_term(sl->m_signs_p);
      memo_term(sl->m_forms);
      memo_term(sl->m_lists);
      memo_term(sl->m_values);
      memo_term(sl->m_insts);
      memo_term(sl->m_insts_p);
      memo_term(sl->m_lv_data);
      memo_term(sl->m_split_v);
      memo_term(sl->m_compounds);
      memo_term(sl->m_digests);
      memo_term(sl->m_parents);
      memo_term(sl->m_notes);
      memo_term(sl->m_memostr);
      memo_term(sl->m_syss);
      memo_term(sl->m_links);
      pool_term(sl->p);
      free(sl);
    }
}

static void
asl_bld_list_string(const unsigned char *t, List **lp)
{
  if (!*lp)
    *lp = list_create(LIST_SINGLE);
  list_add(*lp, (void*)t);
}

static void
asl_bld_singleton_string(Mloc *locp, const unsigned char *t, const char *tag, unsigned char const* *dest, Boolean *seen)
{
  if (*seen)
    mesg_verr(locp, "tag @%s can only be used once in a @sign or @form", tag);
  else
    {
      *dest = t;
      *seen = 1;
    }
}

static Tree *
asl_bld_gdl(Mloc *locp, unsigned char *s)
{
  Tree *tp = NULL;
  mesg_init();
  tp = gdlparse_string(locp, (char*)s);
  gdlparse_reset();
  return tp;
}

static uccp
reset_num_n(struct sl_signlist *sl, const unsigned char *n, const unsigned char *asif)
{
  unsigned char buf[strlen((ccp)n)+strlen((ccp)asif)];
  const unsigned char *t = n;
  unsigned char *b = buf;
  while ('(' != *t)
    *b++ = *t++;
  *b++ = *t++;
  strcpy((char*)b, (ccp)asif);
  b += strlen((ccp)b);
  t = (uccp)strrchr((ccp)n, ')');
  while (*t)
    *b++ = *t++;
  *b = '\0';
  return pool_copy((uccp)buf, sl->p);
#if 0
  struct sl_token *tokp = asl_bld_token(NULL, sl, n, 0);
  if (asl_bld_num(NULL, sl, n, tokp, 3))
    {
      /* This is strange but true; we re-reset the text of the reset
	 tokp to be the original value. We do this so the value maps
	 to an OID in sx_num_data */
      tokp->t = tp->t;
      tokp->s = tp->s;
      return tokp;
    }
  else
    return NULL;
#endif
}

int
asl_bld_num(Mloc *locp, struct sl_signlist *sl, const uchar *n, struct sl_token *tokp, int priority)
{
  struct sl_token *htp = hash_find(sl->hnums, n);
  if (!htp && !gvl_looks_like_sname(n))
    htp = hash_find(sl->hnums, g_uc(n));
  if (!htp || htp->priority > priority)
    {
      tokp->priority = priority;
      if (tokp->gdl && tokp->gdl->kids && !strcmp(tokp->gdl->kids->name, "g:n"))
	{
	  if (tokp->gdl->kids &&  tokp->gdl->kids->kids && tokp->gdl->kids->kids->next)
	    {
	      const uchar *set_n;
	      struct node *setnode = tokp->gdl->kids->kids->next;
	      if (!strcmp(setnode->name, "g:c"))
		set_n = (uccp)setnode->kids->text;
	      else
		set_n = (uccp)setnode->text;
	      
	      /* if a set is in the numvmap ensure that the mapped
		 name is a token and register the mapped version in
		 hnums not the original version */	      
	      struct numvmap_tab *np = numvmap((ccp)set_n, strlen((ccp)set_n));
	      if (np)
		{
		  /* rewrite the incoming name */
		  n = reset_num_n(sl, n, (uccp)np->asif);
		  tokp = asl_bld_token(locp, sl, (ucp)n, 0);
		  tokp->priority = priority;
		  setnode = tokp->gdl->kids->kids->next;
		  if (!strcmp(setnode->name, "g:c"))
		    set_n = (uccp)setnode->kids->text;
		  else
		    set_n = (uccp)setnode->text;
		}

	      hash_add(sl->hnums, n, tokp);
	      
	      /* ensure that future set names are tokens */
	      if (!strcmp(setnode->name, "g:c"))
		{
		  struct sl_token *stp = asl_bld_token(locp, sl, pool_copy(g_uc(set_n), sl->p), 0);
		  tokp->gsig = stp->gsig; /* rewrite the num tok so the gsig is the compound head oid */
		}
	      else
		{
		  (void)asl_bld_token(locp, sl, pool_copy(g_uc(set_n), sl->p), 0);
		}
	      return 1;
	    }
	}
    }
  return 0;
}

/* All of the list entries in @listdef are processed here as literals;
 * this means that when outputting <sl:name> we need to use
 * tokp->gdl->text if tokp->gdl->kids is NULL.
 *
 * We could reparse literals if they occur in another context, but it
 * should be safe to use the literal because listdef entries have a
 * limited syntax.
 *
 * 2024-09-18: overload 'literal': if it is -1 it means we are adding
 * a homophone base, can't be a number.
 *
 */
struct sl_token *
asl_bld_token(Mloc *locp, struct sl_signlist *sl, unsigned char *t, int literal)
{
  struct sl_token *tokp = NULL;
  
  if (!(tokp = hash_find(sl->htoken, t)))
    {
      Tree *tp;
      const char *gsig = NULL, *deep = NULL;
      extern int asl_literal_flag;
      tokp = memo_new(sl->m_tokens);
      tokp->t = t;
      if (literal > 0 || asl_literal_flag)
	tp = gdl_literal(locp, (char*)t);
      else
	{
	  extern int gdlsig_depth_mode;
	  tp = asl_bld_gdl(locp, t);
	  /* deep? or a special mode for unicode rendering? */
	  gdlsig_depth_mode = -1;
	  gsig = gdlsig(tp);
	  gdlsig_depth_mode = 1;
	  deep = gdlsig(tp);
	  if (deep)
	    {
	      if (!strchr(deep, '.'))
		{
		  free((void*)deep);
		  deep = NULL;
		}
	    }		
	}
      tokp->priority = 100;
      tokp->gdl = tp->root;
      tokp->gdl->name = "g:w";
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "form", tokp->gdl->text);
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", "sux");
      tokp->gsh = gsort_prep(tp);
      tokp->gsig = gsig;
      tokp->deep = deep;
      tokp->oid_ip = sl->curr_inst;
      hash_add(sl->htoken, t, tokp);
    }
  asl_literal_flag = 0;
  return tokp;
}

/* This routine builds the signlist tree of letter/group/signs */
void
asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s)
{
  struct sl_letter *lp;
  struct sl_token *tokp;
  unsigned char *group;
  int number_group = 0;
  static int nth_num_letter = 0;

  s->sl = sl;
  
  if ('|' == *s->name && s->inst->valid)
    list_add(sl->compounds, (void*)s->inst);

#if 0
  tp = asl_bld_gdl(locp, (char*)pool_copy(s->name,sl->p));
  s->gdl = tp->root;
#endif

  /* get the group sign */
  tokp = hash_find(sl->htoken, s->name);
  if ((group = pool_copy(gdl_first_s(tokp->gdl), sl->p)))
    {
      /* get the letter from the group sign */
      unsigned char *letter = NULL;
      List *gslist; /* group signs */
      int code = -1;
#if 0
      unsigned char *group_orig = group;
#endif
      char *at = NULL;
      unsigned char *slist = (ucp)sll_is_signlist((ccp)group);

      if (!isdigit(*group))
	{
	  if ((at = strchr((ccp)group, '@')))
	    *at = '\0';
	  else if ((at = strchr((ccp)group, '/')))
	    *at = '\0';
	  else if ('O' == *group)
	    group[1] = '\0';
	}
      
      group = pool_copy(g_base_of_preserve_case(group), sl->p);
 
      if (isdigit(*group))
	{
	  unsigned char *n = group;
	  while (*n && '(' != *n)
	    ++n;
	  if (*n)
	    {
	      unsigned char *p = group = pool_copy(n, sl->p);
	      *p = '0';
	      while (*p && ')' != *p)
		++p;
	      if (*p)
		*p = '\0';
	      letter = group;
	      code = -1;
	      number_group = 1;
	    }
	  else
	    {
	      letter = pool_alloc(2, sl->p);
	      letter[0] = '0';
	      letter[1] = '\0';
	      code = 0; /* generates a letter code 'l0000' */
	    }
	  if (!slist)
	    {
	      if (!number_group)
		{
		  if (group[1] && isdigit(group[1]))
		    group[2] = '\0';
		  else
		    group[1] = '\0';
		}
	    }
	}
      else if (*group < 128)
	{
	  letter = pool_alloc(2, sl->p);
	  letter[0] = *group;
	  letter[1] = '\0';
	  code = (int)letter[0];
	  if (group[1] && group[1] < 128)
	    group[2] = '\0';
	  else
	    {
	      wchar_t *w = NULL;
	      unsigned char *c = NULL;
	      size_t len = 0;
	      w = utf2wcs(group, &len);
	      if (len > 1)
		{
		  w[2] = L'\0';
		  c = wcs2utf(w, 2);
		  group = pool_copy(c,sl->p);
		}
	      else
		group = letter;
	    }
	}
      else
	{
	  wchar_t *w = NULL;
	  unsigned char *c = NULL;
	  size_t len = 0;
	  w = utf2wcs(group, &len);
	  code = (int)w[0];
	  if (len == 1)
	    {
	      c = wcs2utf(w, 1);
	      group = letter = pool_copy(c,sl->p);
	    }
	  else
	    {
	      wchar_t z = w[1];
	      w[1] = L'\0';
	      c = wcs2utf(w, 1);
	      letter = pool_copy(c,sl->p);
	      w[1] = z;
	      w[2] = L'\0';
	      c = wcs2utf(w, 2);
	      group = pool_copy(c,sl->p);
	    }	  
	}

      if (slist)
	group = slist;
      else
	{
	  if (number_group)
	    {
	      
	    }
	  else
	    {
	      if (isdigit(group[0]))
		group = (ucp)"0";
	      else if (!strchr("AEIU", group[0]) && 'N' == group[1])
		{
		  group = (ucp)"NN";
		  letter = (ucp)"N";
		}
	    }
	}

#if 0
      fprintf(stderr, "[asl] letter %s group %s -> %s\n", letter, group_orig, group);
#endif

      /* This is where the structure of the signlist is built */

      /* Recompute "letter" based on N01 style numbering */
      if (code == -1)
	{
	  char *dest, *src, *tmp;
	  int llen = strlen((ccp)letter);
	  tmp = (char*)pool_alloc(llen+1, sl->p);
	  *tmp = 'l';
	  strcat(tmp, (ccp)letter+1);
	  src = tmp;
	  if ((dest = strchr(src, '~')))
	    *dest = '\0';
	  dest = src;
	  while (*src)
	    if ('@' == *src)
	      ++src;
	    else
	      *dest++ = *src++;
	  *dest = '\0';
	  letter = (ucp)tmp;
	  group = (ucp)tmp;
	}

      char *lname = NULL;
      if (code == -1)
	{
	  lname = (char*)pool_copy(letter, sl->p);
	  *letter = '0';
	  *group = '0';
	}

      /* remember the letter */	  
      if (!(lp = hash_find(sl->hletters, letter)))/* AB1: hash of letters in signlist;
						     value is struct sl_letter* */
	{
	  char id[32];	  
	  lp = memo_new(sl->m_letters);

	  hash_add(sl->hletters, letter, lp);


	  lp->name = letter;
	  lp->lname = lname;
	  lp->code = code;
	  if (lp->code < 0)
	    sprintf(id, "ln%04d", ++nth_num_letter);
	  else
	    sprintf(id, "l%04d", lp->code);
	  lp->xmlid = (ccp)pool_copy((uccp)id, sl->p);
	  lp->hgroups = hash_create(32);
	}
      else if (code == -1)
	*group = '0';
      
      /* remember the group belongs to the letter */
      if (!(gslist = hash_find(lp->hgroups, group)))	      
	hash_add(lp->hgroups, group,
		 (gslist = list_create(LIST_SINGLE)));  /* AB2: hash of groups in letter;
							   value is list of struct sl_sign * */
      list_add(gslist, s->inst);			/* AB3: list of signs in group,
					   		   data member is struct sl_inst* */ /* WHAT ABOUT @comp ? */

      /* set the sign's pointer to it's letter data */
      s->letter = lp;
    }
  else
    mesg_verr(locp, "no sign name found in GDL of %s", s->name);
}

struct sl_sign *
asl_form_as_sign(struct sl_signlist *sl, struct sl_form *f)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  struct sl_inst *ip = list_first(f->insts);

  s->inst = ip;
  s->name = f->name;
  s->xref = f;
  hash_add(sl->hsentry, s->name, s);
  asl_register_sign(&ip->mloc, sl, s);
  return s;
}

void
asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  int literal, query;
  
  if (asl_sign_guard(locp, sl, "form"))
    {

      if (minus_flag)
	{
	  if (minus_flag > 0)
	    {
	      if (!sl->curr_invalid || sl->curr_invalid == 3)
		sl->curr_invalid = 2;
	    }
	}
      else if (sl->curr_invalid == 2)
	sl->curr_invalid = 0;

      sl->curr_value = NULL;
      check_flags(locp, (char*)n, &query, &literal);

      struct sl_token *tp = asl_bld_token(locp, sl, (ucp)n, 0);
      if (!sl->curr_invalid)
	asl_bld_num(locp, sl, (ucp)n, tp, 2);

      if (sl->curr_sign->hfentry && hash_find(sl->curr_sign->hfentry, n))
	{
	  mesg_verr(locp, "duplicate form %s in sign %s\n", n, sl->curr_sign->name);
	}
      else
	{
	  struct sl_form *f = NULL;
	  struct sl_inst *i = new_inst(sl);

	  if (!(f = hash_find(sl->hfentry, n)))
	    {
	      f = memo_new(sl->m_forms);
	      f->name = n;
	      f->deep = tp->deep;
	      f->insts = list_create(LIST_SINGLE);
	      list_add(f->insts, i);
	      f->owners = list_create(LIST_SINGLE);
	      list_add(f->owners, sl->curr_sign);	/* list of signs that have this n as a form */
	      hash_add(sl->hfentry, (uccp)f->name, f); 	/* The forms that belong to the signlist are sl_form* */
	    }
	  else
	    {
	      list_add(f->owners, sl->curr_sign);    	/* list of signs that have this n as a form */	  
	      list_add(f->insts, i);
	    }

	  sl->curr_form = sl->curr_inst = i;

	  i->parent_s = sl->curr_sign->inst;
	  i->type = 'f';
	  i->u.f = f;
	  i->mloc = *locp;
	  i->valid = (Boolean)(minus_flag > 0 ? 0 : 1);
	  i->deflt = (Boolean)(minus_flag < 0 ? 1 : 0);
	  i->query = (Boolean)query;
	  i->literal = literal;
	  i->lv = memo_new(sl->m_lv_data);

	  if (!sl->curr_sign->hfentry)
	    sl->curr_sign->hfentry = hash_create(128);

	  hash_add(sl->curr_sign->hfentry, f->name, i); /* The forms that belong to signs are sl_inst* */
	}
    }
}

void
asl_bld_end_form(Mloc *locp, struct sl_signlist *sl)
{
  if (asl_sign_guard(locp, sl, "form"))
    {
      if (!sl->curr_form)
	mesg_verr(locp, "misplaced @@\n");
      else if (!sl->curr_form->key)
	mesg_verr(locp, "no key set for form %s", sl->curr_form->u.f->name);
      sl->curr_form = sl->curr_inst = NULL;
    }
}

static void
asl_register_list_item(Mloc *locp, struct sl_signlist *sl, struct sl_list *l)
{
  /* Check that the @list is valid against the signlists's listdefs
     and register it as seen */
  char name[32];
  if (strlen((ccp)l->name) < 32)
    {
      strcpy(name, (ccp)l->name);
      char *end = strpbrk(name, "0123456789");
      if (end)
	{
	  struct sl_listdef *ldp = NULL;
	  *end = '\0';
	  if ('U' == name[0] && '+' == name[1])
	    name[2] = '\0';
	  if ((ldp = hash_find(sl->listdefs, (uccp)name)))
	    {
	      if (!(hash_find(ldp->known, l->name)))
		{
		  mesg_verr(locp, "%s is not in @listdef %s", l->name+strlen(name), name);
		}
	      else
		{
		  if (!(hash_find(ldp->seen, l->name)))
		    hash_add(ldp->seen, l->name, l);
		}
	    }
	  else if (!sll_signlist(name,strlen(name)))
	    mesg_verr(locp, "@list %s has unknown list-name part %s", l->name, name);
	}
      else
	{
	  mesg_verr(locp, "@list %s has no digits", l->name);
	  return;
	}
    }
  else
    {
      mesg_verr(locp, "@list entry too long (max 31 characters)");
      return;
    }
}

/* Check for list/lref conflict; if there is no conflict return 0; if there is a conflict return 1
 */
 
static int
asl_list_lref_guard(Mloc *locp, struct sl_signlist *sl, unsigned const char *n, enum sl_ll_type type)
{
  struct sl_list *lp = hash_find(sl->hlentry, n);
  if (lp)
    {
      if ((lp->type == sl_ll_list && sl_ll_lref == type)
	  || (type == sl_ll_list && sl_ll_lref == lp->type))
	{
	  mesg_verr(locp, "%s can't be @lref and @list; append letters to disambiguate if necessary", n);
	  return 1;
	}
      else if (lp->type == sl_ll_lref && type == sl_ll_lref)
	{
	  mesg_verr(locp, "duplicate @lref %s (first occurrence at line %d", n, lp->inst->mloc.line);
	  return 1;
	}
    }
  return 0;
}
static void
asl_add_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int lit, int q, int m)
{
  struct sl_list *l = NULL;
  struct sl_inst *i = new_inst(sl);
  Hash *h = NULL;
  
  i->mloc = *locp;
  i->type = 'l';
  i->valid = (Boolean)!m;
  i->literal = (Boolean)lit;
  i->query = (Boolean)q;
  /*sl->curr_inst = i;*/

  if (asl_list_lref_guard(locp, sl, n, sl_ll_list))
    return;

  /* If this list is already in the lists hash for the sign or the form-instance it's an error */
  if (sl->curr_form)
    {
      if (!sl->curr_form->lv->hlentry)
	h = sl->curr_form->lv->hlentry = hash_create(1);
    }
  else
    h = sl->curr_sign->hlentry;

  /* After setting h to an hlentry hash we can check if we've seen a
     valid @list with this name in the appropriate context */
  if (h && (l = hash_find(h, n)))
    {
      const char *f_or_s = (sl->curr_form ? "form" : "sign");
      const char *f_or_s_name = (sl->curr_form ? (ccp)sl->curr_form->u.f->name : (ccp)sl->curr_sign->name);
      mesg_verr(locp, "duplicate @list %s within @%s %s\n", n, f_or_s, f_or_s_name);
    }
  else
    {
      /* Now check the signlist's registry of @lists and add the new
	 sl_list* or add the inst to the existing sl_list* */
      struct sl_inst *parent_inst = (sl->curr_form ? sl->curr_form : sl->curr_sign->inst);
      if ((l = hash_find(sl->hlentry, n)))
	{
	  list_add(l->insts, parent_inst);
	}
      else
	{
	  l = memo_new(sl->m_lists);
	  l->name = n;
	  l->insts = list_create(LIST_SINGLE);
	  l->type = sl_ll_list;
	  list_add(l->insts, parent_inst);
	  hash_add(sl->hlentry, n, l);
	}
    }

  asl_register_list_item(locp, sl, l);
  
  i->u.l = l;

  if (sl->curr_form)
    {
      if (!sl->curr_form->lv->hlentry)
	sl->curr_form->lv->hlentry = hash_create(1);
      hash_add(sl->curr_form->lv->hlentry, l->name, i);
      /*list_add(l->insts, sl->curr_form);*/
    }
  else
    {
      if (!sl->curr_sign->hlentry)
	sl->curr_sign->hlentry = hash_create(1);
      hash_add(sl->curr_sign->hlentry, l->name, i);
      /*list_add(l->insts, sl->curr_sign->inst);*/
    }
}

void
asl_bld_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  if (asl_sign_guard(locp, sl, "list"))
    {
      int literal, query = 0;

      check_flags(locp, (char*)n, &query, &literal);
  
      (void)asl_bld_token(locp, sl, (ucp)n, 1);
  
      if (sl->curr_form)
	asl_add_list(locp, sl, n, literal, query, minus_flag);
      else
	asl_add_list(locp, sl, n, literal, query, minus_flag);

      /* U+ list entries are both lists and uhex; they are specialcased on
	 output and emitted only as @list U+ but within the Unicode
	 block */
      if ('U' == n[0] && '+' == n[1])
	asl_bld_uhex(locp, sl, n);
    }
}

void
asl_bld_linkdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment)
{
  if (!hash_find(sl->linkdefs, (uccp)name))
    {
      struct sl_linkdef *sdp = calloc(1, sizeof(struct sl_linkdef));
      sdp->name = pool_copy((uccp)name, sl->p);
      sdp->comment = (ccp)pool_copy((uccp)comment, sl->p);
      sdp->inst.type = 'L';
      sdp->inst.u.L = sdp;
      sl->curr_inst = &sdp->inst;
      hash_add(sl->linkdefs, sdp->name, sdp);
    }
  else
    {
      mesg_verr(locp, "repeated @sysdef %s\n", name);
    }
}

void
asl_bld_sysdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment)
{
  if (!hash_find(sl->sysdefs, (uccp)name))
    {
      struct sl_sysdef *sdp = calloc(1, sizeof(struct sl_sysdef));
      sdp->name = pool_copy((uccp)name, sl->p);
      sdp->comment = (ccp)pool_copy((uccp)comment, sl->p);
      sdp->inst.type = 'y';
      sdp->inst.u.y = sdp;
      sl->curr_inst = &sdp->inst;
      hash_add(sl->sysdefs, sdp->name, sdp);
    }
  else
    {
      mesg_verr(locp, "repeated @sysdef %s\n", name);
    }
}

#if 0
static void
asl_check_sys(Mloc *locp, struct sl_signlist *sl, const char *txt)
{
  /* NAME VALUE (=> VALUES)? */
  char *c = strdup(txt), *s, *t;
  enum sysstate { sys_name, sys_value, sys_goesto, sys_values };
  enum sysstate currstate = sys_name;
  s = c;
  while ((t = strtok(s," \t\n")))
    {
      if (s)
	s = NULL;
      if (currstate == sys_name)
	{
	  if (!hash_find(sl->sysdefs, (uccp)t))
	    {
	      mesg_verr(locp, "undefined system name %s in @sys", t);
	      return;
	    }
	  ++currstate;
	}
      else if (currstate == sys_value || currstate == sys_values)
	{
	  if (!hash_find(sl->hventry, (uccp)t))
	    {
	      mesg_verr(locp, "undefined value %s in @sys", t);
	      return;
	    }
	  if (currstate == sys_value)
	    ++currstate;
	}
      else if (currstate == sys_goesto)
	{
	  if (strcmp(t, "=>"))
	    {
	      mesg_verr(locp, "unexpected token in @sys %s; expected '=>'", t);
	      return;
	    }
	  ++currstate;
	}
      else
	{
	  abort();
	}
    }
  
}
#endif

void
asl_bld_link(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv)
{
  struct sl_inst *ip = NULL;
  if (sl->curr_form)
    ip = sl->curr_form;
  else if (sl->curr_sign)
    ip = sl->curr_sign->inst;
  else
    mesg_verr(locp, "misplaced @link: must belong to @sign or @form");
  if (ip)
    {
      char *xsysname = (char*)pool_copy((uccp)sysname, sl->p);
      if (!hash_find(sl->linkdefs, (uccp)xsysname))
	{
	  mesg_verr(locp, "undefined link name '%s' in @link", xsysname);
	  return;
	}
      if (!ip->links)
	{
	  ip->links = list_create(LIST_SINGLE);
	  if (!sl->linklists)
	    sl->linklists = list_create(LIST_SINGLE);
	  list_add(sl->linklists, ip->links);
	}
      struct sl_link *sp = memo_new(sl->m_syss);
      sp->name = xsysname;
      sp->label = v;
      while (*v && !isspace(*v))
	++v;
      if (*v)
	{
	  /* This is safe because v actually points to pool_copy space */
	  char*vv = (char*)v;
	  *vv++ = '\0';
	  while (isspace(*vv))
	    ++vv;
	  sp->url = (uccp)vv;
	  sp->ip = ip;
	  list_add(ip->links, sp);
	}
      else
	mesg_verr(locp, "@link text `%s' should have a label and a url", xsysname);
    }
}

void
asl_bld_note(Mloc *locp, struct sl_signlist *sl, const char *tag, const char *txt)
{
  if (sl->curr_inst)
    {
      struct sl_note *n = memo_new(sl->m_notes);
      n->tag = tag; /* tag is a constant from asl.y */
      n->txt = txt;
      if (!sl->curr_inst->notes)
	sl->curr_inst->notes = list_create(LIST_SINGLE);
      list_add(sl->curr_inst->notes, n);
#if 0
      /* @sys is implemented as a type of note but has special
	 location and syntax rules which we check here rather than in
	 the lexer/parser */
      if (!strcmp(tag,"sys"))
	{
	  if (sl->curr_inst->type != 'v')
	    mesg_verr(locp, "misplaced @sys -- must occur in the notes block belonging to an @v");
	  else
	    asl_check_sys(locp, sl, txt);
	}
#endif
    }
  else
    (void)asl_sign_guard(locp, sl, tag);
}

void
asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  static int one = 1;
  Memo_str *m = NULL;

  check_flags(locp, (char*)t, &query, &literal);

  if (query)
    mesg_verr(locp, "'?' is ignored on @aka");

  (void)asl_bld_token(locp, sl, (ucp)t, literal);

  if (sl->curr_form)
    {
      if (!sl->curr_form->u.f->aka)
	sl->curr_form->u.f->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_form->u.f->aka, (m = memo_str(locp, t)));
      if (literal)
	m->user = &one;
      else
	m->user = NULL;
      /* If @aka is on a @form check that the form doesn't have a
	 parent sign; if it does, the @aka should be up there */
      if (sl->curr_form->u.f->sign)
	{
	  if (!sl->curr_form->u.f->sign->xref)
	    {
	      mesg_verr(locp, "@aka %s should be on @sign %s, not @form", t, sl->curr_form->u.f->sign->name);
	    }
	  else
	    {
	      if (!hash_find(sl->haka, t))
		hash_add(sl->haka, t, sl->curr_sign->inst);
	      else
		mesg_verr(locp, "duplicate @aka %s", t);
	    }
	}
      else
	{
	  if (!hash_find(sl->haka, t))
	    hash_add(sl->haka, t, sl->curr_form);
	  else
	    mesg_verr(locp, "duplicate @aka %s", t);
	}
    }
  else if (sl->curr_sign)
    {
      if (!sl->curr_sign->aka)
	sl->curr_sign->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_sign->aka, (m = memo_str(locp, t)));
      if (literal)
	m->user = &one;
      else
	m->user = NULL;
      if (!hash_find(sl->haka, t))
	hash_add(sl->haka, t, (void*)sl->curr_sign->inst);
      else
	mesg_verr(locp, "duplicate @aka %s", t);
    }
  else
    (void)asl_sign_guard(locp, sl, "aka");
}

void
asl_bld_smap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags(locp, (char*)t, &query, &literal);
  if (literal)
    mesg_verr(locp, "'*' is ignored on @smap");
  if (query)
    mesg_verr(locp, "'?' is ignored on @smap");
  
  (void)asl_bld_token(locp, sl, (ucp)t, 0);

  if (sl->curr_form)
    {
      mesg_verr(locp, "@smap %s should be on @sign %s, not @form", t, sl->curr_sign->name);
    }
  else if (sl->curr_sign)
    {
      if (!sl->curr_sign->smap)
	sl->curr_sign->smap = (ccp)t;
      else
	mesg_verr(locp, "duplicate @smap %s", t);
    }
  else
    (void)asl_sign_guard(locp, sl, "smap");
}

void
asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags(locp, (char*)t, &query, &literal);

  if (literal)
    mesg_verr(locp, "'*' is ignored on @aka");
  if (query)
    mesg_verr(locp, "'?' is ignored on @pname");
    
  (void)asl_bld_token(locp, sl, (ucp)t, 0);

  if (sl->curr_form)
    {
      if (sl->curr_form->u.f->pname)
	mesg_verr(locp, "@pname can only be given once for each @form");
      else
	sl->curr_form->u.f->pname = pool_copy(t, sl->p);
    }
  else if (sl->curr_sign)
    {
      if (sl->curr_sign->pname)
	mesg_verr(locp, "@pname can only be given once for each @sign");
      else
	sl->curr_sign->pname = pool_copy(t, sl->p);
    }
  else
    (void)asl_sign_guard(locp, sl, "pname");
}

/* Top-level entities other than sign set sl->curr_inst to host
   metadata but NULL-out sl->curr-sign because it's an error for them
   to have values */
void
asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *m, enum sx_tle type)
{
  if (type == sx_tle_componly && sl->curr_sign)
    {
      /* We need to build a form here which has a sign->xref backref,
	 like a form that doesn't also occur as a sign */
      struct sl_form *f = NULL;
      asl_bld_form(locp, sl, n, 0);
      f = hash_find(sl->hfentry, n);
      f->sign = asl_form_as_sign(sl, f);
      f->sign->type = sx_tle_formproxy;
      f->compoundonly = 1;
      list_add(sl->componly, sl->curr_inst);
    }
  else
    {
      asl_bld_sign(locp, sl, n, 0);
      sl->curr_sign->type = type;
      sl->curr_sign->pname = pool_copy(m, sl->p);
      sl->curr_inst = sl->curr_sign->inst;
      if (type == sx_tle_componly)
	list_add(sl->componly, sl->curr_inst);

      /* At this point sl->curr_sign is still set, e.g., to the @lref;
	 this means we can use the sign's inst for the list inst */
      if (type == sx_tle_lref)
	{
	  if (!asl_list_lref_guard(locp, sl, n, sl_ll_lref))
	    {
	      struct sl_list *l = memo_new(sl->m_lists);
	      l->name = n;
	      l->type = sl_ll_lref;
	      sl->curr_inst = l->inst = sl->curr_sign->inst;
	      hash_add(sl->hlentry, n, l);
	      asl_register_list_item(locp, sl, l);
	    }
	}
      else
	sl->curr_inst = sl->curr_sign->inst;
      /* There is no current sign in effect after a tle except for componly in a form */
      sl->curr_sign = NULL;
    }
}

static void
asl_bld_sign_sub(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
		 int minus_flag, enum sx_tle type)
{
  int literal = 0, query = 0;
  struct sl_sign *s;

  if (minus_flag)
    sl->curr_invalid = 1;
  
  if (!sl)
    {
      fprintf(stderr, "asl: signlist must begin with @signlist [PROJECTNAME]\n");
      exit(1);
    }

  check_flags(locp, (char*)n, &query, &literal);
  struct sl_token *tp = asl_bld_token(locp, sl, (ucp)n, 0);

  if ((type == sx_tle_sign || type == sx_tle_pcun) && !minus_flag) /* might need to allow compoundonly also */
    asl_bld_num(locp, sl, (ucp)n, tp, 1);

  sl->curr_form = NULL;
  sl->curr_value = NULL;

  if ((s = hash_find(sl->hsentry, n)))
    {
      if (s->inst->valid && minus_flag)
	mesg_verr(locp, "@sign- %s duplicates @sign- %s; remove one", n, n);
      else if (!s->inst->valid || minus_flag)
	mesg_verr(locp, "@sign- %s duplicates valid @sign %s; remove one", n, n);
      else
	mesg_verr(locp, "duplicate @sign %s (first occurrence at line %d)\n", n, s->inst->mloc.line);
    }
  else
    {
      struct sl_inst *i = new_inst(sl);
      s = memo_new(sl->m_signs);
      s->name = n;
      s->inst = i;
      i->type = 's';
      s->deep = tp->deep;
      i->mloc = *locp;
      i->valid = (Boolean)!minus_flag;
      i->query = (Boolean)query;
      i->literal = (Boolean)literal;
      sl->curr_inst = i;
      i->u.s = s;
      hash_add(sl->hsentry, s->name, s);
      asl_register_sign(locp, sl, s);
    }
  s->type = type;
  sl->curr_sign = s;
}

void
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  asl_bld_sign_sub(locp, sl, n, minus_flag, sx_tle_sign);
}

void
asl_bld_liga(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *f, const unsigned char *u)
{
  if (asl_sign_guard(locp, sl, "liga"))
    {
      struct sl_liga *lp = memo_new(sl->m_ligas);
      lp->n = n;
      lp->f = f;
      lp->u = u;
      if (sl->curr_form)
	{
	  if (!sl->curr_form->u.f->h_ligas)
	    sl->curr_form->u.f->h_ligas = hash_create(3);
	  hash_add(sl->curr_form->u.f->h_ligas, lp->n, lp);
	}
      else
	{
	  if (!sl->curr_sign->h_ligas)
	    sl->curr_sign->h_ligas = hash_create(3);
	  hash_add(sl->curr_sign->h_ligas, lp->n, lp);
	}
    }
}

void
asl_bld_pcun(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  asl_bld_sign_sub(locp, sl, n, minus_flag, sx_tle_pcun);
}

void
asl_bld_xsux(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  asl_bld_sign_sub(locp, sl, n, minus_flag, sx_tle_xsux);
}

void
asl_bld_end_sign(Mloc *locp, struct sl_signlist *sl, enum sx_tle t)
{
  if (sl->curr_sign)
    {
      if (sl->curr_sign->type == t)
	{
	  if (sl->curr_sign->script)
	    {
	      struct sl_scriptdata *dp;
	      const char *code = sl->curr_sign->U.uhex;
	      if (!code)
		code = sl->curr_sign->U.useq;
	      if (!code)
		code = sl->curr_sign->U.upua;
	      if (code)
		for (dp = list_first(sl->curr_sign->script); dp; dp = list_next(sl->curr_sign->script))
		  dp->code = code;
	      else
		mesg_verr(locp, "%s uses @script but has no unicode info", sl->curr_sign->name);
	    }
	  if (!sl->curr_sign->inst->key)
	    mesg_verr(locp, "no key set for sign %s", sl->curr_sign->name);
	  sl->curr_sign = NULL;
	  sl->curr_form = NULL;
	  sl->curr_inst = NULL;
	  sl->curr_invalid = 0;
	}
      else
	mesg_verr(locp, "mismatched @end");
    }
  else
    mesg_verr(locp, "misplaced @end sign, not in an @sign");    
}

/* This routine is overloaded to set @domain as well as
   @project/@signlist project=0 == signlist; project=1 == project;
   project=2 == domain */
void
asl_bld_signlist(Mloc *locp, const unsigned char *n, int project)
{
  if (!curr_asl)
    {
      curr_asl = asl_bld_init();
      curr_asl->mloc = *locp;
      curr_asl->curr_inst = curr_asl->notes = new_inst(curr_asl);
    }
  if (n)
    {
      while (isspace(*n))
	++n;
      if (project == 1)
	curr_asl->project = (ccp)n;
      if (project == 2)
	curr_asl->domain = (ccp)n;
      else
	curr_asl->signlist = (ccp)n;
    }
}

void
asl_bld_sys(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv)
{
  struct sl_inst *ip = NULL;
  if (sl->curr_form)
    ip = sl->curr_form;
  else if (sl->curr_sign)
    ip = sl->curr_sign->inst;
  else
    mesg_verr(locp, "misplaced @sys: must belong to @sign or @form");
  if (ip)
    {
      char *xsysname = (char*)pool_copy((uccp)sysname, sl->p);
      char *subname = strchr(xsysname, ':');
      if (subname)
	*subname++ = '\0';
      if (!hash_find(sl->sysdefs, (uccp)xsysname))
	{
	  mesg_verr(locp, "undefined system name '%s' in @sys", xsysname);
	  return;
	}
      /* FIXME: need to validate values as well, but probably best
	 done in sx_marshall when all values are known */
      if (!ip->sys)
	{
	  ip->sys = list_create(LIST_SINGLE);
	  if (!sl->syslists)
	    sl->syslists = list_create(LIST_SINGLE);
	  list_add(sl->syslists, ip->sys);
	}
      struct sl_sys *sp = memo_new(sl->m_syss);
      sp->name = xsysname;
      if (subname)
	sp->subname = subname;
      sp->v = v;
      sp->vv = pool_copy(vv, sl->p);
      sp->ip = ip;
      list_add(ip->sys, sp);
    }
}

void
asl_bld_images(Mloc *locp, struct sl_signlist *sl, const unsigned char *n)
{
  if (n)
    {
      while (isspace(*n))
	++n;
      Mloc *m = mloc_mloc(locp);
      m->user = (void*)n;
      if (!sl->images)
	sl->images = list_create(LIST_SINGLE);
      list_add(sl->images, m);
    }
  else
    mesg_verr(locp, "@images has no image file name");
}

void
asl_bld_merge(Mloc *locp, struct sl_signlist *sl, const unsigned char *n)
{
  if (asl_sign_guard(locp, sl, "merge"))
    {
      if (n)
	{
	  while (isspace(*n))
	    ++n;
	  if (sl->curr_sign->merge)
	    mesg_verr(locp, "multiple @merge commands in the same sign are not allowed");
	  else
	    sl->curr_sign->merge = list_from_str((char *)n, NULL, LIST_SINGLE);
	  unsigned const char *p;
	  /* curr_sign is the sign receiving merges; the names given
	     in @merge are the ones being merged */
	  for (p = list_first(sl->curr_sign->merge); p; p = list_next(sl->curr_sign->merge))
	    {
	      if (hash_find(sl->h_merge, p))
		mesg_verr(locp, "sign %s given in multiple @merge lines", p);
	      else
		{
		  Mloc *m = mloc_mloc(locp);
		  m->user = (void*)sl->curr_sign->name;
		  hash_add(sl->h_merge, p, m);
		}
	    }
	}
      else
	mesg_verr(locp, "@merge has no sign names");
    }
}

void
asl_bld_scriptdef(Mloc *locp, struct sl_signlist *sl, char *n)
{
  if (n)
    {
      while (isspace(*n))
	++n;
      /* parse and store @scriptdef data */
      char *sname = n;
      while (*n && !isspace(*n))
	++n;
      if (*n)
	{
	  *n++ = '\0';
	  char *sarg = n;
	  while (*n && !isspace(*n))
	    ++n;
	  if (isspace(*n))
	    *n = '\0';
	  if (!*n)
	    {
	      struct sl_scriptdef *sp = memo_new(sl->m_scriptdata);
	      sp->name = sname;
	      if ('=' == *sarg)
		{
		  ++sarg;
		  struct sl_scriptdef *sp2 = hash_find(sl->h_scripts, (uccp)sarg);
		  if (sp2)
		    {
		      sp->sset = sarg;
		      sp->codes = sp2->codes;
		      hash_add(sl->h_scripts, (uccp)sp->name, sp);
		    }
		  else
		    mesg_verr(locp, "@scriptdef %s has bad =%s (@scriptdef %s needed before =%s)", sp->name, sarg, sarg, sarg);
		}
	      else
		{
		  sp->sset = sarg;
		  sp->codes = list_create(LIST_SINGLE);
		  hash_add(sl->h_scripts, (uccp)sp->name, sp);
		}
	    }
	  else
	    mesg_verr(locp, "@scriptdef has bad argument to script %s", n);
	}
      else
	mesg_verr(locp, "@scriptdef has no script name", n);
    }
}

static char *
asl_script_elt(Mloc *locp, char *n, char **argp, char **endp)
{
  if (n && *n)
    {
      while (isspace(*n))
	++n;
      char *e = n;
      while (*e && !isspace(*e))
	++e;
      if (*e)
	{
	  *e++ = '\0';
	  while (isspace(*e))
	    ++e;
	  if (*e)
	    {
	      *argp = e;
	      while (*e && ';' != *e && '.' != *e)
		++e;
	      if (*e)
		{
		  *e++ = '\0';
		  *endp = e;
		  return n;
		}
	      else
		mesg_verr(locp, "@script directive %s %s is should end with ';' or '.'", n, *argp);
	    }
	  else
	    mesg_verr(locp, "@script directive %s has no argument", n);
	}
      else
	mesg_verr(locp, "no space after directive name in @script");
    }
  return NULL;
}

void
asl_bld_script(Mloc *locp, struct sl_signlist *sl, char *n)
{
  if (asl_sign_guard(locp, sl, "script"))
    {
      if (n)
	{
	  while (isspace(*n))
	    ++n;
	  /* parse and store @script data */
	  struct sl_scriptdata *sp = memo_new(sl->m_scriptdata);
	  if (!sl->curr_sign->script)
	    sl->curr_sign->script = list_create(LIST_SINGLE);
	  char *end;
	  if ((end = strchr(n, ':')))
	    {
	      *end++ = '\0';
	      sp->name = n;
	      int status = mesg_status();
	      char *arg = NULL;
	      while (isspace(*end))
		++end;
	      char *elt;
	      while ((elt = asl_script_elt(locp, end, &arg, &end)))
		{
		  if (!strncmp(elt, "ivs", strlen("ivs")))
		    sp->oivs = arg;
		  else if (!strncmp(elt, "salt", strlen("salt")))
		    sp->salt = arg;
		  else if (!strncmp(elt, "sset", strlen("sset")))
		    sp->sset = arg;
		  else if (!strncmp(elt, "cvnn", strlen("cvnn")))
		    sp->sset = arg;
		  else if (!strncmp(elt, "merge", strlen("merge")))
		    sp->merge = arg;
		  else
		    {
		      mesg_verr(locp, "unknown script element %s", n);
		      break;
		    }
		}
	      if (mesg_status() == status)
		{
		  if (sl->curr_form)
		    sp->sign = sl->curr_form->u.f->sign;
		  else
		    sp->sign = sl->curr_sign;
		  /* if n is a comma list that is what goes into curr_sign script list */
		  list_add(sl->curr_sign->script, sp);
		  /* if n is a comma list the sp gets added to all the script lists */
		  vec_sep_push(",");
		  char **names = vec_from_str(n, NULL, NULL);
		  vec_sep_pop();
		  int i;
		  for (i = 0; names[i]; ++i)
		    {
		      struct sl_scriptdef *defp = hash_find(sl->h_scripts, (uccp)names[i]);
		      if (defp)
			list_add(defp->codes, sp);
		      else
			mesg_verr(locp, "@script %s has not been defined with @scriptdef", names[i]);
		      free(names[i]);
		    }
		  free(names);
		}
	    }
	  else
	    mesg_verr(locp, "missing ':' in @script");
	}
      else
	mesg_verr(locp, "@script is empty");
    }
}

void
asl_bld_oid(Mloc *locp, struct sl_signlist *sl, const unsigned char *n)
{
  if (sl->curr_inst)
    {
      if ('s' == sl->curr_inst->type || 'f' == sl->curr_inst->type)
	{
	  sl->curr_inst->atoid = (ccp)n;
	  if ('s' == sl->curr_inst->type)
	    asl_add_key(locp, sl, sl->curr_inst, (ccp)n, NULL, NULL);
	  else
	    asl_add_key(locp, sl, sl->curr_inst, sl->curr_sign->inst->atoid, (ccp)n, NULL);
	}
      else
	mesg_verr(locp, "misplaced @oid--only allowed in @sign or @form");
    }
}

void
asl_bld_uhex(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "uhex"))
    asl_bld_singleton_string(locp, t, "uhex",
			     sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.uhex : (uccp*)&sl->curr_sign->U.uhex,
			     sl->curr_form ? &sl->curr_form->uhex : &sl->curr_sign->inst->uhex);
}

void
asl_bld_upua(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "upua"))
    {
      asl_bld_singleton_string(locp, t, "upua",
			       sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.upua : (uccp*)&sl->curr_sign->U.upua,
			       sl->curr_form ? &sl->curr_form->upua : &sl->curr_sign->inst->upua);
      /* Check/register @upua U+F0000 as if it were @list U+F0000 */
      struct sl_listdef *ldp = NULL;
      if ((ldp = hash_find(sl->listdefs, (uccp)"U+")))
	{
	  if (!(hash_find(ldp->known, t)))
	    {
	      mesg_verr(locp, "%s is not in @listdef %s", t+strlen("U+"), "U+");
	    }
	  else
	    {
	      /* Add it to the seen Unicode values as a flag value
		 pointer; the @upua is not added to the signlist's
		 list of lists, so this should not ever need to be
		 dereferenced ... */
	      if (!(hash_find(ldp->seen, t)))
		hash_add(ldp->seen, t, (void*)(uintptr_t)-1);
	    }
	}
    }
}

void
asl_bld_utf8(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "ucun"))
    asl_bld_singleton_string(locp, t, "ucun",
			     sl->curr_form ? &sl->curr_form->u.f->U.utf8 : &sl->curr_sign->U.utf8,
			     sl->curr_form ? &sl->curr_form->utf8 : &sl->curr_sign->inst->utf8);
}

void
asl_bld_umap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "umap"))
    asl_bld_singleton_string(locp, t, "umap",
			     sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.umap : (uccp*)&sl->curr_sign->U.umap,
			     sl->curr_form ? &sl->curr_form->umap : &sl->curr_sign->inst->umap);
}

void
asl_bld_useq(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "useq"))
    asl_bld_singleton_string(locp, t, "useq",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.useq : &sl->curr_sign->U.useq),
			     sl->curr_form ? &sl->curr_form->useq : &sl->curr_sign->inst->useq);
}

void
asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "uname"))
    asl_bld_singleton_string(locp, t, "uname",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.uname : &sl->curr_sign->U.uname),
			     sl->curr_form ? &sl->curr_form->uname : &sl->curr_sign->inst->uname);
}

void
asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "unote"))
    asl_bld_list_string(t, sl->curr_form ? &sl->curr_form->u.f->U.unotes : &sl->curr_sign->U.unotes);
}

void
asl_bld_urev(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "urev"))
    asl_bld_singleton_string(locp, t, "urev",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.urev : &sl->curr_sign->U.urev),
			     sl->curr_form ? &sl->curr_form->urev : &sl->curr_sign->inst->urev);
}

void
asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
	      const char *lang, int atf_flag, int minus_flag)
{
  struct sl_value *v;
  struct sl_inst *i = NULL; 
  int literal = 0, xvalue = 0, uvalue = 0, query = 0;
  const unsigned char *base = NULL;

  check_flags(locp, (char*)n, &query, &literal);

  if (minus_flag)
    {
      if (!sl->curr_invalid)
	sl->curr_invalid = 3;
    }
  else if (sl->curr_invalid == 3)
    sl->curr_invalid = 0;

  /* special case 1/3(|NINDA₂×(ŠE.AŠ)|) and  2/3(|NINDA₂×(ŠE.AŠ.AŠ)|) */
  if ('/' == n[1]
      && (!strcmp((ccp)n, "1/3(|NINDA₂×(ŠE.AŠ)|)") || !strcmp((ccp)n, "2/3(|NINDA₂×(ŠE.AŠ.AŠ)|)")
	  || !strcmp((ccp)n, "1/3(|NINDA₂×(ŠE.1(AŠ))|)") || !strcmp((ccp)n, "2/3(|NINDA₂×(ŠE.2(AŠ))|)")
	  ))
    base = n;
  else
    base = g_base_of(n);

  struct sl_token *tp = hash_find(sl->htoken,base);
  if (!tp)
    {
      base = pool_copy(base, sl->p);
      (void)asl_bld_token(locp, sl, (ucp)base, -1); /* guarantee that every base has a sort value */
    }
  else
    base = tp->t;

  /* if the base of the new value duplicates a base in the current
     form/sign, it's an error; don't proceed with it, just warn and
     return */

  if (sl->curr_form)
    {
      unsigned const char *b = NULL;
      if (!minus_flag && sl->curr_form->lv && sl->curr_form->lv->hvbases && (b = hash_find(sl->curr_form->lv->hvbases, base)))
	{
	  mesg_verr(locp, "form %s values %s and %s have the same base %s\n", sl->curr_form->u.f->name, b, n, base);
	  return;
	}
      else if (!minus_flag) /* only add valid values to hvbases */
	{
	  if (!sl->curr_form->lv->hvbases)
	    sl->curr_form->lv->hvbases = hash_create(1);
	  hash_add(sl->curr_form->lv->hvbases, (uccp)base, (void*)n);
	}
    }
  else if (sl->curr_sign)
    {
      unsigned const char *b = NULL;
      if (!minus_flag && (b = hash_find(sl->curr_sign->hvbases, base)))
	{
	  mesg_verr(locp, "sign %s values %s and %s have the same base %s\n", sl->curr_sign->name, b, n, base);
	  return;
	}
      else if (!minus_flag)
	{
	  if (!sl->curr_sign->hvbases)
	    sl->curr_sign->hvbases = hash_create(1);
	  hash_add(sl->curr_sign->hvbases, (uccp)base, (void*)n);
	}
    }
  else
    {
      (void)asl_sign_guard(locp, sl, "v");
      return;
    }

  tp = asl_bld_token(locp, sl, (ucp)n, 0);
  if (!sl->curr_invalid)
    asl_bld_num(locp, sl, (ucp)n, tp, 3);
  
  if (strlen((ccp)n) > 3)
    {
      if (!strcmp((ccp)(n+(strlen((ccp)n)-3)), U_s_x_u8str))
	xvalue = 1;
      /*fprintf(stderr, "detected x-value %s\n", n);*/
    }
  else if (!strcmp((ccp)n, "x"))
    uvalue = 1;

  i = new_inst(sl);
  i->type = 'v';
  i->mloc = *locp;
  i->valid = (Boolean)!minus_flag;
  i->query = (Boolean)query;
  if (lang)
    i->lang = lang+1; /* skip the % */
  if (sl->curr_form)
    i->parent_f = sl->curr_form;
  else
    i->parent_s = sl->curr_sign->inst;
  sl->curr_inst = i;

  
  /* If we are processing a sign and the v is already in
     signlist's sign-values it's an error for it to occur again */
  if ((v = hash_find(sl->hsignvvalid, n)))
    {
      if (!sl->curr_form && !xvalue && !v->atf && !minus_flag && sl->curr_sign->inst->valid)
	{
	  if (!strcmp((ccp)v->sowner->name, (ccp)sl->curr_sign->name))
	    mesg_verr(locp, "value %s occurs more than once in sign %s\n", n, sl->curr_sign->name);
	  else
	    mesg_verr(locp, "duplicate value %s in sign %s (first occurs in %s)\n",
		      n, sl->curr_sign->name, v->sowner->name);
	  return;
	}
    }

  if (!v && !(v = hash_find(sl->hventry, n)))
    {
      v = memo_new(sl->m_values);
      hash_add(sl->hventry, n, v);
      v->name = n;
      v->atf = atf_flag;
      v->unknown = uvalue;

      v->insts = list_create(LIST_SINGLE);
      list_add(v->insts, i);
      
      /* Add inst to sign or form's values */
      if (sl->curr_form)
	{
	  if (!v->fowners)
	    v->fowners = list_create(LIST_SINGLE);
	  list_add(v->fowners, sl->curr_form);
	  if (!sl->curr_form->lv->hventry)
	    sl->curr_form->lv->hventry = hash_create(1);
	  hash_add(sl->curr_form->lv->hventry, v->name, i);
	}
      else
	{
	  if (xvalue)
	    {
	      if (!v->fowners)
		v->fowners = list_create(LIST_SINGLE);
	      list_add(v->fowners, sl->curr_sign->inst);
	    }
	  else
	    v->sowner = sl->curr_sign;
	  if (!sl->curr_sign->hventry)
	    sl->curr_sign->hventry = hash_create(1);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	  if (sl->curr_sign->inst->valid && i->valid)
	    hash_add(sl->hsignvvalid, v->name, v);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	}
    }      
  else
    {
      if (sl->curr_form)
	{
	  if (!sl->curr_form->lv->hventry)
	    sl->curr_form->lv->hventry = hash_create(1);
	  if (hash_find(sl->curr_form->lv->hventry, n))
	    {
	      if (!xvalue)
		{
		  mesg_verr(locp, "duplicate value %s in form %s::%s", n,
			    sl->curr_sign->name, sl->curr_form->u.f->name);
		  return;
		}
	    }
	  else
	    {
	      hash_add(sl->curr_form->lv->hventry, v->name, i);
	      if (!v->fowners)
		v->fowners = list_create(LIST_SINGLE);
	      list_add(v->fowners, sl->curr_form);
	    }
	}
      else
	{
	  if (!sl->curr_sign->hventry)
	    sl->curr_sign->hventry = hash_create(1);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	  if (xvalue)
	    list_add(v->fowners, sl->curr_sign->inst);
	}
      list_add(v->insts, i);
    }

  v->base = base;
  
  i->u.v = v;
  i->u.v->xvalue = xvalue;
  sl->curr_value = i;

  if (sl->curr_form)
    asl_add_key(locp, sl, i, sl->curr_sign->inst->atoid, i->parent_f->atoid, (ccp)i->u.v->name);
  else
    asl_add_key(locp, sl, i, sl->curr_sign->inst->atoid, NULL, (ccp)i->u.v->name);
}

/* The lexer rules only allow the flags in the order:
 *
 *   optional ?
 *   optional =
 */
static void
check_flags(Mloc *locp, char *n, int *q, int *l)
{
  char *last = (char*)(n + (strlen((ccp)n)-1));
  *l = *q = 0;
  if ('=' == *last)
    {
      mesg_verr(locp, "literal strings are no longer marked with '='; enclose in \"...\" instead");
#if 0      
      *last-- = '\0';
      *l = 1;
#endif
    }
  if ('?' == *last)
    {
      *last = '\0';
      *q = 1;
    }
  if (asl_literal_flag)
    *l = 1;
}
