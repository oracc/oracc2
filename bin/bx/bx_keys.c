#include <oraccsys.h>
#include "bx.h"

/* module to create system and project key files used by the inline
 * bib handler to validate keys
 */

extern Hash *keys = NULL;
extern Pool *p = NULL;

static FILE *keyfp;
static int bib_lnum;

static void bx_keys_sub(const char *f);
static void bx_keys_ids(const char *f);
static void bx_keys_key(FILE *bibfp);

void
bx_keys(const char **bibfiles, const char *keyfile)
{
  int i;
  if (keyfile)
    {
      if (!(keyfp = fopen(keyfile, "w")))
	{
	  fprintf(stderr, "can't write keyfile %s", keyfile);
	  exit(1);
	}
    }
  else
    keyfp = stdout;
  
  for (i = 0; bibfiles[i]; ++i)
    bx_keys_sub(bibfiles[i]);

  const char **kk = hash_keys2(hkeys, &i);
  qsort(kk, i, sizeof(const char *), cmpstringp);
  for (i = 0; kk[i]; ++i)
    fprintf(keyfp, "%s\n", kk[i]);

  fclose(keyfp);
}

static void
bx_keys_sub(const char *f)
{
  FILE *bibfp = fopen(f, "r");
  if (!bibfp)
    {
      fprintf(stderr, "can't read .bib file %s\n", f);
      return;
    }
  int ch;
  bib_lnum = 1;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if ('@' == ch)
	{
	  while (EOF != (ch = fgetc(bibfp)))
	    if ('{' == ch)
	      break;
	  bx_keys_key(bibfp);
	}
      else if ('\n' == ch)
	{
	  ++bib_lnum;
	  while (EOF != (ch = fgetc(bibfp)))
	    if (' ' != ch && ' ' != ch)
	      break;
	  if ('@' == ch)
	    ungetc(ch, bibfp);
	  else if ('i' == ch)
	    {
	      if ('d' == (ch = fgetc(bibfp))
		  && 's' == ch = fget(bibfp))
		bx_keys_ids(bibfp);
	    }
	}
    }
}

static void
bx_keys_ids(FILE *bibfp)
{
  while (EOF != (ch = fgetc(bibfp)))
    if ('{' == ch)
      break;
  while (bx_keys_key(bibfp))
    ;
}

static void
bx_keys_key(FILE *bibfp)
{
  char buf[1024];
  int nch = 0;
  int ch;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if (' ' != ch && '\t' != ch && '\n' != ch)
	break;
      if ('\n' == ch)
	++bib_lnum;
    }
  buf[nch++] = ch;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if (strcspn(ch, " \t\n,{}"))
	break;
      else if (nch == 1022)
	{
	  buf[nch] = '\0';
	  fprintf(stderr, "overlong key ignored (max = 1023): %s\n", buf);
	  return 0;
	}
      else
	buf[nch++] = ch;
    }
  if (nch)
    {
      buf[nch] = '\0';
      if (!hash_find(keys, buf))
	hash_add(keys, pool_copy((uccp)buf, p), "");
      else
	fprintf(stderr, "duplicate key %s\n", buf);
      if (ch == '}')
	return 0;
      else
	return 1;
    }
  else
    {
      fprintf(stderr, "unexpected failure to find key");
      return 0;
    }
}
