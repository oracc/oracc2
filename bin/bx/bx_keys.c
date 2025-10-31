#include <oraccsys.h>
#include "bx.h"

/* module to create system and project key files used by the inline
 * bib handler to validate keys
 */

static FILE *keyfp;
const char *bib_file;
static int bib_lnum;

static void bx_keys_sub(Bx *bp, const char *f);
static void bx_keys_ids(Bx *bp, FILE *bibfp);
static int bx_keys_key(Bx *bp, FILE *bibfp);

void
bx_keys(Bx *bp, const char *project, const char **bibfiles)
{
  int i;
  if (bp->outfile)
    {
      if (!strcmp(bp->outfile, "-"))
	keyfp = stdout;
    }
  else if (project)
    {
      char buf[strlen(oracc())+strlen(project)+strlen("/pub/bib-key.txt0")];
      (void)sprintf(buf, "%s/pub/%s/bib-key.txt", oracc(), project);
      bp->outfile = (ccp)pool_copy((uccp)buf, bp->p);
    }
  else
    {
      char buf[strlen(oracc())+strlen("/lib/data/bib-key.txt0")];
      (void)sprintf(buf, "%s/lib/data/bib-key.txt", oracc());
      bp->outfile = (ccp)pool_copy((uccp)buf, bp->p);
    }

  if (!keyfp)
    {
      if (!(keyfp = fopen(bp->outfile, "w")))
	{
	  fprintf(stderr, "can't write key output %s", bp->outfile);
	  exit(1);
	}
    }
  
  for (i = 0; bibfiles[i]; ++i)
    bx_keys_sub(bp, bibfiles[i]);

  const char **kk = hash_keys2(bp->keys, &i);
  qsort(kk, i, sizeof(const char *), cmpstringp);
  for (i = 0; kk[i]; ++i)
    fprintf(keyfp, "%s\n", kk[i]);

  fclose(keyfp);
}

static void
bx_keys_sub(Bx *bp, const char *f)
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
	  (void)bx_keys_key(bp, bibfp);
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
	    bx_keys_ids(bp, bibfp);
	}
    }
}

static void
bx_keys_ids(Bx *bp, FILE *bibfp)
{
  int ch;
  while (EOF != (ch = fgetc(bibfp)))
    if ('{' == ch)
      break;
  while (bx_keys_key(bp, bibfp))
    ;
}

static int
bx_keys_key(Bx *bp, FILE *bibfp)
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
      Mloc *mp = hash_find(bp->keys, (uccp)buf);
      if (!mp)
	{
	  hash_add(bp->keys, pool_copy((uccp)buf, bp->p),
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
