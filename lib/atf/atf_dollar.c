void
atf_dollar()
{
  if (transtype)
    {
      lines = trans_dollar(lines);
    }
  else
    {
      s = &lines[0][1];
      while (isspace(*s))
	++s;
      if (doctype == e_composite)
	current = c_attach_point();
      else if (current->level < OBJECT || current->level > COLUMN)
	current = attach_point(COLUMN);
      /* class nonx as MILESTONE because it is supposed to float */
      tmp = elem(e_nonx,NULL,lnum,MILESTONE);
      sprintf(line_id_insertp,"%d", ++line_id);
      setAttr(tmp,a_xml_id,ucc(line_id_buf));
      if (*s == '@' && s[1] == '(')
	{
	  unsigned char *lstart;
	  s += 2;
	  lstart = s;
	  while (*s && *s != ')')
	    ++s;
	  if (*s)
	    {
	      *s++ = '\0';
	      setAttr(tmp,a_label,lstart);
	      setAttr(tmp,a_silent,ucc("1"));
	      check_label(lstart,0,pool_copy(ucc(line_id_buf)));
#if 0
	      while (*s)
		++s;
#endif
	    }
	  else
	    warning("label on $-line lacks closing ')'");
	  while (isspace(*s))
	    ++s;
	}
      else if (dollar_fifo)
	dollar_add((const char*)getAttr(tmp,"xml:id"));
      nonxp = parse_nonx(s);
      if (nonxp)
	{
	  appendAttr(tmp,attr(a_strict,
			      nonxp->strict ? ucc("1") : ucc("0")));
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
		  ++lines;
		  continue;
		}
	      else
		switch (nonxp->scope->type)
		  {
		  case x_object:
		    /* WATCHME: does this need to attach
		       to <object> in the same way as surface
		       nonx's need to attach to <surface>? */
		    nonx_attach = TEXT;
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
		    nonx_attach = SURFACE;
		    break;
		  case x_column:
		    nonx_attach = SURFACE;
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
		    nonx_attach = COLUMN;
		    break;
		  default:
		    nonx_attach = COLUMN;
		    break;
		  }
	    }
	  else
	    {
	      nonx_attach = COLUMN;
	    }
	  if (nonxp->ref)
	    {
	      appendAttr(tmp,attr(a_ref,ucc(nonxp->ref)));
	      switch (nonxp->scope->type)
		{
		case x_image:
		  appendAttr(tmp,attr(a_type, ucc("image")));
		  appendAttr(tmp,attr(a_alt, nonxp->literal));
		  break;
		case x_empty:
		  appendAttr(tmp,attr(a_type, ucc("empty")));
		  break;
		default:
		  appendAttr(tmp,attr(a_scope,
				      nonxp->scope ? ucc(nonxp->scope->str) 
				      : ucc("impression")));
		  break;
		}
	    }
	  else
	    {
	      if (nonxp->extent)
		appendAttr(tmp,attr(a_extent,ucc(nonxp->extent->str)));
	      else if (nonxp->number)
		appendAttr(tmp,attr(a_extent,ucc(nonxp->number)));
	      else if (nonxp->strict)
		warning("extent not found in strict $-line");
	      if (nonxp->scope)
		appendAttr(tmp,attr(a_scope,ucc(nonxp->scope->str)));
	      else if (nonxp->strict)
		appendAttr(tmp,attr(a_scope,ucc("line")));
	      if (nonxp->state)
		appendAttr(tmp,attr(a_state,ucc(nonxp->state->str)));
	      else
		{
		  appendAttr(tmp,attr(a_state,ucc("other")));
		  nonx_attach = COLUMN;
		}
	    }
	  if (nonxp->link)
	    {
	      nonx_link_data(nonxp->link, tmp);
	    }
	  else
	    {
	      if (*nonxp->flags)
		appendAttr(tmp,attr(a_flags,ucc(nonxp->flags)));
	      appendChild(tmp,textNode(*s == '(' ? (s+1) : s));
	    }
	  if (doctype == e_composite)
	    current = c_attach_point();
	  else if (current->level != nonx_attach)
	    current = attach_point(nonx_attach);
	  appendChild(current,tmp);
	}
    }
}
