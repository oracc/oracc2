#include <oraccsys.h>
#include <xml.h>

#include "bx_bid_lib.c"

/* Limited program to put an XML wrapper around Oracc-format .bib
 * files so the .bib data is easy to integrate into the ood display of
 * .bib records
 */
static char *
find_bid(char **lp, int i)
{
  static char Bbuf[8];
  *Bbuf = '\0';
  while (lp[i])
    {
      char *s = lp[i];
      while (isspace(*s))
	++s;
      if (!strncmp(s, "ids", 3))
	{
	  if ('=' == s[3] || isspace(s[3]))
	    {
	      const char *B = bx_bid_first(s);
	      if (B)
		strncpy(Bbuf,B,7);
	      return Bbuf;
	    }
	}
      else if ('@' == *s) /* start of new entry */
	break;
      ++i;
    }
  return Bbuf;
}

int
main(int argc, char *const*argv)
{
  const char *input = (argv[1] ? argv[1] : "-");
  char **lp = (char**)loadfile_lines3((uccp)input, NULL, NULL);
  char *text2bib = "";
  int i = 0;
  printf("<refs xmlns=\"http:/www.w3.org/1999/html\">");
  while (lp[i])
    {
      char *s = lp[i];
      while (isspace(*s))
	++s;
      if (*s)
	{
	  if ('%' == lp[i][0] && strstr(lp[i], "text2bib"))
	    {
	      text2bib = lp[i];
	      ++text2bib;
	      while (isspace(*text2bib))
		++text2bib;
	      if ('!' == *text2bib)
		text2bib = "";
	    }
	  else
	    {
	      char *bid = find_bid(lp, i);
	      /* This is integrated with ood data via the xml:id so if
		 there's no xml:id there's no point outputting it */
	      if (*bid)
		{
		  printf("<div xml:id=\"%s\">", bid);
		  printf("<h1>.bib data for %s</h1>", bid);
		  if (*text2bib)
		    printf("<p>Originally %s May have been edited.</p>", text2bib);
		  fputs("<pre>",stdout);
		  while (s && *s)
		    {
		      printf("%s\n", xmlify((uccp)s));
		      if ('}' == *s)
			break;
		      else
			s = lp[++i];
		    }
		  fputs("</pre></div>",stdout);
		}
	    }
	}
      ++i;
    }
  puts("</refs>");
}
