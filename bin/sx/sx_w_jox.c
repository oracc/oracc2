#include <list.h>
#include <xml.h>
#include <gdl.h>
#include <signlist.h>
#include <oraccsys.h>
#include <sx.h>
#include <joxer.h>
#include <tok.h>
#include <tis.h>

int slgroups = 0;
Hash *xidseen = NULL;

static unsigned char *x_cdp_refs(struct sl_signlist *sl, struct sl_inst *s);

extern struct rnvval_atts *ratts;

static sx_signlist_f sx_w_jx_signlist;
static sx_letter_f sx_w_jx_letter;
static sx_group_f sx_w_jx_group;
static sx_sign_f sx_w_jx_sign;
static sx_form_f sx_w_jx_form;
static sx_list_f sx_w_jx_list;
static sx_value_f sx_w_jx_ivalue;
static sx_value_f sx_w_jx_qvs;
static sx_value_f sx_w_jx_value;
static sx_notes_f sx_w_jx_notes;
static sx_notes_f sx_w_jx_links;
static sx_notes_f sx_w_jx_syss;
static sx_notes_f sx_w_jx_images;
static sx_notes_f sx_w_jx_cpds;
static sx_notes_f sx_w_jx_lems;
static sx_unicode_f sx_w_jx_unicode;

struct sx_functions sx_w_jox_fncs;


struct sx_functions *
sx_w_jox_init(void)
{
  sx_w_jox_fncs.sll = sx_w_jx_signlist;
  sx_w_jox_fncs.let = sx_w_jx_letter;
  sx_w_jox_fncs.grp = sx_w_jx_group;
  sx_w_jox_fncs.sgn = sx_w_jx_sign;
  sx_w_jox_fncs.lst = sx_w_jx_list;
  sx_w_jox_fncs.frm = sx_w_jx_form;
  sx_w_jox_fncs.val = sx_w_jx_value;
  sx_w_jox_fncs.inh = sx_w_jx_ivalue;
  sx_w_jox_fncs.not = sx_w_jx_notes;
  sx_w_jox_fncs.lnk = sx_w_jx_links;
  sx_w_jox_fncs.sys = sx_w_jx_syss;
  sx_w_jox_fncs.img = sx_w_jx_images;
  sx_w_jox_fncs.cpd = sx_w_jx_cpds;
  sx_w_jox_fncs.lem = sx_w_jx_lems;
  sx_w_jox_fncs.uni = sx_w_jx_unicode;
  sx_w_jox_fncs.qvs = sx_w_jx_qvs;
  sx_w_jox_fncs.fp = NULL;
  sx_w_jox_fncs.fname = NULL;
  return &sx_w_jox_fncs;
}

static void
sx_w_jx_scripts(struct sx_functions *f, struct sl_signlist *sl)
{
  if (!sl->h_scripts->key_count)
    return;

  const char **k = hash_keys(sl->h_scripts);
  
  joxer_eaa(&sl->mloc, "sl:scripts", NULL);
  joxer_ao("j:scripts");
  int i;
  for (i = 0; k[i]; ++i)
    {
      struct sl_scriptdef *defp = hash_find(sl->h_scripts, (uccp)k[i]);
      ratts = rnvval_aa("x", 
			"n", defp->name,
			"sset", defp->sset,
			NULL);
      joxer_ea(&sl->mloc, "sl:script", ratts);
      joxer_ao("j:codes");
      struct sl_scriptdata *sdp;
      for (sdp = list_first(defp->codes); sdp; sdp = list_next(defp->codes))
	{
	  List *a = list_create(LIST_SINGLE);
	  if (sdp->code)
	    {
	      list_add(a, "code");
	      list_add(a, (void*)sdp->code);
	    }
	  if (sdp->salt)
	    {
	      list_add(a, "salt");
	      list_add(a, (void*)sdp->salt);
	    }
	  if (sdp->oivs)
	    {
	      list_add(a, "oivs");
	      list_add(a, (void*)sdp->oivs);
	    }
	  if (sdp->sset)
	    {
	      list_add(a, "sset");
	      list_add(a, (void*)sdp->sset);
	    }
	  const char **atts = list2chars(a);
	  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	  joxer_ec(&sl->mloc, "sl:code", ratts);
	}
      joxer_ac();/*close j:codes"*/
      joxer_ac();/*close j:script*/
      joxer_ee(&sl->mloc, "sl:script");
    }
  joxer_ac();/*close j:scripts*/
  joxer_eea(&sl->mloc, "sl:scripts");  
}

static const uchar *
numset_unit(struct sl_signlist *sl, struct sl_numset *nsp)
{
  int k;
  for (k = nsp->from; k <= nsp->last; ++k)
    if (!strcmp((ccp)sl->numbers[k].rep, "1"))
      return sl->numbers[k].oid;
  return sl->numbers[nsp->from].oid;
}

static void
sx_w_jx_numbers(struct sx_functions *f, struct sl_signlist *sl)
{
  sx_numbers(sl);
  joxer_eaa(&sl->mloc, "sl:numbers", NULL);
  int i;
#define nsfrom_num(ns) sl->numbers[sl->numsets[ns].from]
#define nslast_num(ns) sl->numbers[sl->numsets[ns].last]
  joxer_ao("j:numsets");
  for (i = 0; i < sl->nnumsets; ++i)
    {
      char nsid[64];
      sprintf(nsid, "%s.ns%d",nsfrom_num(i).oid, i);
      char setsort[10];
      sprintf(setsort, "%d", nsfrom_num(i).setsort);
      const char *sset = nsfrom_num(i).sset;
      const uchar *unit_oid = numset_unit(sl, &sl->numsets[i]);
      struct noset_tab *nosetp = noset((ccp)nsfrom_num(i).set, strlen((ccp)nsfrom_num(i).set));
      if (!nosetp)
	{
	  if (sset)
	    ratts = rnvval_aa("x", 
			      "xml:id", nsid,
			      "n", nsfrom_num(i).set,
			      "noid", unit_oid,
			      "sort", setsort,
			      "sset", sset,
			      NULL);
	  else
	    ratts = rnvval_aa("x", 
			      "xml:id", nsid,
			      "n", nsfrom_num(i).set,
			      "noid", unit_oid,
			      "sort", setsort,
			      NULL);	    
	  joxer_ea(&sl->mloc, "sl:numset", ratts);
	  joxer_ao("j:nums");
	  int j;
	  for (j = sl->numsets[i].from; j <= sl->numsets[i].last; ++j)
	    {
	      struct sl_number *np = &sl->numbers[j];
	      char sort[10];
	      sprintf(sort, "%d", np->t->s);
	      if (np->sset)
		ratts = rnvval_aa("x",
				  "n", np->t->t,
				  "ref", np->ref,
				  "oid", np->oid,
				  "rep", np->rep,
				  "sort", sort,
				  "sset", np->sset,
				  NULL
				  );
	      else
		ratts = rnvval_aa("x",
				  "n", np->t->t,
				  "ref", np->ref,
				  "oid", np->oid,
				  "rep", np->rep,
				  "sort", sort,
				  NULL
				  );
	      joxer_ec(&sl->mloc, "sl:num", ratts);
	    }
	  joxer_ac();/*close j:nums*/
	  joxer_ee(&sl->mloc, "sl:numset");
	}
    }
  joxer_ac();/*close j:numsets*/
  joxer_eea(&sl->mloc, "sl:numbers");  
}

static void
sx_w_jx_homophones(struct sx_functions *f, struct sl_signlist *sl)
{
  const char **keys;
  int nkeys, i;
  /*joxer_ao("j:homophones");*/
  joxer_eaa(NULL, "sl:homophones", NULL);
  keys = hash_keys2(sl->homophones, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      List *lp = hash_find(sl->homophones, (uccp)keys[i]);
      struct sl_token *tp = hash_find(sl->htoken, (uccp)keys[i]);

      /* Some values, e.g., 1/3(|NINDA₂×(ŠE.AŠ)|), do not have a lowercased base */
      if (!tp)
	continue;

      char ssort[32];
      sprintf(ssort,"%d",tp->s);
      ratts = rnvval_aa("x",
			"xml:id", hash_find(sl->homophone_ids, (uccp)keys[i]),
			"n", keys[i],
			"count", itoa(list_len(lp)),
			"sort", ssort,
			NULL);
      joxer_ao("j:bases");
      joxer_ea(&sl->mloc, "sl:base", ratts);
      joxer_ao("j:h");
      struct sl_split_value *spv;
      for (spv = list_first(lp); spv; spv = list_next(lp))
	{
	  struct sl_token *vp = hash_find(sl->htoken, (uccp)spv->v);
	  char vsort[32];
	  if (vp)
	    sprintf(vsort, "%d", vp->s);
	  else
	    *vsort = '\0';
	  char ssort[32];
	  if (spv->oid)
	    sprintf(ssort, "%d", (int)(uintptr_t)hash_find(oid_sort_keys, (uccp)spv->oid));
	  ratts = rnvval_aa("x",
			    "v", spv->v,
			    "oid", spv->oid,
			    "vsort", vsort,
			    "ssort", ssort,
			    NULL);
	  joxer_ec(&sl->mloc, "sl:h", ratts);
	}
      joxer_ac();
      joxer_ee(&sl->mloc, "sl:base");
      joxer_ac();
    }
  joxer_eea(&sl->mloc, "sl:homophones");
  /*joxer_ac();*/
}

static void
sx_w_jx_lems(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst*ip)
{
  joxer_eaaa(&ip->mloc, "sl:lemmas", NULL);
  struct cbdex *lm;
  /*joxer_ao("j:lemma");*/
  for (lm = list_first(ip->lp); lm; lm = list_next(ip->lp))
    {
      ratts = rnvval_aa("x",
			"n", lm->lem,
			"oid", lm->oid,
			"sort", lm->sort,
			"lcnt", lm->lcnt,
			"base", lm->base,
			"bcnt", lm->bcnt,
			"gpos", lm->gpos,
			NULL);
      joxer_ec(&ip->mloc, "sl:lemma", ratts);
    }
  /*joxer_ac();*/
  joxer_eeaa(&ip->mloc, "sl:lemmas");
}

/* This is the entry point for xml output */
static void
sx_w_jx_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
    if (p == sx_pos_init)
    {
      if (ldata_http)
	ratts = rnvval_aa("x", "project", sl->project, "signlist", sl->signlist,  "domain", sl->domain, "lemma-http", ldata_http, NULL);
      else
	ratts = rnvval_aa("x", "project", sl->project, "signlist", sl->signlist,  "domain", sl->domain, NULL);

      joxer_ea(&sl->mloc, "sl:signlist", ratts);
      xidseen = hash_create(1024);
      
      sx_w_jx_notes(f, sl, sl->notes);
      int nn, i;
      const char **n = hash_keys2(sl->listdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);

      joxer_ao("j:listdefs");
      for (i = 0; i < nn; ++i)
	{
	  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)n[i]);
	  ratts = rnvval_aa("x", "name", n[i], NULL);
	  joxer_ea(&ldp->inst.mloc, "sl:listdef", ratts);
	  joxer_et(&ldp->inst.mloc, "sl:info", NULL, ldp->str);
	  sx_w_jx_notes(f, sl, &ldp->inst);
	  joxer_ee(&ldp->inst.mloc, "sl:listdef");
	}
      joxer_ac();

      n = hash_keys2(sl->sysdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      joxer_ao("j:sysdefs");
      for (i = 0; i < nn; ++i)
	{
	  struct sl_sysdef *sdp = hash_find(sl->sysdefs, (uccp)n[i]);
	  ratts = rnvval_aa("x", "name", n[i], NULL);
	  joxer_ea(&sdp->inst.mloc, "sl:sysdef", ratts);
	  if (sdp->comment)
	    joxer_et(&sdp->inst.mloc, "sl:info", NULL, sdp->comment);
	  sx_w_jx_notes(f, sl, &sdp->inst);
	  joxer_ee(&sdp->inst.mloc, "sl:sysdef");
	}
      joxer_ac();

      n = hash_keys2(sl->linkdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      joxer_ao("j:linkdefs");
      for (i = 0; i < nn; ++i)
	{
	  struct sl_linkdef *sdp = hash_find(sl->linkdefs, (uccp)n[i]);
	  ratts = rnvval_aa("x", "name", n[i], NULL);
	  joxer_ea(&sdp->inst.mloc, "sl:linkdef", ratts);
	  if (sdp->comment)
	    joxer_et(&sdp->inst.mloc, "sl:info", NULL, sdp->comment);
	  sx_w_jx_notes(f, sl, &sdp->inst);
	  joxer_ee(&sdp->inst.mloc, "sl:linkdef");
	}
      joxer_ac();

      joxer_ao("j:iheaders");
      if (sl->iheaders)
	{
	  for (i = 0; i < list_len(sl->images); ++i)
	    {
	      const char *o = itoa(i);
	      ratts = rnvval_aa("x",
				"xml:id", sl->iheaders[i].id,
				"proj", sl->iheaders[i].proj ? sl->iheaders[i].proj : ".",
				"path", sl->iheaders[i].path,
				"order", o,
				"label", sl->iheaders[i].label,
				sl->iheaders[i].thumb ? "thumb" : NULL,
				sl->iheaders[i].thumb ? sl->iheaders[i].thumb : NULL, 
				NULL);
	      joxer_ec(&sl->iheaders[i].mloc, "sl:iheader", ratts);
	    }
	}
      joxer_ac();
    }
  else if (p == sx_pos_term)
    {
      sx_w_jx_homophones(f, sl);
      sx_w_jx_numbers(f, sl);
      sx_w_jx_scripts(f, sl);
      joxer_ee(&sl->eloc, "sl:signlist");
      hash_free(xidseen, NULL);
    }
}

static void
sx_w_jx_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
  if (slgroups)
    {
      static int in_group = 0;
      if (g->signs)
	{
	  if (p == sx_pos_inst)
	    {
	      if (in_group)
		{
		  joxer_ee(&g->signs[g->nsigns-1]->mloc, "sl:signs");
		  joxer_ac();
		}
	  
	      ratts = rnvval_aa("x", "name", g->name, "title", g->name, NULL);
	      joxer_ea(&g->signs[0]->mloc, "sl:signs", ratts);
	      joxer_ao("j:group");
	      in_group = 1;
	    }
	  else if (p == sx_pos_term)
	    {
	      if (in_group)
		{
		  joxer_ee(&g->signs[g->nsigns-1]->mloc, "sl:signs");
		  joxer_ac();
		}
	      in_group = 0;
	    }
	}
    }
}

static void
sx_w_jx_letter(struct sx_functions *f, struct sl_signlist *sl, struct sl_letter *l, enum sx_pos_e p)
{
  static int in_letter = 0;

  if (p == sx_pos_init)
    joxer_ao("j:letters");
  if (l)
    {
      if (p == sx_pos_inst)
	{
	  if (in_letter)
	    {
	      struct sl_group *glast = &l->groups[l->ngroups-1];
	      struct sl_inst *ilast = glast->signs[glast->nsigns-1];
	      joxer_ac();
	      joxer_ee(&ilast->mloc, "sl:letter");
	    }
	  if (l->name)
	    {
	      struct sl_inst *i1 = l->groups[0].signs[0];
	      if (l->code == -1)
		ratts = rnvval_aa("x", "name", l->name, "title", l->name, "xml:id", l->xmlid, "num", "1", NULL);
	      else
		ratts = rnvval_aa("x", "name", l->name, "title", l->name, "xml:id", l->xmlid, NULL);
	      joxer_ea(&i1->mloc, "sl:letter", ratts);
	      joxer_ao("j:signs");
	      in_letter = 1;
	    }
	  else
	    fprintf(stderr, "letter has no name/id\n");
	}
    }
  if (p == sx_pos_term)
    {
      if (in_letter)
	{
	  joxer_ac();
	  if (l && l->groups)
	    {
	      struct sl_group *glast = &l->groups[l->ngroups-1];
	      struct sl_inst *ilast = glast->signs[glast->nsigns-1];
	      joxer_ee(&ilast->mloc, "sl:letter");
	    }
	  else
	    joxer_ee(NULL, "sl:letter");
	  in_letter = 0;
	}
      joxer_ac();
    }
}

static char *
charstarstar_concat(const char **p)
{
  int len, i;
  char *ret;
  for (i = len = 0; p[i]; ++i)
    len += strlen(p[i]) + 1;
  ret = malloc(len+1);
  *ret = '\0';
  for (i = 0; p[i]; ++i)
    {
      strcat(ret, p[i]);
      strcat(ret, " ");
    }
  ret[strlen(ret)-1] = '\0';
  return ret;
}

static void
sx_w_jx_cpd_elt(const char *name, const char **oids)
{
  if (oids)
    {
      char *s = charstarstar_concat(oids);
      struct rnvval_atts *ratts = NULL;
      ratts = rnvval_aa("x", "oids", s, NULL);
      joxer_eaa(NULL, name, ratts);
      joxer_eea(NULL, name);
    }
}

static void
sx_w_jx_cpds(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  struct sl_compound_digest *cdp = hash_find(ip->u.s->hcompounds, (uccp)"#digest_by_oid");
  if (cdp)
    {
      char icnt[12];
      sprintf(icnt, "%ld", ip->u.s->ctotal);
      ratts = rnvval_aa("x", "icnt", icnt, NULL);
      joxer_eaa(NULL, "sl:cpds", ratts);
      sx_w_jx_cpd_elt("sl:memb", cdp->memb);
      sx_w_jx_cpd_elt("sl:init", cdp->initial);
      sx_w_jx_cpd_elt("sl:medl", cdp->medial);
      sx_w_jx_cpd_elt("sl:finl", cdp->final);
      sx_w_jx_cpd_elt("sl:ctnr", cdp->container);
      sx_w_jx_cpd_elt("sl:ctnd", cdp->contained);
      joxer_eea(NULL, "sl:cpds");
    }
}

static void
sx_w_jx_images(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  int skip_tle = 0;
  if (ip->type == 's')
    skip_tle = (ip->u.s->type == sx_tle_componly
		|| ip->u.s->type == sx_tle_lref
		|| ip->u.s->type == sx_tle_sref);
  else
    skip_tle = ip->u.f->compoundonly;
  if (ip && !ip->inherited && !skip_tle)
    {
      const char *oid = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
      if (oid)
	{
	  int index = (uintptr_t)hash_find(sl->oidindexes, (uccp)oid);
	  if (index)
	    {
	      --index; /* oidindexes stores index+1 */
	      if (sl->iarray->rows[index])
		{
		  int i;
		  int n = list_len(sl->images)+1;
		  joxer_ea(NULL, "sl:images", NULL);
		  joxer_ao("j:imagenodes");
		  for (i = 1; i < n; ++i)
		    {
		      struct rnvval_atts *ratts = NULL;
		      if (sl->iarray->rows[index][i])
			{
			  Link *lp;
			  for (lp = (Link*)sl->iarray->rows[index][i]; lp; lp = lp->next)
			    {
			      const char *n = ((char**)lp->data)[2];
			      if (n)
				ratts = rnvval_aa("x", "ref", sl->iheaders[i-1].id,
						  "loc", ((char**)lp->data)[1],
						  "n", n,
						  NULL);
			      else
				ratts = rnvval_aa("x", "ref", sl->iheaders[i-1].id, "loc", ((char**)lp->data)[1], NULL);
			      joxer_ec(NULL,"sl:i", ratts);
			    }
			}
		      else
			joxer_ec(NULL,"sl:i", NULL);
		    }
		  joxer_ac();
		  joxer_ee(NULL,"sl:images");
		}
	    }
	}
    }
}

static void
sx_w_jx_aka(struct sx_functions *f, struct sl_signlist *sl, List *a)
{
  struct memo_str *msp;

  joxer_ao("j:aka");
  for (msp = list_first(a); msp; msp = list_next(a))
    {
      ratts = rnvval_aa("x", "n", msp->s, NULL);
      joxer_ea(&msp->m, "sl:aka", ratts);
      if (!msp->user) /* AKA has literal flag stored in user */
	{
	  struct sl_token *tp = hash_find(sl->htoken, msp->s);
	  joxer_eaaa(&msp->m, "sl:name", NULL);
	  grx_jox(tp->gdl, "g:w");
	  joxer_eeaa(&msp->m, "sl:name");
	}
      joxer_ee(&msp->m, "sl:aka");
    }
  joxer_ac();
}

static void
x_tis_atts(List *a, struct tis_data *tp, size_t ctotal)
{
  list_add(a, "icnt");
  list_add(a, tp->cnt);
  list_add(a, "ipct");
  list_add(a, tp->pct);
  list_add(a, "iref");
  list_add(a, tp->ref);
  list_add(a, "ctotal");
  /* This itoa is OK as long as we are printing the atts before
     another itoa occurs */
  if (ctotal)
    list_add(a, (void*)itoa(atoi(tp->cnt)+ctotal));
  else
    list_add(a, tp->cnt);
}

static void
sx_w_jx_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_form = 0;

  if (p == sx_pos_init)
    joxer_ao("j:forms");
  else if (p == sx_pos_inst)
    {
      const char *ref = NULL;
      
      if (in_form)
	{
	  joxer_ee(&s->eloc, "sl:form");
	  in_form = 0;
	}

      if (s->type == 'f')
	{
	  char scode[32];
	  struct sl_token *tp = NULL;
	  (void)sprintf(scode, "%d", s->u.f->sort);
	  tp = hash_find(sl->htoken, s->u.f->name);

	  if (s->u.f->compoundonly)
	    {
	      List *a = list_create(LIST_SINGLE);
	      const char **atts = NULL;
	      static char scode[32];
	      struct rnvval_atts *ratts = NULL;

	      list_add(a, "n");
	      list_add(a, (void*)xmlify(s->u.f->name));
  
	      list_add(a, "xml:id");
	      list_add(a, (void*)s->iid);

	      if (s->u.f->oid)
		{
		  list_add(a, "oid");
		  list_add(a, (void*)s->u.f->oid);
		}

	      if (s->u.f->sort > 0)
		(void)sprintf(scode, "%d", s->u.f->sort);
	      if (*scode)
		{
		  list_add(a, "sort");
		  list_add(a, scode);
		}

	      list_add(a, "compoundonly");
	      list_add(a, "yes");
	      
	      unsigned char *oids = x_cdp_refs(sl, s);
	      if (oids)
		{
		  list_add(a, "cpd-refs");
		  list_add(a, oids);
		}

	      atts = list2chars(a);
	      ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	      list_free(a, NULL);
	      joxer_ea(&s->mloc, "sl:form", ratts);
	      joxer_eaaa(&s->mloc, "sl:name", NULL);
	      grx_jox(tp->gdl, "g:w");
	      joxer_eeaa(&s->mloc, "sl:name");
	      in_form = 1;
	    }
	  else
	    {
	      List *a = list_create(LIST_SINGLE);
	      const char **atts = NULL;

	      if (!s->u.f->sign->xref)
		ref = "ref";
	      else
		{
		  if (s->u.f->oid)
		    {
		      if (hash_find(xidseen, (uccp)s->u.f->oid))
			ref = "ref";
		      else
			hash_add(xidseen, (uccp)s->u.f->oid, "");
		    }
		}

	      if (s->key)
		{
		  list_add(a, "key");
		  list_add(a, (void*)xmlify(s->key));
		}

	      list_add(a, "n");
	      list_add(a, (void*)xmlify(s->u.f->name));

	      /* One form is the defining instance--it has xml:id==oid */
	      if (s->u.f->sign->xref && s->u.f->sign->formdef == s)
		{
		  list_add(a, (void*)"xml:id");
		  list_add(a, (void*)s->u.f->oid);
		}
	      else
		{
		  list_add(a, (void*)"xml:id");
		  list_add(a, (void*)s->iid);
		  
		  list_add(a, "oid");
		  list_add(a, s->u.f->oid ? (void*)s->u.f->oid : (void*)"");
		}

	      if (ref)
		{
		  list_add(a, "ref");
		  list_add(a, s->u.f->oid ? (void*)s->u.f->oid : (void*)"");
		}

	      list_add(a, "sort");
	      list_add(a, scode);

	      unsigned char *oids = x_cdp_refs(sl, s);
	      if (oids)
		{
		  list_add(a, "cpd-refs");
		  list_add(a, oids);
		}

	      if (s->tp)
		x_tis_atts(a, s->tp, 0);
	      
	      atts = list2chars(a);
	      ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	      list_free(a, NULL);
	      joxer_ea(&s->mloc, "sl:form", ratts);
	      joxer_eaaa(&s->mloc, "sl:name", NULL);
	      grx_jox(tp->gdl, "g:w");
	      joxer_eeaa(&s->mloc, "sl:name");
	      if (s->u.f->aka && list_len(s->u.f->aka))
		sx_w_jx_aka(f, sl, s->u.f->aka);
	      in_form = 1;
	    }
	}
    }
  else if (p == sx_pos_term)
    {
      if (in_form)
	joxer_ee(&s->eloc, "sl:form");
      in_form = 0;
      joxer_ac();
    }
}

static void
sx_w_jx_syss(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->sys)
    {
      struct sl_sys *sp;
      joxer_ao("j:syss");
      for (sp = list_first(ip->sys); sp; sp = list_next(ip->sys))
	{
	  if (sp->subname)
	    ratts = rnvval_aa("x", "name", sp->name, "subname", sp->subname, "token", sp->v, NULL);
	  else
	    ratts = rnvval_aa("x", "name", sp->name, "token", sp->v, NULL);
	  joxer_ea(&ip->mloc, "sl:sys", ratts);
	  if (sp->vv)
	    joxer_ch(&ip->mloc, (ccp)xmlify((uccp)sp->vv));
	  joxer_ee(&ip->mloc, "sl:sys");
	}
      joxer_ac();
    }
}

static void
sx_w_jx_links(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->links)
    {
      struct sl_link *sp;
      joxer_ao("j:links");
      for (sp = list_first(ip->links); sp; sp = list_next(ip->links))
	{
	  List *a = list_create(LIST_SINGLE);
	  list_add(a, "name");
	  list_add(a, (void*)sp->name);
	  list_add(a, "label");
	  list_add(a, (void*)xmlify(sp->label));
	  list_add(a, "url");
	  list_add(a, (void*)xmlify(sp->url));
	  const char **atts = list2chars(a);
	  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	  joxer_ec(&ip->mloc, "sl:link", ratts);
	  list_free(a, NULL);
	}
      joxer_ac();
    }
}

static void
sx_w_jx_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  static int in_value = 0;
  
  if (p == sx_pos_init)
    {
      joxer_eaaa(NULL, "sl:inherited", NULL);
    }
  else if (p == sx_pos_inst)
    {
      if (v->inherited)
	{
	  char scode[32];
	  (void)sprintf(scode, "%d", v->u.v->sort);
	  
	  if (in_value)
	    joxer_ee(&v->mloc, "sl:iv");
	  else
	    in_value = 1;
	  
	  ratts = rnvval_aa("x", "n", v->u.v->name, "sort", scode, NULL);
	  joxer_ea(&v->mloc, "sl:iv", ratts);
	}
    }
  if (p == sx_pos_term)
    {
      if (in_value)
	joxer_ee(NULL, "sl:iv");
      in_value = 0;
      joxer_eeaa(NULL, "sl:inherited");
    }
}

static void
sx_w_jx_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
  if (p == sx_pos_init)
    joxer_ao("j:lists");
  else if (p == sx_pos_inst)
    {
      char scode[32];
      (void)sprintf(scode, "%d", l->u.l->sort);
      ratts = rnvval_aa("x", "n", l->u.l->name, "sort", scode, NULL);
      joxer_ec(&l->mloc, "sl:list", ratts);
    }
  else
    joxer_ac();
}

static void
sx_w_jx_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  struct sl_note *np;
  if (ip && ip->notes)
    {
      joxer_ao("j:notes");
      for (np = list_first(ip->notes); np; np = list_next(ip->notes))
	{
	  const char *xtag = "sl:inote";
	  if (*np->tag == 'n')
	    xtag = "sl:note";
	  else if (*np->tag == 'l')
	    xtag = "sl:lit";
	  joxer_eto(&ip->mloc, xtag, NULL, np->txt);
	}
      joxer_ac();
    }
}

static void
sx_w_jx_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
  static int in_qs = 0;
  if (p == sx_pos_init)
    {
      /* don't open qs until we find a q to avoid <qs/> */
    }
  else if (p == sx_pos_inst)
    {
      char *qv = NULL;
      const char *o = NULL;
      unsigned const char *p = NULL;
      int n = 0;

      p = vi->parent_s ? vi->parent_s->u.s->name : vi->parent_f->u.f->name;
      n += strlen((ccp)vi->u.v->name) + strlen((ccp)p) + 3;
      qv = malloc(n);
      sprintf(qv, "%s(%s)", vi->u.v->name, p);
      o = vi->parent_s ? vi->parent_s->u.s->oid : vi->parent_f->u.f->oid;
      if (!o)
	o = "";
      
      ratts = NULL;
      if (vi->u.v->qvmust)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      else if (vi->u.v->qvsign && vi->parent_s)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      else if (vi->u.v->qvform && vi->parent_f)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      if (ratts && ratts->atts[1])
	{
	  if (!in_qs++)
	    joxer_eaaa(&vi->mloc,"sl:qs", NULL);

	  joxer_ec(&vi->mloc,"sl:q", ratts);
	}
    }
  if (p == sx_pos_term)
    {
      if (in_qs)
	joxer_eeaa(&vi->mloc,"sl:qs");
      in_qs = 0;
    }
}

static const char *
x_tle_tag(enum sx_tle t)
{
  switch (t)
    {
    case sx_tle_componly:
      return "sl:sign";
    case sx_tle_formproxy:
      return "sl:formproxy";
    case sx_tle_lref:
      return "sl:lref";
    case sx_tle_sign:
      return "sl:sign";
    case sx_tle_pcun:
      return "sl:pcun";
    case sx_tle_xsux:
      return "sl:xsux";
    case sx_tle_sref:
      return "sl:sref";
    default:
      return NULL;
    }
  return NULL;
}

static int
x_cpd_only(struct sl_inst *ip)
{
  if (ip->type == 'f' && ip->u.f->compoundonly)
    return 1;
  else if (ip->type == 's' && ip->u.s->type == sx_tle_componly)
    return 1;
  else
    return 0;
}

static unsigned char *
x_cdp_refs(struct sl_signlist *sl, struct sl_inst *s)
{
  struct sl_compound_digest *cdp = NULL;

  if (s->type == 's')
    cdp = hash_find(s->u.s->hcompounds, (uccp)"#digest_by_oid");
  else if (s->type == 'f')
    cdp = hash_find(s->u.f->sign->hcompounds, (uccp)"#digest_by_oid");
  if (cdp)
    {
      unsigned char *oids;
      int len = 0;
      int i;
      for (i = 0; cdp->memb[i]; ++i)
	;
      len = (i * (strlen("o1234567")+1))+1;
      oids = pool_alloc(len, sl->p);
      *oids = '\0';
      for (i = 0; cdp->memb[i]; ++i)
	{
	  strcat((char*)oids, cdp->memb[i]);
	  strcat((char*)oids, " ");
	}
      oids[strlen((ccp)oids)-1] = '\0';
      /*fprintf(stderr, "%s:#digest_by_oid=%s\n", s->u.s->name, oids);*/
      if (oids && *oids)
	return oids;
      else if (x_cpd_only(s))
	mesg_verr(&s->mloc, "@compoundonly entry %s not found in any compounds",
		  s->type == 's' ? s->u.s->name : s->u.f->name);
    }
  else if (x_cpd_only(s))
    mesg_verr(&s->mloc, "@compoundonly entry %s not found in any compounds (no #digest_by_oid)",
	      s->type == 's' ? s->u.s->name : s->u.f->name);
  return NULL;
}

static struct rnvval_atts *
x_tle_atts(struct sl_signlist *sl, struct sl_inst *s)
{
  struct rnvval_atts *ratts = NULL;
  List *a = list_create(LIST_SINGLE);
  const char **atts = NULL;
  static char scode[32];
  char *as_form = NULL;

  list_add(a, "n");
  list_add(a, (void*)xmlify(s->u.s->name));
  
  if (s->u.s->oid)
    {
      list_add(a, "xml:id");
      list_add(a, (void*)s->u.s->oid);
    }

  if (s->key)
    {
      list_add(a, "key");
      list_add(a, (void*)xmlify(s->key));
    }

  if (s->u.s->fake)
    {
      list_add(a, "fake");
      list_add(a, (void*)"1");
    }
    

  if (s->u.s->as_form && list_len(s->u.s->as_form))
    {
      const char *p[list_len(s->u.s->as_form)+1];
      int i = 0;
      struct sl_inst *afp;
      for (afp = list_first(s->u.s->as_form); afp; afp = list_next(s->u.s->as_form))
	p[i++] = afp->iid;
      p[i] = NULL;
      char *fids = charstarstar_concat(p);
      list_add(a, "as-form");      
      list_add(a, (as_form = strdup(fids)));
    }
    
  if (s->u.s->sort > 0)
    (void)sprintf(scode, "%d", s->u.s->sort);
  if (*scode)
    {
      list_add(a, "sort");
      list_add(a, scode);
    }

  if (s->u.s->type == sx_tle_componly)
    {
      list_add(a, "compoundonly");
      list_add(a, "yes");
    }

  unsigned char *oids = x_cdp_refs(sl, s);
  if (oids)
    {
      list_add(a, "cpd-refs");
      list_add(a, oids);
    }
  
  if (s->u.s->moid)
    {
      list_add(a, "moid");
      list_add(a, (void*)s->u.s->moid);
    }
  else if (s->u.s->merge)
    {
      unsigned char *snames = list_to_str(s->u.s->merge);
      unsigned char *oids = sx_oids_of(sl, snames);
      if (oids && strlen((ccp)oids))
	{
	  list_add(a, "merge");
	  list_add(a, oids);
	}
    }
  
  if (s->tp)
    x_tis_atts(a, s->tp, s->u.s->ctotal);
  
  atts = list2chars(a);
  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
  list_free(a, NULL);
  return ratts;
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_jx_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static const char *in_sign = 0;

  if (p == sx_pos_inst)
    {
      if (in_sign)
	{
	  joxer_ee(&s->eloc, in_sign);
	  in_sign = NULL;
	}

      if (s->type == 's')
	{
	  struct sl_token *tp = NULL;
	  tp = hash_find(sl->htoken, s->u.s->name);
	  ratts = x_tle_atts(sl, s);
	  joxer_ea(&s->mloc, (in_sign = x_tle_tag(s->u.s->type)), ratts);	  
	  if (s->u.s->smap)
	    {
	      ratts = rnvval_aa("x", "oid", s->u.s->smoid, NULL);
	      joxer_et(NULL, "sl:smap", ratts, s->u.s->smap);
	    }
	  if (tp && tp->gdl)
	    {
	      joxer_eaaa(&s->mloc, "sl:name", NULL);
	      grx_jox(tp->gdl, "g:w");
	      joxer_eeaa(&s->mloc, "sl:name");
	    }
	  if (s->u.s->aka && list_len(s->u.s->aka))
	    sx_w_jx_aka(f, sl, s->u.s->aka);

#if 0
	  if (s->u.s->nvalues)
	    sx_w_jx_homophone_links(f, sl, s->u.s);
#endif
	}
#if 0
      else if (s->type == 'f')
	{
	  /* xref */
	}
      /* also handle compound-only here */
#endif
    }
  else if (p == sx_pos_term)
    {
      if (in_sign)
	{
	  joxer_ee(&s->eloc, in_sign);
	  in_sign = NULL;
	}
    }
}

static void
sx_w_jx_value(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  static int in_value = 0;

  if (p == sx_pos_init)
    joxer_ao("j:values");
  else if (p == sx_pos_inst)
    {
      if (!v->inherited && !v->u.v->atf)
	{
	  char scode[32];
	  struct sl_token *tp = NULL;
	  (void)sprintf(scode, "%d", v->u.v->sort);
	  tp = hash_find(sl->htoken, v->u.v->name);

	  struct rnvval_atts *ratts = NULL;
	  List *a = list_create(LIST_SINGLE);
	  const char **atts = NULL;

	  list_add(a, "n");
	  list_add(a, (void*)xmlify(v->u.v->name));

	  if (v->parent_f)
	    {
	      list_add(a, "xv");
	      list_add(a, "1");
	    }
	  
	  if (v->u.v->sort > 0)
	    (void)sprintf(scode, "%d", v->u.v->sort);
	  if (*scode)
	    {
	      list_add(a, "sort");
	      list_add(a, scode);
	    }

	  if (v->lang)
	    {
	      list_add(a, "xml:lang");
	      list_add(a, (void*)v->lang);
	    }

	  list_add(a, "iid");
	  list_add(a, (void*)v->iid);

	  if (v->key)
	    {
	      list_add(a, "key");
	      list_add(a, (void*)xmlify(v->key));
	    }
	  else
	    mesg_verr(&v->mloc, "value %s has no key", v->u.v->name);

	  if (!v->valid)
	    {
	      list_add(a, "deprecated");
	      list_add(a, "1");
	    }
	  
	  const char *hid = hash_find(sl->homophone_ids, v->u.v->base);
	  if (hid)
	    {
	      list_add(a, "hid");
	      list_add(a, (void*)hid);
	    }
	  
	  if (v->tp)
	    x_tis_atts(a, v->tp, 0);
	  
	  if (in_value)
	    joxer_ee(&v->mloc, "sl:v");
	  else
	    in_value = 1;
#if 1
	  atts = list2chars(a);
	  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	  list_free(a, NULL);
#else
	  if (v->lang)
	    ratts = rnvval_aa("x", "n", v->u.v->name, "xml:lang", v->lang, "sort", scode, NULL);
	  else
	    ratts = rnvval_aa("x", "n", v->u.v->name, "sort", scode, NULL);
#endif
	  joxer_ea(&v->mloc, "sl:v", ratts);
	  joxer_eaaa(&v->mloc, "sl:name", NULL);
	  grx_jox(tp->gdl, "g:w");
	  joxer_eeaa(&v->mloc, "sl:name");

	  if (v->lp)
	    f->lem(f, sl, v);
	}
    }
  else
    {
      if (in_value)
	joxer_ee(&v->mloc, "sl:v");
      in_value = 0;
      joxer_ac();
    }
}

static void
sx_w_jx_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
  if (up->uname)
    joxer_et(NULL, "sl:uname", NULL, up->uname);
  if (up->uhex || up->useq || up->upua)
    {
      const char *u;
      if (useq_force && up->useq && up->uhex)
	ratts = rnvval_aa("x", "hex", up->uhex, "seq", up->useq, NULL);
      else
	{
	  const char *h = up->uhex;
	  if (!h)
	    h = (up->upua ? up->upua : up->useq);
	  ratts = rnvval_aa("x", "hex", h, NULL);
	}
      if (up->utf8)
	u = (ccp)up->utf8;
      else
	u = "";
      joxer_et(NULL, "sl:ucun", ratts, u);
    }
  if (up->urev)
    joxer_et(NULL, "sl:uage", NULL, up->urev);
}
