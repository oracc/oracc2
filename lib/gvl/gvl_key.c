#include <oraccsys.h>
#include <gdl.h>

uccp
gvl_key(gvl_g *gbgp, Pool *p)
{
  if (gbgp && gbgp->oid)
    {
      char buf[strlen((ccp)gbgp->orig)+14];
      sprintf(buf, "%s.%s;fvp", gbgp->oid, gbgp->orig);
      const unsigned char *fvp = gvl_lookup((uccp)buf);
      if (!fvp)
        {
          sprintf(buf, "%s;default", gbgp->oid);
          fvp = gvl_lookup((uccp)buf);
          if (fvp)
            sprintf(buf, "%s.", gbgp->oid);
        }
      if (fvp)
        {
          char key[strlen((ccp)buf)+10];
          sprintf(key, "%s.%s", fvp, buf);
          char *s = strchr(key, ';');
          if (s)
            *s = '\0';
          return pool_copy((uccp)key, p);
        }
      else
        {
          char key[strlen((ccp)gbgp->orig)+14];
          if (gbgp->type)
            sprintf(key, "%s..%s", gbgp->oid, (*gbgp->type=='v' ? (ccp)gbgp->orig : ""));
          else
            sprintf(key, "%s..", gbgp->oid);
          return pool_copy((uccp)key, p);
        }
    }
  else
    return NULL;
}
