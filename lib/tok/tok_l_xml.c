#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

const char *curr_ref;

void
tok_l_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if (!strcmp(name, "xcl"))
    r->rs.in_xcl = 1;
  else if (r->rs.in_xcl)
    {
      if (!strcmp(name, "l"))
	{
	  const char *sig = findAttr(atts, "sig");
	  if (!*sig)
	    sig = findAttr(atts, "exosig");
	  if (*sig)
	    fprintf(r->o, "l\t%s\t%s\t%s\t%s\n",
		    findAttr(atts, "ref"), sig, findAttr(atts, "exo_proj"), findAttr(atts, "exo_lang"));
	  else
	    curr_ref = strdup(findAttr(atts, "ref"));
	}
      else if (!strcmp(name, "xff:f"))
	{
	  const char *pos = findAttr(atts, "pos");
	  if (!strcmp(pos, "n"))
	    {
	      fprintf(r->o, "l\t%s\t%s=%s\t\n",
		      curr_ref, findAttr(atts, "form"), pos);
	    }
	}
      else if (!strcmp(name, "d"))
	{
	  const char *type = findAttr(atts, "type");
	  const char *subtype = findAttr(atts, "subtype");
	  if (!subtype || !*subtype)
	    subtype = "#none";
	  if (!strcmp(type, "field-begin"))
	    fprintf(r->o, "D.sof\t%s\n", subtype);
	  else if (!strcmp(type, "field-end"))
	    fprintf(r->o, "D.eof\t%s\n", subtype);
	}
    }
}

void
tok_l_eH(void *userData, const char *name)
{
  Trun *r = userData;
  if (r->rs.in_xcl)
    {
      if (!strcmp(name, "l"))
	{
	  free((void*)curr_ref);
	  curr_ref = NULL;
	}
      else if (!strcmp(name, "xcl"))
	r->rs.in_xcl = 0;
    }
}
