#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "atf.h"
#include "atf.tab.h"
extern int atf_scan_buffer(const char *base, size_t len);

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

Tree *
atf_read_atf(const char *atf, size_t len)
{
  if (!atf_scan_buffer(atf, len))
    {
      fprintf(stderr, "atf_read_atf: atf_scan_buffer failed\n");
      return NULL;
    }
  atf_init();
  return atfyacc();
}
