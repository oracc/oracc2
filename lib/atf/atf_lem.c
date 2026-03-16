#include <oraccsys.h>
#include <form.h>
#include <lng.h>
#include <l3props.h>
#include <ilem.h>
#include "atf.h"

#define propxid(x) prop_find_kv(x->props, "xml:id", NULL)->u.k->v
#define proplng(x) prop_find_kv(x->props, "xml:lang", NULL)->u.k->v
#define propfrm(x) prop_find_kv(x->props, "form", NULL)->u.k->v

/* Port of otf/atf/lem_save_form:
 */
/* This routine should not set anything but FORM at the f2 level;
   that is the job of ilem_parse */
struct xcl_ilem*
atf_save_lem(Node *np, const char *lemstr)
#if 0
  const char *ref, const char *lang, 
  const char *formstr, struct lang_context *langcon,
  const char *field
#endif
{
  struct ilem_form *form = memo_new(atfmp->milem_forms);
  extern int curr_cell;
  
  form->f2.owner = form;
  form->ref = (char*)propxid(np);
  
  if (!form->ref || !form->ref[0])
    return NULL;

  form->literal = (char*)pool_copy((uccp)lemstr, atfmp->pool);
  
  const char *lang = proplng(np);
  if (lang)
    {
      form->f2.lang = (unsigned char*)lang;
      form->f2.core = langcore_of(lang);
      if (strstr(lang,"949"))
	  BIT_SET(form->f2.flags,FORM_FLAGS_LEM_BY_NORM);
      form->lang = lang_switch(NULL, lang, NULL, np->mloc->file, np->mloc->line);
    }

  const char *formstr = propfrm(np);  
  if (BIT_ISSET(form->f2.flags,FORM_FLAGS_LEM_BY_NORM))
    {
      form->f2.norm = (unsigned char *)formstr;
      form->f2.form = (const unsigned char *)"*";
    }
  else
    form->f2.form = (unsigned char *)formstr;
  form->file = (char*)np->mloc->file;
  form->lnum = np->mloc->line;

  /* NEED TO INTEGRATE LANG CONTEXT INTO GDL AND STORE CONTEXT IN WORD PROPS */

  /*form->lang = langcon;*/

  /* TODO: ensure that cell/field work in GDL and are accessible via, e.g., np->parent */
#if 0
  if (field)
    {
      /* props_XXX are l3/props functions not std/prop functions */
      props_add_prop(form,
		     uxpool_copy("env",atfmp->pool),
		     uxpool_copy("field",atfmp->pool),
		     uxpool_copy(field,atfmp->pool),
		     NULL,
		     NULL,
		     NULL,
		     -1);
    }
#endif
  
  struct xcl_ilem *xip = memo_new(atfmp->mxis);
  xip->w = np;
  xip->i = form;

  return xip;
}
