#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tok_l_sH(void *userData, const char *name, const char **atts)
{
#if 0
  Trun *r = userData;
  
  if (!strcmp(name, "xcl"))
    r->rs.in_xcl = 1;
  else if (r->rs.in_xcl && !strcmp(name, "l"))
    {
      const char *sig = findAttr(atts, "sig");
      if (!*sig)
	sig = findAttr(atts, "exosig");
      if (sig)
	fprintf(r->o, "l\t%s\t%s\t%s\t%s\n",
		findAttr(atts, "ref"), sig, findAttr(atts, "exo_proj"), findAttr(atts, "exo_lang"));
    }
#endif
}

void
tok_l_eH(void *userData, const char *name)
{
}
