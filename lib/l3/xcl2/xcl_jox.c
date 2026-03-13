#include <oraccsys.h>
#include <joxer.h>
#include <xmlify.h>
#include "xcl.h"
#include "links.h"
#include "form.h"
#include "../prop/l3props.h"
#include "../ilem/ilem_form.h"
#include "../ilem/ilem_para.h"

extern FILE *f_log;

#define XML_DECL "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

/* Set this to 1 to get an XML dump of the XCL tree structure with no phrase skipping */
static int xcl_debug = 0;

int mds_printed = 0;

static const char *psu_list_lang = NULL;

const char *xcl_c_type_str[] =
  {
    "text","discourse","chunk","sentence","clause","phrase","line-var","word-var"
  };
const char *xcl_d_type_str[] =
  {
    "line-start","break","cell-start","cell-end",
    "field-start","field-end",
    "gloss-start","gloss-end",
    "punct",
    "object","surface","column",
    "nonw" , "nonx"
  };

/*
 * NOTE: NO F2 SERIALIZATION CODE SHOULD BE USED HERE EXCEPT f2_serialize
 * ON Form.
 */

void
xj_attr(List *lp, const char *name, const char *value)
{
  if (value && *value)
    list_pair(lp,name, value);
}

void
xj_attr_i(List *lp, const char *name, int value)
{
  if (value)
    list_pair(lp,name, itoa(value));
}

static void
xj_serialize_one_l_sub(struct xcl_l*lp, struct ilem_form *fp)
{
  List *lp = list_create(LIST_SINGLE);
  if (!fp)
    xj_attr(lp,"xml:id",lp->xml_id);
  xj_attr_i(lp,"lnum",fp ? fp->lnum : lp->lnum);
  xj_attr(lp,"ref",lp->ref);
  xj_attr(lp,"ftype",lp->subtype);

  if (lp->cof_head)
    {
      xj_attr(lp, "cof-head", lp->cof_head->xml_id);
      if (fp)
	fp->f2.sig = NULL;
      else if (lp->f)
	lp->f->f2.sig = NULL;
    }
  else if (lp->cof_tails)
    {
      struct xcl_l *tailp;
      fputs(" cof-tails=\"", f_xcl);
      for (tailp = list_first(lp->cof_tails); tailp; tailp = list_next(lp->cof_tails))
	{
	  fputs(tailp->xml_id, f_xcl);
	  if (tailp != list_last(lp->cof_tails))
	    fputc(' ', f_xcl);
	}
      fputs("\"", f_xcl);
    }

  if (fp)
    {
      xj_attr(lp,"inst",fp->sublem);
      if (BIT_ISSET(fp->f2.flags,FORM_FLAGS_INVALID)
	  || BIT_ISSET(fp->f2.flags,FORM_FLAGS_PARTIAL)
	  || BIT_ISSET(fp->f2.flags,FORM_FLAGS_NO_FORM))
	xj_attr(lp, "bad", "yes");
      else
	{
	  xj_attr(lp,"sig",((const char*)fp->f2.sig));
	  xj_attr(lp,"tail-sig",((const char*)fp->f2.tail_sig));
	}
    }
  else
    {
      xj_attr(lp,"inst",lp->inst);
      if (lp->f)
	{
	  if (BIT_ISSET(lp->f->f2.flags,FORM_FLAGS_INVALID))
	    xj_attr(lp, "bad", "yes");
	  else if (BIT_ISSET(lp->f->f2.flags, FORM_FLAGS_NOT_IN_SIGS))
	    {
	      extern const char *phase;
	      extern int cbd_lem_sigs;
	      const char *ophase = phase;
	      phase = "sig";
	      if (lp->f->f2.sig)
		{
		  xj_attr(lp,"newsig",(char *)lp->f->f2.sig);
		  if (cbd_lem_sigs)
		    mesg_vnotice((char*)lp->f->file,lp->f->lnum,"\t%s", lp->f->f2.sig);
		}
	      else if (lp->f->f2.tail_sig)
		{
 #if 0
		  /* You can't generate newsig for tail sigs; if the tail is new, make the whole
		     COF new so that harvest creates the right members */
		  xj_attr(lp,"newsig",(char *)lp->f->f2.tail_sig);
		  if (cbd_lem_sigs)
		    mesg_vnotice((char*)lp->f->file,lp->f->lnum,"\t%s", lp->f->f2.tail_sig);
#endif
		}
	      phase = ophase;
	    }
	  else if (BIT_ISSET(lp->f->f2.flags,FORM_FLAGS_PARTIAL)
	      || BIT_ISSET(lp->f->f2.flags,FORM_FLAGS_NO_FORM))
	    xj_attr(lp, "bad", "yes");
	  else
	    {
	      if (BIT_ISSET(lp->f->f2.flags, FORM_FLAGS_NEW_BY_PROJ)
		       || BIT_ISSET(lp->f->f2.flags, FORM_FLAGS_NEW_BY_LANG))
		{
		  xj_attr(lp,"exosig",(char *)lp->f->f2.sig);
		  xj_attr(lp,"exoprj",(char *)lp->f->f2.exo_project);
		  xj_attr(lp,"exolng",(char *)lp->f->f2.exo_lang);
		}
	      else
		xj_attr(lp,"sig",(char *)lp->f->f2.sig);
	      xj_attr(lp,"tail-sig",((const char*)lp->f->f2.tail_sig));
	    }
	}
    }

  Ratts *ratts = ratts_list2ratts(lp);
  joxer_ea(ilem_loc(fp), "xcl:l", ratts);

  if (lp->f)
    {
      form_serialize_jox(&lp->f->f2);
      if (lp->f->props)
	props_dump_jox(lp->f);
    }

  ilem_para_dump(lp);

  joxer_ee(ilem_form(fp), "xcl:l");
}

static void
xj_serialize_one_l(struct xcl_l*lp)
{
  struct ilem_form *fp = lp->f;

  if (fp && fp->ambig)
    {
      const char *r[3] = { "xml:id", lp->xml_id, NULL };
      joxer_ea(lp->np->mloc, "ll", rnvval_aa_ccpp(r));
      do
	{
	  xj_serialize_one_l_sub(lp, fp);
	  fp = fp->ambig;
	}
      while (fp);
      joxer_ee(p->np->mloc, "ll");
    }
  else
    {
      xj_serialize_one_l_sub(f_xcl, lp, NULL);
    }
}

static int
no_words(struct xcl_c*cp)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	case xcl_node_l:
	  return 0;
	default:
	  break;
	}
    }
  return 1;
}

static int 
singleton_phrase_children(struct xcl_c*cp, int *pi)
{
  int i;
  int nphrases = 0;
  *pi = 0;
  if (!cp->children)
    return 0;
  for (i = 0; i < cp->nchildren; ++i)
    {
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	  if (cp->children[i].c->type == xcl_c_phrase)
	    {
	      if (++nphrases > 1)
		return 0;
	      else
		*pi = i;
	    }
	  break;
	case xcl_node_l:
	  return 0;
	default:
	  break;
	}
    }
  return 1;
}

static void
xj_serialize_m(const unsigned char *key,void*val)
{
  if ('#' != *key)
    {
      const char *r[3] = { "k", key, NULL };
      joxer_et(NULL, "m", rnvval_aa_ccpp(r), val);
    }
}

static void
xj_serialize_one_node(void *vp)
{
  enum xcl_node_types t = ((struct xcl_c *)vp)->node_type;
  /* int need_ll = 0; */
  int i, unwrapping = 0, phrase_index = 0;

#define cp ((struct xcl_c*)vp)
#define dp ((struct xcl_d*)vp)
#define lp ((struct xcl_l*)vp)

  switch (t)
    {
    case xcl_node_c:
      if (cp->type == xcl_c_sentence)
	{ 
	  if (no_words(cp))
	    unwrapping = 1;
	  else
	    {
	      int skipped_any_phrases = 0;
	      int set_cp_subtype = 0;
	      while (cp->nchildren && singleton_phrase_children(cp, &phrase_index))
		{
		  const char *phrase_subtype = cp->children[phrase_index].c->subtype;
		  int skip_phrase = 0;
		  if (!phrase_subtype)
		    phrase_subtype = (const char *)ilem_para_head_label(cp,1);
		  if (phrase_subtype)
		    {
		      if ((set_cp_subtype || !cp->subtype) 
			  && (!phrase_subtype || 'S' == *phrase_subtype))
			{
			  cp->subtype = phrase_subtype;
			  set_cp_subtype = 1;
			  skip_phrase = 1;
			  /* either this was already NULL or we've used it as
			     phrase_subtype */
			  cp->children[phrase_index].c->subtype = NULL;
			}
		    }
		  else
		    skip_phrase = 1;
		  if (skip_phrase)
		    {
		      union xcl_u tmp = cp->children[phrase_index];
		      union xcl_u *free_me = cp->children;
		      union xcl_u *new_c = NULL;
		      int n_new_c = 0, src, dest;

		      n_new_c = cp->nchildren + tmp.c->nchildren - 1;
		      new_c = malloc(n_new_c * sizeof(union xcl_u));
		      for (src = dest = 0; src < phrase_index; )
			new_c[dest++].c = cp->children[src++].c;
		      for (src = 0; src < tmp.c->nchildren; ++src)
			new_c[dest++].c = tmp.c->children[src].c;
		      for (src = phrase_index + 1; src < cp->nchildren; )
			new_c[dest++].c = cp->children[src++].c;
		      cp->nchildren = cp->children_alloced = n_new_c;
		      cp->children = new_c;
#if 0
		      cp->children = tmp.c->children;
		      cp->nchildren = tmp.c->nchildren;
		      cp->children_alloced = tmp.c->children_alloced;
#endif
		      free(free_me);
		      tmp.c->nchildren = tmp.c->children_alloced = 0;
		      tmp.c->children = NULL;
		      ++skipped_any_phrases;
		    }
		  else
		    break;
		}
	      /* if (skipped_any_phrases) */
		goto skipping_phrase;
	    }
	}
      else if (cp->type == xcl_c_phrase && singleton_phrase_children(cp, &phrase_index))
	{
	  unwrapping = 1;
	  if (cp->subtype && !cp->children[phrase_index].c->subtype)
	    cp->children[phrase_index].c->subtype = cp->subtype;
	}
      else
	{
	skipping_phrase:
	  {
	    List *lp = list_create(LIST_SINGLE);
	    xj_attr(lp,"type",xcl_c_type_str[cp->type]);
	    if (cp->type == xcl_c_sentence)
	      xj_attr(lp,"implicit","yes");
	    if (cp->subtype && *cp->subtype)
	      {
		if (cp->type == xcl_c_sentence
		    || cp->type == xcl_c_clause
		    || cp->type == xcl_c_phrase)
		  xj_attr(lp,"tag",cp->subtype);
		else
		  xj_attr(lp,"subtype",cp->subtype);
	      }
	    if (cp->id)
	      xj_attr(lp,"xml:id",cp->id);
	    xj_attr(lp,"label",cp->label);
	    xj_attr(lp,"ref",cp->ref);
	    xj_attr_i(lp,"level",cp->level);
	    list_pair(lp,"b",cp->bracketing_level);

	    Ratts *ratts = ratts_list2ratts(lp);
	    joxer_ea(NULL,"xcl:c", ratts);
	    if (cp->meta && !mds_printed++)
	      {
		const char *r[3] = { "xml:id",
				     (char*)hash_find(cp->meta, (unsigned char *)"#xml:id"),
				     NULL };
		joxer_ea(NULL, "mds", rnvval_aa_ccpp(r));
		hash_exec2(cp->meta,xj_serialize_m);
		joxer_ee(NULL, "mds");
	      }
	  }
	}
      for (i = 0; i < cp->nchildren; ++i)
	{
	  switch (cp->children[i].c->node_type)
	    {
	    case xcl_node_c:
	      xj_serialize_one_node(cp->children[i].c);
	      break;
	    case xcl_node_d:
	      xj_serialize_one_node(cp->children[i].d);
	      break;
	    case xcl_node_l:
	      xj_serialize_one_node(cp->children[i].l);
	      break;
	    }
	}
      if (!unwrapping)
	joxer_ee(NULL, "xcl:c");
      break;
    case xcl_node_d:
      {
	List *lp = list_create(LIST_SINGLE);
	list_pair(lp, "type", xcl_d_type_str[dp->type]);
	if (dp->ref)
	  list_pair(lp,"r",  dp->ref);
	if (dp->subtype)
	  list_pair(lp,"s",  dp->subtype);
	Ratts *ratts = ratts_list2ratts(lp);	
	joxer_ec(NULL, "xcl:d", ratts);
      }
      break;
    case xcl_node_l:
#if 0
      if (lp->f && lp->f->acount)
	{
	  list_pair(lp,"<", lp->xml_id, lp->ref);
	  need_ll = 1;
	}
#endif
      xj_serialize_one_l(f_xcl,lp);
#if 0
      while (need_ll)
	{
	  fprintf(f_xcl,"</ll>");
	  --need_ll;
	}
#endif
      break;
    }

#undef cp
#undef dp
#undef lp
}

static void
xj_serialize_one_psu(const unsigned char*psu)
{
  const char *r[3] = { "lang" , psu_list_lang, NULL };
  joxer_et(NULL, "psu", rnvval_aa_ccpp(r), psu);
}

static void
xj_serialize_psu_list(char *key,List *lp)
{
  psu_list_lang = key;
  list_exec(lp,(list_exec_func*)xj_serialize_one_psu);
}

static void
xj_serialize_psus(FILE *f_xcl,Hash*psus)
{
  static_f_xcl = f_xcl;
  joxer_ea(NULL, "psus", NULL);
  hash_exec2(psus, (hash_exec2_func*)xj_serialize_psu_list);
  joxer_ee(NULL, "psus");
}

void
xcl_jox(struct xcl_context *xc, FILE *f_xcl, int with_xml_decl)
{
  if (!xc || !xc->root)
    return;

  mds_printed = 0;

#if 0
  if (with_xml_decl)
    fprintf(f_xcl, "%s", XML_DECL);
#endif

  List *lp = list_create(LIST_SINGLE);
  if (xc->project)
    list_pair(lp, "p", xc->project);
  if (xc->textid)
    list_pair(lp, "t", xc->textid);
  if (xc->file)
    list_pair(lp, "f", xc->file);
  if (xc->langs)
    list_pair(lp, "l", xc->langs);

  Ratts *ratts = ratts_list2ratts(lp);

  joxer_ea(NULL, "xcl", ratts);

  if (xc->psus->key_count > 0)
    xj_serialize_psus(f_xcl,xc->psus);
  if (xcl_debug)
    xcl_debug_tree(f_log, xc->root);
  xj_serialize_one_node(f_xcl,xc->root);
  if (xc->linkbase)
    xj_links_dump(f_xcl, xc->linkbase);

  joxer_ee(NULL, "xcl");
}

#if 0
void
x2_serialize_links_only(struct xcl_context *xc,FILE *f_xcl)
{
  if (xc->linkbase)
    links_dump(f_xcl, xc->linkbase);
}
#endif
