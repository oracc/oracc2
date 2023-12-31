#include <list.h>
#include <xml.h>
#include <gdl.h>
#include <signlist.h>
#include <oraccsys.h>
#include <sx.h>
#include <joxer.h>

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
static sx_notes_f sx_w_jx_syss;
static sx_notes_f sx_w_jx_images;
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
  sx_w_jox_fncs.sys = sx_w_jx_syss;
  sx_w_jox_fncs.img = sx_w_jx_images;
  sx_w_jox_fncs.uni = sx_w_jx_unicode;
  sx_w_jox_fncs.qvs = sx_w_jx_qvs;
  sx_w_jox_fncs.fp = NULL;
  sx_w_jox_fncs.fname = NULL;
  return &sx_w_jox_fncs;
}

/* This is the entry point for xml output */
static void
sx_w_jx_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
    if (p == sx_pos_init)
    {
      ratts = rnvval_aa("x", "project", sl->project, "signlist", sl->signlist, NULL);

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
      joxer_ao("j:iheaders");
      if (sl->iheaders)
	{
	  for (i = 0; i < list_len(sl->images); ++i)
	    {
	      const char *o = itoa(i);
	      ratts = rnvval_aa("x",
				"xml:id", sl->iheaders[i].id,
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
			      ratts = rnvval_aa("x", "ref", sl->iheaders[i-1].id, "loc", lp->data, NULL);
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
sx_w_jx_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_form = 0;

  if (p == sx_pos_init)
    joxer_ao("j:forms");
  else if (p == sx_pos_inst)
    {
      const char *id_or_ref = "xml:id";
      
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
  
	      if (s->u.f->oid)
		{
		  list_add(a, "xml:id");
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
	      joxer_ee(&s->eloc, "sl:form");
	      /* don't set in_form here */
	    }
	  else
	    {
	      if (!s->u.f->sign->xref)
		id_or_ref = "ref";
	      else
		{
		  if (s->u.f->oid)
		    {
		      if (hash_find(xidseen, (uccp)s->u.f->oid))
			id_or_ref = "ref";
		      else
			hash_add(xidseen, (uccp)s->u.f->oid, "");
		    }
		}
	      ratts = rnvval_aa("x", "n", s->u.f->name, id_or_ref, s->u.f->oid ? s->u.f->oid : "", "sort", scode, NULL);
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
      joxer_ao("js:syss");
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
    case sx_tle_sref:
      return "sl:sref";
    default:
      return NULL;
    }
  return NULL;
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
      else
	mesg_verr(&s->mloc, "@compoundonly entry %s not found in any compounds",
		  s->type == 's' ? s->u.s->name : s->u.f->name);
    }
  else
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

  list_add(a, "n");
  list_add(a, (void*)xmlify(s->u.s->name));
  
  if (s->u.s->oid)
    {
      list_add(a, "xml:id");
      list_add(a, (void*)s->u.s->oid);
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
      unsigned char *oids = x_cdp_refs(sl, s);
      if (oids)
	{
	  list_add(a, "cpd-refs");
	  list_add(a, oids);
	}
    }
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
	  
	  if (in_value)
	    joxer_ee(&v->mloc, "sl:v");
	  else
	    in_value = 1;
	  if (v->lang)
	    ratts = rnvval_aa("x", "n", v->u.v->name, "xml:lang", v->lang, "sort", scode, NULL);
	  else
	    ratts = rnvval_aa("x", "n", v->u.v->name, "sort", scode, NULL);
	  joxer_ea(&v->mloc, "sl:v", ratts);
	  joxer_eaaa(&v->mloc, "sl:name", NULL);
	  grx_jox(tp->gdl, "g:w");
	  joxer_eeaa(&v->mloc, "sl:name");
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
  if (up->uhex || up->useq)
    {
      const char *u;
      ratts = rnvval_aa("x", "hex", up->uhex ? up->uhex : up->useq, NULL);
      if (up->utf8)
	u = (ccp)up->utf8;
      else
	u = "";
      joxer_et(NULL, "sl:ucun", ratts, u);
    }
  if (up->urev)
    joxer_et(NULL, "sl:uage", NULL, up->urev);
}
