#include <oraccsys.h>
#include "bx.h"

const char *bib_file;
static int bib_lnum;
static void bx_key_run_one(Bx *bp, const char *fname);

/* Pre for KEY mode means:
 *
 * - make an array of the .bib inputs
 *
 */

void
bx_key_pre(Bx *bp)
{
  if (bp->mode == BX_ICF) /* in ICF mode argv has .xml files; must use -b to name .bib files */
    bxl_bib_files(bp, 0);
  else
    bxl_bib_files(bp, 1);
  bp->keys = hash_create(1024);
}

void
bx_key_run(Bx *bp)
{
  if (bp->files_bib)
    {
      int i;
      for (i = 0; bp->files_bib[i]; ++i)
	bx_key_run_one(bp, bp->files_bib[i]);
    }
}

void
bx_key_out(Bx *bp)
{
  FILE *keyfp = NULL;
  
  if (!bp->quiet)
    mesg_print(stderr); /* should be just duplicate warnings */
  
  if (!bp->no_output)
    {
      if (bp->outfile)
	bp->bibkey_file = bp->outfile;
      else
	bxl_bibkey_file(bp);
      if (!strcmp(bp->bibkey_file, "-"))
	keyfp = stdout;
      else if (!(keyfp = fopen(bp->bibkey_file, "w")))
	fprintf(stderr, "bx: can't write key output %s", bp->bibkey_file);
      if (keyfp)
	{
	  bxl_key_writer(bp, keyfp);
	  if (strcmp(bp->bibkey_file,"-"))
	    fclose(keyfp);
	}
    }
}

static int
bx_key_key(Bx *bp, FILE *bibfp)
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
	  fprintf(stderr, "bx: overlong key ignored (max = 1023): %s\n", buf);
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
      fprintf(stderr, "bx: unexpected failure to find key");
      return 0;
    }
}

static void
bx_key_ids(Bx *bp, FILE *bibfp)
{
  int ch;
  while (EOF != (ch = fgetc(bibfp)))
    if ('{' == ch)
      break;
  while (bx_key_key(bp, bibfp))
    ;
}

static void
bx_key_run_one(Bx *bp, const char *f)
{
  FILE *bibfp = fopen(f, "r");
  if (!bibfp)
    {
      fprintf(stderr, "bx: can't read .bib file %s for key extraction\n", f);
      return;
    }
  else if (verbose)
    fprintf(stderr, "bx: processing %s in KEY mode\n", f);
  
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
	  (void)bx_key_key(bp, bibfp);
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
	    bx_key_ids(bp, bibfp);
	}
    }
}

