%option prefix="cdli2xtf"
%option never-interactive noyywrap yylineno nounput

%{
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "xmlify.h"
#include "cdli2xtf.h"

xtfhead *h = NULL;

int cdli2xtfflextrace = 0;

#define uxmlify(s) xmlify((const unsigned char *)(s))

static void atf_protocol(const char *t);
static void begin_text(const char *t);
static void end_text(void);

%}

%%

^"&"[^\r\n]+/\n	{ begin_text(yytext); }
^"#atf:".*/\n	{ atf_protocol(yytext); }

.	{ }
[ \t\r\n] { }

%%

static void
add_line(const char *t)
{
  xtfline *this;
  if (h->last)
    this = h->last->next = calloc(1, sizeof(xtfline));
  else
    this = h->lines = calloc(1, sizeof(xtfline));
  this->text = strdup(t);
  h->last = this;
}

static void
atf_protocol(const char *t)
{
  add_line(t);
  char *l = strstr(t, "lang ");
  if (l)
    {
      l += 5;
      while (isspace(*l))
	++l;
      h->lang = strdup(l);
      l = h->lang + strlen(h->lang);
      while (l > h->lang && isspace(l[-1]))
	--l;
      if (l)
	*l = '\0';	
    }
}

static void
begin_text(const char *t)
{
  if (h)
    end_text();

  h = calloc(1, sizeof(xtfhead));
  h->P = strdup(t+1);
  char *s = h->P;
  while (!isspace(*s))
    ++s;
  if (*s)
    {
      *s++ = '\0';
      while (isspace(*s))
	++s;
      h->equals = s;
      while (*s)
	++s;
      while (isspace(s[-1]))
	--s;
    }
}

static void
end_text(void)
{
  printf("<x:text xml:id=\"%s\" n=\"%s\"", h->P, uxmlify(h->equals));
  if (h->lang)
    printf(" xml:lang=\"%s\"", h->lang);
  printf(">");
  xtfline *lp;
  for (lp = h->lines; lp; lp = lp->next)
    printf("<x:line>%s</x:line>", uxmlify(lp->text));
  printf("</x:text>");
  free(h->P);
  if (h->lang)
    free(h->lang);
  free(h);
  h = NULL;
}

