#include <oraccsys.h>
#include "block.h"
#include "atf.h"

int m_trace = 1;

void
atf_milestone(Block *bp, char *rest)
{
#if 1
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
 if (toks[0] && !strcmp(toks[0], "locator"))
   {
   }
 else if (mdiscourse(bp->bt->name, strlen(bp->bt->name)))
   {
     atf_xprop(bp->np, "type", "discourse");
     atf_xprop(bp->np, "subtype", bp->bt->name);
   }
 else
   fprintf(stderr, "%s:%d: unhandled MILESTONE %s",
	   bp->np->mloc->file, bp->np->mloc->line, bp->bt->name);
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
