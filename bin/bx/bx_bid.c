#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bib.h"

static unsigned int last_BID = 0;

/* Scan input until '}' checking for [{, \t\n]B[0-9]{6}[}, \t\n] */
static void
bx_bid_id_BIDs(FILE *fp)
{
  int ch, lastch = '{';
  while (!feof(fp))
    {
      ch = fgetc(fp);
      if ('B' == ch && strchr("{, \t\n", lastch))
	{
	  char bidbuf[8] = { 'B','\0','\0','\0','\0','\0','\0','\0' };
	  int bidlen = 1;
	  while (EOF != (ch = fgetc(fp)))
	    {
	      if (isdigit(ch))
		{
		  if (bidlen < 7)
		    bidbuf[bidlen++] = ch;
		  else
		    {
		      *bidbuf = '\0';
		      break;
		    }
		}
	      else
		{
		  *bidbuf = '\0';
		  break;
		}
	    }
	  if (strlen(bidbuf) == 7)
	    {
	      fprintf(stderr, "bx_bid_id_BIDs: found %s\n", bidbuf);
	    }
	}
      else
	lastch = ch;
    }
}

/* Position the FILE * input at '=' of 'id ='; return feof status */
static int
bx_bid_find_id_line(FILE *fp)
{
  int ch;
  while (EOF != (ch = fgetc(fp)))
    {
      if (!isspace(ch) && 'i' == ch)
	{
	  ch = fgetc(fp);
	  if ('d' == ch)
	    {
	      ch = fgetc(fp);
	      if (isspace(ch) || '=' == ch)
		{
		  while (isspace(ch))
		    ch = fgetc(fp);
		  break;
		}
	      else
		{
		  while (EOF != (ch = fgetc(fp)))
		    if ('\n' == ch)
		      break;
		}
	    }
	}
      else
	{
	  while (EOF != (ch = fgetc(fp)))
	    if ('\n' == ch)
	      break;
	}
    }
  return feof(fp);
}

static void
bx_bid_luB_one(char *fn)
{
  char bib[strlen(oracc())+strlen("/bib/00etc/0") + strlen(fn)];
  sprintf(bib, "%s/bib/00etc/%s", oracc(), fn);
  if (verbose)
    fprintf(stderr, "bx_bid: finding last used B-ID in %s\n", bib);
  FILE *fp = fopen(bib, "r");
  if (!fp)
    {
      perror(bib);
      return;
    }
  else
    {
      while (!bx_bid_find_id_line(fp))
	bx_bid_id_BIDs(fp);
    }
}

static void
bx_bid_last_used_BID(void)
{
  int nbibs;
  char oraccbib[strlen(oracc())+strlen("/bib/00etc0")];
  strcpy(oraccbib, oracc());
  strcat(oraccbib, "/bib/00etc");
  char **bibs = dirarray(oraccbib, &nbibs);
  int i;
  for (i = 0; bibs[i]; ++i)
    bx_bid_luB_one(bibs[i]);
}

void
bx_bid_pre(Bx *bp)
{
  /* scan the Oracc bib files to find last BID */
  bx_bid_last_used_BID();
  
  /* load the .bib file that we are adding BID to */
  bx_ref_pre(bp);
  bx_ref_run(bp);
}

void
bx_bid_run(Bx *bp)
{
  ++last_BID;
}

void
bx_bid_out(Bx *bp)
{
  bx_ref_out(bp);
}
