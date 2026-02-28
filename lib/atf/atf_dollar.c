#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "nonx.h"
#include "otf-defs.h"

void
atf_dollar(Mloc l, char *rest)
{
  if (0/*transtype*/)
    {
      /*lines = trans_dollar(lines);*/
    }
  else
    {
      Block_level nonx_attach = B_bl_top;
      const char *dollar_label = NULL;
      
      char *s = rest;
      while (isspace(*s))
	++s;
      if (*s == '@' && s[1] == '(')
	{
	  char *lstart;
	  s += 2;
	  lstart = s;
	  while (*s && *s != ')')
	    ++s;
	  if (*s)
	    {
	      *s++ = '\0';
	      dollar_label = (ccp)pool_copy((uccp)lstart, atfmp->pool);
	    }
	  else
	    warning("label on $-line lacks closing ')'");
	  while (isspace(*s))
	    ++s;
	}

      Nonx *nonxp = parse_nonx(&l, (unsigned char*)s);
      
      if (nonxp)
	{
	  if (nonxp->strict || nonxp->scope)
	    {
	      if (nonxp->strict
		  && (!nonxp->extent || nonxp->extent->class != x_rule)
		  && (!nonxp->scope || ((nonxp->scope->class != x_impression
					 && nonxp->scope->type != x_dock)
					&& !nonxp->state))
		  && !nonxp->ref)
		{
		  warning("strict $-lines must have scope and state indicators");
		  return;
		}
	      else
		switch (nonxp->scope->type)
		  {
		  case x_object:
		    /* WATCHME: does this need to attach
		       to <object> in the same way as surface
		       nonx's need to attach to <surface>? */
		    nonx_attach = B_TEXT;
		    break;
		  case x_surface:
		    /* if we have:
		       @reverse
		       $ start of reverse missing
		       @column 1
		       the structure breaks if the nonx is
		       attached to <object> -- it's simpler
		       just to attach it to <surface>
		    */
		    nonx_attach = B_SURFACE;
		    break;
		  case x_column:
		    nonx_attach = B_SURFACE;
		    if (nonxp->state)
		      switch (nonxp->state->type)
			{
			case x_broken:
			  /* don't do multiple primes on columns */
			  if (!lninfo.colprimes)
			    ++lninfo.colprimes;
			  lninfo.lineno = 0;
			  lninfo.lineprimes = 1;
			  break;
			case x_effaced:
			case x_illegible:
			case x_traces:
			  if (nonxp->number 
			      && isdigit(*nonxp->number)
			      && !strchr(cc(nonxp->number),'-'))
			    lninfo.colno += strtol(cc(nonxp->number), NULL, 10);
			  else
			    {
			      ++lninfo.colprimes;
			      lninfo.lineno = 0;
			      lninfo.lineprimes = 1;
			    }
			  break;
			default:
			  break;
			}
		    break;
		  case x_line:
		    if (nonxp->state)
		      {
			switch (nonxp->state->type)
			  {
			  case x_broken:
			    ++lninfo.lineprimes;
			    break;
			  case x_effaced:
			  case x_illegible:
			  case x_traces:
			    if (nonxp->number 
				&& isdigit(*nonxp->number)
				&& !strchr(cc(nonxp->number),'-'))
			      lninfo.lineno += strtol(cc(nonxp->number), NULL, 10);
			    else
			      ++lninfo.lineprimes;
			    break;
			  default:
			    break;
			  }
		      }
		    nonx_attach = B_COLUMN;
		    break;
		  default:
		    nonx_attach = B_COLUMN;
		    break;
		  }
	    }
	  else
	    {
	      nonx_attach = B_COLUMN;
	    }

	  if (nonx_attach == B_bl_top)
	    nonx_attach = B_COLUMN;

	  /* Defer Node creation and attaching until after we have typed the $-line */
	  Node *np;
	  set_block_curr(nonx_attach);
	  if (atfp->edoc == EDOC_TRANSLITERATION)
	    np = atf_push("nonx");
	  else
	    np = atf_add("nonx");

	  sprintf(line_id_insertp,"%d", ++line_id);
	  const char *xid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool);
	  atf_xprop(np, "xml:id", xid);
	  atf_xprop(np, "strict", nonxp->strict ? cc("1") : cc("0"));

	  if (dollar_label)
	    {
	      atf_xprop(np, "label", dollar_label);
	      atf_xprop(np,"silent",cc("1"));
	      check_label((uccp)dollar_label,0,(uccp)xid);
	    }
	  else if (dollar_fifo)
	    dollar_add(xid);

	  if (nonxp->ref)
	    {
	      atf_xprop(np, "ref", cc(nonxp->ref));
	      switch (nonxp->scope->type)
		{
		case x_image:
		  atf_xprop(np, "type", cc("image"));
		  atf_xprop(np, "alt", cc(nonxp->literal));
		  break;
		case x_empty:
		  atf_xprop(np, "type", cc("empty"));
		  break;
		default:
		  atf_xprop(np, "scope",
				      nonxp->scope ? cc(nonxp->scope->str) 
				      : cc("impression"));
		  break;
		}
	    }
	  else
	    {
	      if (nonxp->extent)
		atf_xprop(np, "extent", cc(nonxp->extent->str));
	      else if (nonxp->number)
		atf_xprop(np, "extent", cc(nonxp->number));
	      else if (nonxp->strict)
		warning("extent not found in strict $-line");
	      if (nonxp->scope)
		atf_xprop(np, "scope", cc(nonxp->scope->str));
	      else if (nonxp->strict)
		atf_xprop(np, "scope", cc("line"));
	      if (nonxp->state)
		atf_xprop(np, "state", cc(nonxp->state->str));
	      else
		{
		  atf_xprop(np, "state", cc("other"));
		}
	    }
	  if (nonxp->link)
	    {
	      /*nonx_link_data(nonxp->link, tmp)*/; /* this is commented out in atf_nonx.c for now */
	    }
	  else
	    {
	      if (*nonxp->flags)
		atf_xprop(np, "flags", cc(nonxp->flags));
	      np->text = (*s == '(' ? (s+1) : s);
	    }
	}
    }
}
