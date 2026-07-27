#include <oraccsys.h>
#include "block.h"
#include "atf.h"

int m_trace = 0;
static int m_object_index = 0;
const char *curr_discourse;

/* strings assigned to type and subtype should be persistent, either
   from the Block struct or pool copied */

void
atf_milestone(Block *bp, char *rest)
{
#if 1
  const char *type = NULL, *subtype = NULL, *text = NULL, *m_n = NULL;
  extern int m_label_col_index;
  char **toks = NULL;
  char *tokx[] = { NULL };
  if (rest)
    toks = vec_from_str(rest, NULL, NULL);
  else
    toks = tokx;

 if (m_trace)
   {
     int i;
     fprintf(stderr, "%s:%d: MILESTONE %s", bp->np->mloc->file, bp->np->mloc->line, bp->bt->name);
     for (i = 0; toks[i]; ++i)
       fprintf(stderr, "::%s", toks[i]);
     fputc('\n', stderr);
   }
 if (toks[0])
   {
     int bt_index = -1;
     if (!strcmp(toks[0], "locator"))
       bt_index = 1;
     else
       bt_index = 0;

     if (toks[bt_index])
       {
	 Blocktok *btokp = blocktok(toks[bt_index],strlen(toks[bt_index]));
	 if (btokp)
	   {
	     const char *sub_n = NULL;
	     type = "locator";
	     subtype = btokp->name;
	     switch (btokp->type)
	       {
	       case B_OBJECT:
		 if (toks[2])
		   {
		     sub_n = toks[2];
		     text = toks[3];
		   }
		 m_label_col_index = 0;
		 if (strcmp(subtype,"tablet") || sub_n)
		   update_mlabel(B_OBJECT, (uccp)(sub_n ? sub_n : type));
		 m_object_index = m_label_col_index;
		 break;
	       case B_SURFACE:
		 if (toks[++bt_index])
		   text = sub_n = toks[bt_index];
		 reset_mlabel();
		 update_mlabel(B_SURFACE,
			       (uccp)(btokp->nano ? btokp->nano : (sub_n ? sub_n : subtype)));
		 break;
	       case B_COLUMN:
		 update_mlabel(B_COLUMN, (uccp)(text = toks[++bt_index]));
		 break;
	       case B_DIVISION:
		 type = "division";
		 m_label_col_index = 0;
		 ++bt_index;
		 if (toks[bt_index])
		   {
		     subtype = toks[bt_index];
		     char *div = NULL;
		     if (!strcmp(toks[bt_index],"segment"))
		       div = "Seg";
		     else if (!strcmp(toks[bt_index],"paragraph"))
		       div = "Par";
		     else if (!strcmp(toks[bt_index],"exemplar"))
		       div = "Ex";
		     if (div)
		       {
			 ++bt_index;
			 if (toks[bt_index])
			   {
			     char buf[strlen(div)+strlen(toks[bt_index])+3];
			     sprintf(buf, "%s.%s,", div, toks[bt_index]);
			     m_n = (ccp)pool_copy((uccp)buf, atfmp->pool);
			     update_mlabel(B_SURFACE, (uccp)m_n);
			   }
			 else
			   mesg_verr(bp->np->mloc, "@division type incomplete (needs section info)");
		       }
		     else
		       {
			 char *tmp = vec_to_str(&toks[1], vec_len(&toks[1]), " ");
			 m_n = (ccp)pool_copy((uccp)tmp, atfmp->pool);
			 free(tmp);
			 update_mlabel(B_SURFACE, (uccp)m_n);
			 subtype = NULL;
		       }
		   }
		 else
		   mesg_verr(bp->np->mloc, "@division incomplete (needs division info)");
		 break;
	       case B_DISCOURSE:
		 type = "discourse";
		 curr_discourse = subtype = btokp->name;
		 if (!strcmp(subtype,"body"))
		   {
		     extern const char *default_discourse_level;
		     default_discourse_level = "preamble";
		   }
		 break;
	       case B_MILESTONE:
		 if (!strcmp("fragment",bp->bt->name) || !strcmp("cfragment",bp->bt->name))
		   {
		     if (toks[0])
		       {
			 type = "locator";
			 subtype = "fragment";
			 if (atfp->edoc == EDOC_COMPOSITE)
			   {
			     label_frag(bp->np,(uccp)toks[0]);
			     update_label(bp->np,etu_none);
			   }
			 else
			   {
			     char *buf = (char*)pool_alloc(strlen(toks[0])+strlen("frg.0"), atfmp->pool);
			     strcpy(buf,"frg.");
			     strcat(buf,toks[0]);
			     m_n = buf;
			     if ('c' == *bp->bt->name)
			       update_mlabel(B_OBJECT,(uccp)buf);
			     else
			       update_mlabel(B_SURFACE,(uccp)buf);
			   }
		       }
		     else
		       mesg_verr(bp->np->mloc, "@fragment/@cfragment must have frag indicator");
		   }
		 else
		   mesg_verr(bp->np->mloc, "unhandled MILESTONE %s", bp->bt->name);
		 break;
	       default:
		 mesg_verr(bp->np->mloc, "unhandled milestone block type %s", bp->bt->name);
		 break;
	       }
	   }
	 else
	   {
	     char *tmp = vec_to_str(&toks[bt_index], vec_len(&toks[bt_index]), " ");
	     subtype = (ccp)pool_copy((uccp)tmp, atfmp->pool);
	     m_label_col_index = 0;
	     free(tmp);
	     update_mlabel(B_SURFACE, (uccp)subtype);
	   }
       }
     else
       {
	   mesg_verr(bp->np->mloc, "@m locator incomplete (needs locator info)");
       }
   }

 if (type)
   {
     atf_xprop(bp->np, "type", type);
     if (subtype)
       atf_xprop(bp->np, "subtype", subtype);
     if (m_n)
       {
	 atf_xprop(bp->np, "n", (ccp)pool_copy((uccp)m_n, atfmp->pool));
	 m_n = NULL;
       }
   }
 if (text)
   bp->np->text = (ccp)pool_copy((uccp)text, atfmp->pool);

#else
  unsigned char *type = l+1;
  unsigned char *subtype = NULL, *m_div_n = NULL;
  struct node *m = elem(e_m,NULL,lnum,MILESTONE);
  extern int m_label_col_index;
  if (type[0] == 'm' && type[1] == '\0')
    {
      struct block_token *blocktokp;
      unsigned char *sub_n = NULL;
      type += 2; /* skip 'm=' */
      l = type;
      while (*l && !isspace(*l))
	++l;
      if (*l)
	*l++ = '\0';
      while (isspace(*l))
	++l;
      if (*l)
	sub_n = l;
      blocktokp = blocktok(cc(type),xxstrlen(type));
      if (blocktokp)
	{
	  switch (blocktokp->etype)
	    {
	    case e_object:
	      subtype = type;
	      type = uc("locator");
	      m_label_col_index = 0;
	      if (strcmp((const char *)subtype,"tablet") || sub_n)
		update_mlabel(e_object, sub_n ? sub_n : type);
	      m_object_index = m_label_col_index;
	      break;
	    case e_surface:
	      subtype = type;
	      type = uc("locator");
	      if (!m_object_index)
		m_label_col_index = 0;
	      update_mlabel(e_surface,
			    blocktokp->nano ? ucc(blocktokp->nano) : (sub_n ? sub_n : subtype));
	      break;
	    case e_column:
	      subtype = type;
	      type = uc("locator");
	      update_mlabel(e_column,l);
	      break;
	    default:
	      break;
	    }
	}
      else if (!xstrcmp("locator",type))
	{
	  subtype = l;
	  type = uc("locator");
	  m_label_col_index = 0;
	  update_mlabel(e_surface,l);
	}
      else if (!xstrcmp("division",type))
	{
	  static unsigned char buf[32];
	  unsigned char *n = l;
	  type = uc("division");
	  m_label_col_index = 0;	      
	  while (*n && !isspace(*n))
	    ++n;
	  if (*n)
	    {
	      *n++ = '\0';
	      while (isspace(*n))
		++n;
	      if (!*n)
		n = NULL;
	    }
	  if (n)
	    {
	      if (!xstrcmp(l,"segment"))
		sprintf((char*)buf,"Seg.%s,",n);
	      else if (!xstrcmp(l,"paragraph"))
		sprintf((char*)buf,"Par.%s,",n);
	      else if (!xstrcmp(l,"exemplar"))
		sprintf((char*)buf,"Ex.%s,",n);
	      else
		sprintf((char*)buf,"%s,",(char*)l);
	      subtype = l;
	      m_div_n = n;
	      update_mlabel(e_surface,buf);
	    }
	  else
	    {
	      subtype = l;
	      update_mlabel(e_surface,l);
	    }
	}
      else if (!xstrcmp("textdesc",type))
	{
	  type = uc("textdesc");
	  if (!strcmp((const char*)sub_n, "-"))
	    *sub_n = '\0';
	  m_div_n = sub_n;
	}
      else if (!xstrcmp("textname",type))
	{
	  type = uc("textdesc");
	  m_div_n = sub_n;
	}
    }
  else
    {
      l += xxstrlen(l);
      if (!xstrcmp(type,"body")
	  || !xstrcmp(type,"catchline")
	  || !xstrcmp(type,"colophon")
	  || !xstrcmp(type,"date")
	  || !xstrcmp(type,"linecount")
	  || !xstrcmp(type,"sealings")
	  || !xstrcmp(type,"signature")
	  || !xstrcmp(type,"summary")
	  || !xstrcmp(type,"witnesses"))
	{
	  subtype = type;
	  /* curr_discourse gets reset later to point to the
	     pool_copy'd version of the subtype */
	  curr_discourse = type = uc("discourse");
	  if (!xstrcmp(subtype,"body"))
	    {
	      extern const char *default_discourse_level;
	      default_discourse_level = "preamble";
	    }
	}
      else if (!xstrcmp("fragment",bp->name) || !xstrcmp("cfragment",bp->name))
	{
	  ++l;
	  while (*l && isspace(*l))
	    ++l;
	  if (l)
	    {
	      unsigned char *end = l + xxstrlen(l);
	      unsigned char buf[32];
	      while (end > l && isspace(end[-1]))
		--end;
	      *end = '\0';
	      type = uc("locator");
	      subtype = uc("fragment");
	      if (doctype != e_composite)
		{
		  label_frag(current,l);
		  update_labels(current,transtype);
		}
	      else
		{
		  if (xxstrlen(l)+5 > 32)
		    warning("fragment label too long (max 26 letters)");
		  else
		    {
		      xstrcpy(buf,"frg.");
		      xstrcat(buf,l);
		      if ('c' == *bp->name)
			update_mlabel(e_enum_top,buf);
		      else
			update_mlabel(e_surface,buf);
		    }
		}
	    }
	  else
	    warning("@fragment must have frag indicator");
	}
    }
  appendAttr(m,attr(a_type,type));
  if (subtype)
    {
      appendAttr(m,attr(a_subtype,subtype));
      if (curr_discourse)
	curr_discourse = getAttr(m,"subtype");
    }
  if (m_div_n)
    {
      appendAttr(m,attr(a_n,m_div_n));
      *m_div_n = '\0';
    }
    
  if (*l)
    appendChild(m,cdata(l));
  return m;
#endif
}

#if 0
	 /* @textdesc and @textname were defined in oxx but are not
	    valid types in xtf.rnc so they have probably never been
	    used and therefore are probably not needed */
     else if (!strcmp(toks[0], "textdesc"))
       {
	 type = "textdesc";
	 if (toks[1])
	   {
	     if (strcmp(toks_n, "-"))
	       {
		 unsigned char *tmp = vec_to_str(&toks[1], vec_len(&toks[1]), " ");
		 m_div_n = pool_copy(tmp, atfmp->pool);
		 free(tmp);
	       }
	   }
	 else
	   mesg_verr(bp->np->mloc, "@textdesc incomplete (needs text description)");
       }
     else if (!strcmp(toks[0], "textname"))
       {
	 type = "textname";
	 if (toks[1])
	   {
	     unsigned char *tmp = vec_to_str(&toks[1], vec_len(&toks[1]), " ");
	     m_div_n = pool_copy(tmp, atfmp->pool);
	     free(tmp);
	   }
	 else
	   mesg_verr(bp->np->mloc, "@textname incomplete (needs text name)");
       }
#endif
