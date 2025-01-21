#include <signlist.h>
#include <tis.h>
#include <sx.h>

static sx_signlist_f sx_w_a_signlist;
static sx_letter_f sx_w_a_letter;
static sx_group_f sx_w_a_group;
static sx_sign_f sx_w_a_sign;
static sx_form_f sx_w_a_form;
static sx_list_f sx_w_a_list;
static sx_value_f sx_w_a_ivalue;
static sx_value_f sx_w_a_qvs;
static sx_value_f sx_w_a_value;
static sx_notes_f sx_w_a_notes;
static sx_notes_f sx_w_a_links;
static sx_notes_f sx_w_a_syss;
static sx_unicode_f sx_w_a_unicode;

struct sx_functions sx_w_asl_fncs;

struct sx_functions *
sx_w_asl_init(FILE *fp, const char *fname)
{
  sx_w_asl_fncs.sll = sx_w_a_signlist;
  sx_w_asl_fncs.let = sx_w_a_letter;
  sx_w_asl_fncs.grp = sx_w_a_group;
  sx_w_asl_fncs.sgn = sx_w_a_sign;
  sx_w_asl_fncs.lst = sx_w_a_list;
  sx_w_asl_fncs.frm = sx_w_a_form;
  sx_w_asl_fncs.val = sx_w_a_value;
  sx_w_asl_fncs.inh = sx_w_a_ivalue;
  sx_w_asl_fncs.not = sx_w_a_notes;
  sx_w_asl_fncs.lnk = sx_w_a_links;
  sx_w_asl_fncs.sys = sx_w_a_syss;
  sx_w_asl_fncs.uni = sx_w_a_unicode;
  sx_w_asl_fncs.qvs = sx_w_a_qvs;
  sx_w_asl_fncs.fp = fp;
  sx_w_asl_fncs.fname = fname;
  return &sx_w_asl_fncs;
}

static void
sx_w_a_str_list(FILE *fp, const char *tag, List *lp)
{
  unsigned const char *t;
  for (t = list_first(lp); t; t = list_next(lp))
    fprintf(fp, "@%s\t%s\n", tag, t);
}

int
cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static void
sx_w_a_aka_list(FILE *fp, List *a)
{
  Memo_str *m;
  for (m = list_first(a); m; m = list_next(a))
    {
      if (m->user)	
	fprintf(fp, "@aka\t\"%s\"\n", m->s);
      else
	fprintf(fp, "@aka\t%s\n", m->s);
    }
}

/* This is the entry point for asl output */
static void
sx_w_a_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
  if (sx_pos_init == p)
    {
      fprintf(f->fp, "@project %s\n", sl->project);
      fprintf(f->fp, "@signlist %s\n", sl->signlist);
      fprintf(f->fp, "@domain %s\n", sl->domain);
      
      /* Normally no blank line before notes; @signlist is an exception */
      if (sl->notes && list_len(sl->notes->notes))
	fputc('\n', f->fp);
      sx_w_a_notes(f, sl, sl->notes);

      int nn, i;
      const char **n = hash_keys2(sl->listdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)n[i]);
	  fprintf(f->fp, "\n@listdef %s %s\n", n[i], ldp->str);
	  sx_w_a_notes(f, sl, &ldp->inst);
	}

      n = hash_keys2(sl->linkdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_linkdef *sdp = hash_find(sl->linkdefs, (uccp)n[i]);
	  const char *cspace = "", *ctext = "";
	  if (sdp->comment)
	    {
	      cspace = " ";
	      ctext = sdp->comment;
	    }
	  fprintf(f->fp, "\n@linkdef %s%s%s\n", n[i], cspace, ctext);
	  sx_w_a_notes(f, sl, &sdp->inst);
	}

      n = hash_keys2(sl->sysdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_sysdef *sdp = hash_find(sl->sysdefs, (uccp)n[i]);
	  const char *cspace = "", *ctext = "";
	  if (sdp->comment)
	    {
	      cspace = " ";
	      ctext = sdp->comment;
	    }
	  fprintf(f->fp, "\n@sysdef %s%s%s\n", n[i], cspace, ctext);
	  sx_w_a_notes(f, sl, &sdp->inst);
	}

      n = hash_keys2(sl->h_scripts, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_scriptdef *sdp = hash_find(sl->h_scripts, (uccp)n[i]);
	  fprintf(f->fp, "\n@scriptdef %s %s\n", n[i], sdp->sset);
	  /*sx_w_a_notes(f, sl, &sdp->inst);*/
	}      
      
      if (sl->images)
	{
	  Mloc *m;
	  for (m = list_first(sl->images); m; m = list_next(sl->images))
	    fprintf(f->fp, "@images %s\n\n", (char*)m->user);
	}
#if 0
      if (nn)
	fputc('\n', f->fp);
#endif
    }
}

static void
sx_w_a_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
  if (sx_pos_inst == p && !identity_mode)
    fprintf(f->fp, "\n@group\t%s\n", g->name);
}

static void
sx_w_a_letter(struct sx_functions *f, struct sl_signlist *sl, struct sl_letter *l, enum sx_pos_e p)
{
  if (sx_pos_inst == p && !identity_mode)
    fprintf(f->fp, "\n@letter\t%s\n", l->name);
}

static void
sx_w_a_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_form = 0;
  if (sx_pos_inst == p)
    {
      struct tis_data *tip;
      if (sl->h_kdata && (!(tip = hash_find(sl->h_kdata,s->key))))
	return;

      /* When a .tis is read the signs and forms which occur in the
	 keys are checked to see if they have dependencies; if so,
	 those dependencies are added to sl->h_idata with a flag value
	 of -1 in tip->cnt */
      if (tip && tip->cnt ==  SX_IDATA_COMPONLY)
	s->u.f->compoundonly = 1;

      const char *minus = "", *query = "", *ref = "", *refspace = "", *literal = "";
      if (!s->valid)
	minus = "-";
      if (s->query)
	query = "?";
      if (s->literal)
	literal = "=";

      if (in_form)
	{
	  fprintf(f->fp, "@@\n");
	  in_form = 0;
	}

      in_form = 1;
      if (s->u.f->compoundonly)
	{
	  fprintf(f->fp, "@compoundonly %s\n", s->u.f->name);
	}
      else
	{
	  fprintf(f->fp, "@form%s %s%s%s%s%s\n", minus, s->u.f->name, query, literal, refspace, ref);
	  if (s->u.f->oid)
	    fprintf(f->fp, "@oid\t%s\n", s->u.f->oid);
	  if (s->u.f->pname)
	    fprintf(f->fp, "@pname\t%s\n", s->u.f->pname);
	}
      if (s->u.f->aka)
	sx_w_a_aka_list(f->fp, s->u.f->aka);
    }
  else if (sx_pos_term == p)
    {
      if (in_form)
	{
	  fprintf(f->fp, "@@\n");
	  in_form = 0;
	}
    }
}

static void
sx_w_a_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  if (!identity_mode || sl->h_kdata)
    f->val(f, sl, v, p);
}

static void
sx_w_a_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
  if (sx_pos_inst == p && strncmp((ccp)l->u.l->name,"U+",2))
    fprintf(f->fp, "@list\t%s%s\n", l->u.l->name, l->query ? "?" : "");
}

static void
sx_w_a_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->notes)
    {
      struct sl_note *np;
      for (np = list_first(ip->notes); np; np = list_next(ip->notes))
	fprintf(f->fp, "@%s\t%s\n", np->tag, (uccp)np->txt);
    }
}

static void
sx_w_a_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
  if (!identity_mode && sx_pos_inst == p)
    {
      const unsigned char *pname;
      const char *poid, *qvm;
      if (vi->parent_f)
	{
	  pname = vi->parent_f->u.f->name;
	  poid = vi->parent_f->u.f->oid;
	}
      else
	{
	  pname = vi->parent_s->u.s->name;
	  poid = vi->parent_s->u.s->oid;
	}
      if (vi->u.v->qvmust)
	qvm = "must";
      else if (vi->u.v->qvsign)
	qvm = "sign";
      else if (vi->u.v->qvform)
	qvm = "form";
      fprintf(f->fp, "@qv [%s] %s(%s) %s\n", qvm, vi->u.v->name, pname, poid);
    }
}

/** Because this is called when walking groups->signs this can be a sign or form inst
 */
static void
sx_w_a_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  const char *minus = "", *query = "", *literal = "";
  static int in_sign = 0;

  if (sx_pos_inst == p)
    {
      if (!s->valid)
	minus = "-";
      if (s->query)
	query = "?";
      if (s->literal)
	literal = "=";

      if (in_sign)
	{
	  if (s->u.s->type == sx_tle_fcomponly)
	    fprintf(f->fp, "@@\n");
	  else
	    {
	      fprintf(f->fp, "@end sign\n");
	      in_sign = 0;
	    }
	}

      struct tis_data *tip;
      if (sl->h_kdata && s->key && (!(tip = hash_find(sl->h_kdata,s->key))))
	return;

#if 0
      /* When a .tis is read the signs and forms which occur in the
	 keys are checked to see if they have dependeencies; if so,
	 those dependencies are added to sl->h_kdata with a flag value
	 of -1 in tip->cnt */
      if (tip && tip->cnt == (const char *)(uintptr_t)-1 && 's' == s->type)
	s->u.s->type = sx_tle_componly;
#endif

      if ('f' == s->type)
	{
	  struct sl_sign *fs = s->u.f->sign;
	  if (fs->xref)
	    {
	      if (!identity_mode)
		{
		  int o;
		  fprintf(f->fp, "\n@xref%s\t%s%s%s\n", minus, s->u.f->name, query, literal);
		  for (o = 0; o < s->u.f->nowners; ++o)
		    fprintf(f->fp, "@see %s\n", s->u.f->owners_sort[o]->name);
		  fprintf(f->fp, "@end xref\n");
		}
	    }
	}
      else if (s->u.s->type == sx_tle_componly)
	{
	  fprintf(f->fp, "\n@compoundonly\t%s\n", s->u.s->name);
	}
      else if (s->u.s->type == sx_tle_fcomponly)
	{
	  fprintf(f->fp, "\n@compoundonly\t%s\n", s->u.s->name);
	}
      else if (s->u.s->type == sx_tle_lref)
	{
	  const char *goesto = "", *dest = "";
	  struct sl_list *l = hash_find(sl->hlentry, s->u.s->name);
	  if (s->u.s->pname)
	    {
	      goesto = " => ";
	      dest = (ccp)s->u.s->pname;
	    }
	  fprintf(f->fp, "\n@lref\t%s%s%s\n", s->u.s->name, goesto, dest);
	  if (l && l->inst)
	    sx_w_a_notes(f, sl, l->inst);
	}
      else if (s->u.s->type == sx_tle_sref)
	{
	  
	  const char *goesto = "", *dest = "";
	  if (s->u.s->pname)
	    {
	      goesto = " => ";
	      dest = (ccp)s->u.s->pname;
	    }
	  fprintf(f->fp, "\n@sref\t%s%s%s\n", s->u.s->name, goesto, dest);
	}
      else
	{
	  const char *tag = "sign";
	  if (s->u.s->type == sx_tle_pcun)
	    tag = "pcun";
	  else if (s->u.s->type == sx_tle_xsux)
	    tag = "xsux";
	  fprintf(f->fp, "\n@%s%s %s%s%s\n", tag, minus, s->u.s->name, query, literal);
	  if (s->u.s->oid)
	    fprintf(f->fp, "@oid\t%s\n", s->u.s->oid);
	  if (s->u.s->fake)
	    fprintf(f->fp, "@fake\t1\n");
	  if (s->u.s->pname)
	    fprintf(f->fp, "@pname\t%s\n", s->u.s->pname);
	  if (s->u.s->aka)
	    sx_w_a_aka_list(f->fp, s->u.s->aka);
	  if (s->u.s->smap)
	    fprintf(f->fp, "@smap\t%s\n", s->u.s->smap);
	  const uchar *m = hash_find(sl->h_merges, s->u.s->name);
	  if (m)
	    fprintf(f->fp, "@merge\t%s\n", m);
	  if (s->u.s->script)
	    {
	      struct sl_scriptdata *sdp;
	      for (sdp = list_first(s->u.s->script); sdp; sdp = list_next(s->u.s->script))
		{
		  fprintf(f->fp, "@script %s: ", sdp->name);
		  if (sdp->merge)
		    {
		      fprintf(f->fp, "merge %s", sdp->merge);
		      if (sdp->salt || sdp->oivs)
			fputs("; ", f->fp);
		    }
		  if (sdp->salt)
		    {
		      fprintf(f->fp, "salt %s", sdp->salt);
		      if (sdp->oivs)
			fputs("; ", f->fp);
		    }
		  if (sdp->oivs)
		    fprintf(f->fp, "ivs %s", sdp->oivs);
		  fputs(".\n", f->fp);
		}
	    }
	  in_sign = 1;
	}
    }
  else if (sx_pos_term == p)
    {
      if (in_sign && s &&
	  (s->u.s->type == sx_tle_sign || s->u.s->type == sx_tle_pcun || s->u.s->type == sx_tle_xsux))
	{
	  const char *tag = "sign";
	  if (s->u.s->type == sx_tle_pcun)
	    tag = "pcun";
	  else if (s->u.s->type == sx_tle_xsux)
	    tag = "xsux";
	  fprintf(f->fp, "@end %s\n", tag);
	  in_sign = 0;
	}
    }
}

static void
sx_w_a_syss(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->sys)
    {
      struct sl_sys *sp;
      for (sp = list_first(ip->sys); sp; sp = list_next(ip->sys))
	{
	  const char *colon = "";
	  const char *subname = "";
	  if (sp->subname)
	    {
	      colon = ":";
	      subname = sp->subname;
	    }
	  fprintf(f->fp, "@sys\t%s%s%s %s", sp->name, colon, subname, sp->v);
	  if (sp->vv)
	    fprintf(f->fp, " => %s\n", sp->vv);
	  else
	    fputc('\n',f->fp);
	}
    }
}

static void
sx_w_a_links(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->links)
    {
      struct sl_link *sp;
      for (sp = list_first(ip->links); sp; sp = list_next(ip->links))
	fprintf(f->fp, "@link\t%s %s %s\n", sp->name, sp->label, sp->url);
    }
}

static void
sx_w_a_value(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  if (sx_pos_inst == p && (!v->inherited || sl->h_kdata))
    {
      if (sl->h_kdata && (!hash_find(sl->h_kdata,v->key)))
	return;
      const char *minus = "", *query = "", *langpct = "", *lang = "", *langspace = "";
      if (!v->valid)
	minus = "-";
      if (v->query)
	query = "?";
      if (v->lang)
	{
	  langpct = "%";
	  langspace = " ";
	  lang = v->lang;
	}
      fprintf(f->fp, "@v%s\t%s%s%s%s%s\n", minus, langpct, lang, langspace, v->u.v->name, query);
    }
}

static void
sx_w_a_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
  if (up->uname)
    fprintf(f->fp, "@uname\t%s\n", up->uname);
  if (up->uhex)
    fprintf(f->fp, "@list\t%s\n", up->uhex);
  if (up->useq)
    fprintf(f->fp, "@useq\t%s\n", up->useq);
  if (up->upua)
    fprintf(f->fp, "@upua\t%s\n", up->upua);
  if (up->umap)
    fprintf(f->fp, "@umap\t%s\n", up->umap);
  if (up->utf8)
    fprintf(f->fp, "@ucun\t%s\n", up->utf8);
  if (up->unotes)
    sx_w_a_str_list(f->fp, "unote", up->unotes);
  if (up->urev)
    fprintf(f->fp, "@uage\t%s\n", up->urev);
}
