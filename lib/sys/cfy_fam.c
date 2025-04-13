#include <oraccsys.h>
char *
cfy_fam(const char *project)
{
  const char *csscfyfam = "/css/cuneify.css";
  char buf[strlen(oracc())+strlen(project)+strlen(csscfyfam)+strlen("/www/0")];
  sprintf(buf, "%s/www/%s%s", oracc(), project, csscfyfam);
  if (!access(buf, R_OK))
    {
      strcpy(buf, csscfyfam);
      return strdup(buf);
    }
  else
#if 1
    return NULL;
#else
    sprintf(buf, "/%s%s", project, csscfyfam);
#endif
}
