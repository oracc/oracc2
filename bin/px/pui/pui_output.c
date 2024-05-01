#include <oraccsys.h>
#include "../px.h"
#include "pui.h"

/* This function is called when iterating over text one character at a
   time; it needs to return a pointer to the last character output
   because the return pointer is then incremented */
static const char *
pui_atat(Isp *ip, FILE *fp,  const char *at)
{
  const char *atend = strstr(at+2, "@@");
  if (atend)
    {
      const char *a = at+2;
      char buf[atend-at], *b;
      a = at+2;
      b = buf;
      while (a < atend)
	*b++ = *a++;
      *b = '\0';
      if (strstr(buf,"()"))
	{
	  struct atactionstab *atap = atactions(buf, strlen(buf));
	  if (atap)
	    atap->func(ip, fp);
	  else
	    fprintf(fp, "@@%s@@", buf);
	}
      else
	{
	  if (pui_output(ip, fp, pui_filetext(buf)))
	    fprintf(fp, "@@%s@@", buf);
	}
      return atend + 1;
    }
  else
    {
      fputc('@', fp);
      return at;
    }
}

/* This routine does not do XML-escaping--that has to be done in the
   *.xml sources */
int
pui_output(Isp *ip, FILE *fp, const char *s)
{
  const char *errstr = "px: error: ";
  if (!strncmp(s, errstr, strlen(errstr)))
    {
      ip->err = s;
      ip->errx = "";
      return 1;
    }
  const char *end = s + strlen(s);
  if (!strncmp(s, "<x>", 3))
    {
      s += 3;
      end = strrchr(s, '<');
    }
  while (s < end)
    {
      if (*s == '@' && s[1] == '@')
	s = pui_atat(ip, fp, s);
      else
	fputc(*s,fp);
      ++s;
    }
  return 0;
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
