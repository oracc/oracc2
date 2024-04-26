#include <oraccsys.h>
#include "../px.h"

/* This function is called when iterating over text one character at a
   time; it needs to return a pointer to the last character output
   because the return pointer is then incremented */
static const char *
pui_atat(Isp *ip, FILE *fp,  const char *at)
{
  const char *atend = strstr(at+2, "@@");
  if (atend)
    {
      atend += 2;
      char buf[(atend-at)+1];
      strncpy(buf, at, atend-at);
      buf[atend-at] = '\0';
      struct idactions *idap = idactions(buf, atend - at);      
      if (idap)
	idap->func(ip, idap, fp);
      else
	while (at < atend)
	  fputs(buf, fp);
      return atend - 1;
    }
  else
    {
      fputc('@', fp);
      return at;
    }
}

/* This routine implements the same escaping as used by oracc2's
   xmlify library routine */
void
pui_output(Isp *ip, FILE *fp, const char *s)
{
  const char *entry = s;
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",fp);
      else if (*s == '>')
	fputs("&gt;",fp);
      else if (*s == '&')
	fputs("&amp;",fp);
#if 0
      else if (*s == '\'')
	fputs("&apos;",fp);
#endif
      else if (*s == '"')
	fputs("&quot;",fp);
      else if (*s == '@' && s[1] == '@')
	s = pui_atat(ip, fp, s);
      else
	fputc(*s,fp);
      ++s;
    }
  const char *errstr = "px: error: ";
  if (!strncmp(entry, errstr, strlen(errstr)))
    free((void*)entry);
}

const char *
pui_filetext(const char *file)
{
  struct puifilestab *p = puifiles(file, strlen(file));
  if (p)
    return *p->text;
  else
    {
      const char *prefix = "px: error: failed to find text for supposed file ";
      char err[strlen(file)+strlen(prefix)+1];
      sprintf(err, "%s%s", prefix, file);
      return strdup(err);
    }
}
