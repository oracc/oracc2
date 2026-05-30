#include <sys/unistd.h>
#include <ctype128.h>
#include <xsystem.h>
#include <pool.h>
#include <memo.h>
#include <mesg.h>
#include <gvl.h>
#include "lng.h"

struct lang_context *global_lang = NULL;
struct lang_context *text_lang = NULL;
struct lang_context *curr_lang_ctxt = NULL;
struct lang_context *logo_lang = NULL;

extern int verbose;

/* Could do gvl_setup here */
struct lang_context *
lang_load(struct proj_context *p, struct lang_tag *lt)
{
  struct lang_context *lp = memo_auto(sizeof(struct lang_context));
  lp->utype = N_U_LANG;
  struct langcore *lcp = NULL;
  struct langmode *lmp = NULL;

  if (!(lcp = lang_core(lt->lang,strlen(lt->lang))))
    return NULL;

  lp->tag = lt;
  lp->owner = p;
  lp->core = lcp;

  /* uscore setting not necessary if _ always means 
     language switch */
  lp->uppercase = lp->core->uppercase;
  lp->underscore = lp->core->underscore;

  if (lp->tag->script)
    {
      lmp = lang_mode(lp->tag->script,strlen(lp->tag->script));
      lp->script = lp->tag->script;
    }
  else
    {
      lmp = lang_mode(lp->core->script,strlen(lp->core->script));
      lp->script = lp->core->script;
    }

  if (lmp)
    lp->mode = lmp->mode;
  else
    lp->mode = m_alphabetic;

  /* Autoload osl for script 020 and pcsl for script 900 */
  if (lp->core->sindex == -1)
    {
      if (!strcmp(lp->core->script, "020"))
	gvl_setup("osl", "osl", "020");
      else if (!strcmp(lp->core->script, "900"))
	gvl_setup("pcsl", "pcsl", "900");
    }

  return lp;
}

struct lang_context *
lang_switch(struct lang_context *curr, const char *tag, int *taglenp, 
	    const char *file, int lnum)
{
  struct lang_context *lp = NULL;
  struct lang_tag *lt = NULL;

  if (!tag)
    return NULL;

  if ((lt = langtag_create(curr, tag, taglenp, file, lnum)))
    {
      if (fwhost && !strcmp(fwhost, lt->lang))
	fwhost = NULL;
      if (!curr || strcmp(lt->tag,curr->fulltag))
	{
	  if (!(lp = lang_load(curr ? curr->owner : NULL, lt)))
	    {
	      lp = curr;
	      mesg_vwarning(file,lnum,"%s: unknown core language in language switch",tag);
	    }
	  else
	    {
	      char *script = NULL;
	      lp->fulltag = lt->tag;
	      for (script = (char *)lp->fulltag; *script; ++script)
		{
		  if (script[0] == '-' && isdigit(script[1]))
		    break;
		}
	      if (*script && !strcmp(script+1, lp->core->script))
		*script = '\0';
	      if (script)
		{
		  /* This is not ideal but at least it means that
		     %akk-949 will switch off validation */
		  gvl_switch(-1);
		}
	      else
		{
		  if (!curr || curr->core->sindex != lp->core->sindex)
		    gvl_switch(lp->core->sindex);
		}
	      texttag_register(lp->fulltag);
	    }
	}
      else
	{
	  lp = curr;
	}
    }
  else
    lp = curr;

  return lp;
}
