#include <oraccsys.h>
#include "bx.h"

/* module to create system and project key files used by the inline
 * bib handler to validate keys
 */

extern Hash *keys;
extern Pool *p;

static FILE *keyfp;
const char *bib_file;
static int bib_lnum;

static void bx_keys_sub(const char *f);
static void bx_keys_ids(FILE *bibfp);
static int bx_keys_key(FILE *bibfp);

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

  const char **kk = hash_keys2(keys, &i);
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
  bib_file = f;
  bib_lnum = 1;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if ('@' == ch)
	{
	  while (EOF != (ch = fgetc(bibfp)))
	    if ('{' == ch)
	      break;
	  (void)bx_keys_key(bibfp);
	}
      else if ('\n' == ch)
	{
	  ++bib_lnum;
	  while (EOF != (ch = fgetc(bibfp)))
	    if (' ' != ch && ' ' != ch)
	      break;
	  if ('@' == ch)
	    ungetc(ch, bibfp);
	  else if ('i' == ch
		   && ('d' == (ch = fgetc(bibfp)))
		   && ('s' == (ch = fgetc(bibfp))))
	    bx_keys_ids(bibfp);
	}
    }
}

static void
bx_keys_ids(FILE *bibfp)
{
  int ch;
  while (EOF != (ch = fgetc(bibfp)))
    if ('{' == ch)
      break;
  while (bx_keys_key(bibfp))
    ;
}

static int
bx_keys_key(FILE *bibfp)
{
  char buf[1024];
  int nch = 0;
  int ch;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if (' ' != ch && '\t' != ch && '\n' != ch && ',' != ch)
	break;
      if ('\n' == ch)
	++bib_lnum;
    }
  buf[nch++] = ch;
  while (EOF != (ch = fgetc(bibfp)))
    {
      if (strchr(" \t\n,{}", ch))
	break;
      else if (nch >= 1023)
	{
	  buf[1023] = '\0';
	  fprintf(stderr, "overlong key ignored (max = 1023): %s\n", buf);
	  return 0;
	}
      else
	buf[nch++] = ch;
    }
  if (nch)
    {
      buf[nch] = '\0';
      Mloc *mp = hash_find(keys, (uccp)buf);
      if (!mp)
	{
	  hash_add(keys, pool_copy((uccp)buf, p),
		   mloc_file_line(bib_file, bib_lnum));
	}
      else
	{
	  mesg_verr(mp, "Initial occurrence of %s\n", buf);
	  char mbuf[strlen(bib_file)+strlen(":1000000: duplicate 0")+strlen(buf)];
	  sprintf(mbuf, "%s:%d: duplicate %s\n", bib_file, bib_lnum, buf);
	  mesg_append(mbuf);
	  mesg_status_ignore_one();
	}
      while (' ' == ch || '\t' == ch)
	ch = fgetc(bibfp);
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
