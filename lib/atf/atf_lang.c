#include <oraccsys.h>
#include <lang.h>
#include "atf.h"
#include "atf_bld.h"

/* atf_lang is "lang %s _%a_" vel sim */
void
atf_lang(Mloc ml, ATF *a, const char *atf_lang)
{
  char lbuf[strlen(atf_lang)], *l = lbuf;
  strcpy(l, atf_lang+4);
  unsigned char *uscore, *altlang = NULL;
  int uscore_logo = 0;
  while (isspace(*l))
    ++l;
      
  if ((uscore = (unsigned char *)strchr((char*)l,'_')))
    {
      unsigned char *l_end = uscore;
      while (isspace(l_end[-1]))
	--l_end;
      *l_end = '\0';
      if (!strncmp((char*)uscore,"_logo",5))
	{
	  /*uscore_logo = 1;*/
	  uscore += 5;
	}
      else
	++uscore;
      while (isspace(*uscore))
	++uscore;
      if (*uscore)
	{
	  altlang = uscore;
	  while (*uscore && !isspace(*uscore) && '_' != *uscore)
	    ++uscore;
	  if (*uscore)
	    *uscore = '\0';
	}
    }

  a->lang = (ccp)pool_copy((uccp)l, atfmp->pool);
  a->altlang = (ccp)pool_copy((uccp)altlang, atfmp->pool);
  Node *np = atf_add("protocol", &ml);
  atf_xprop(np, "type", "atf");
  np->user = atfp;
  atf_input(ml, LT_LANG, a);
  
  char *atsign;
  if ((atsign = strchr(l,'@')))
    {
      warning("specify dialect as in ATF, i.e., say 'nb' not 'akk @NB'\n"
	      "\tSee http://oracc.museum.upenn.edu/doc/builder/XXX");
    }
  if (*l)
    {
      if (!(text_lang = lang_switch(NULL, (char*)l, NULL, ml.file, ml.line)))
	curr_lang_ctxt = text_lang = global_lang;
      if (altlang)
	{
	  struct lang_context *tmp = lang_switch(NULL,(const char*)altlang,NULL,ml.file,ml.line);
	  if (tmp)
	    text_lang->altlang = (char*)altlang;
	}
      if (uscore_logo)
	curr_lang_ctxt->underscore = m_logo;
    }
  else
    warning("atf protocol without argument");
}
