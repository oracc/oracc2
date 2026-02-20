#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "atf.h"
#include "atf.tab.h"

int atftrace = 1;
extern int gdl_legacy, gdl_lexical, gdl_unicode;

void
atf_protocol(const char *p)
{
  const char *p2 = NULL;
  extern int curr_lang;
  
  if (strstr(p, "unicode"))
    gdl_unicode = 1;
  else if (strstr(p, "legacy"))
    gdl_legacy = 1;
  else if (strstr(p, "lexical"))
    gdl_lexical = 1;
  if ((p2 = strstr(p, "lang")))
    {
      p2 += 5;
      while (isspace(*p2))
	++p2;
      curr_lang = *p2;
    }
}

Tree *
atf_read(const char *file)
{
  if (file)
    {
      FILE *fp = NULL;
      atf_lex_init(fp, file);
    }
  atf_init();
  return atfyacc();
}

