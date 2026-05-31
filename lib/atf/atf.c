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

