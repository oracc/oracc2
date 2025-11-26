#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bib.h"

static unsigned int last_BID = 0;

const char *BID_STOPS = "{}, \t\n";

/* Scan input until '}' checking for [{, \t\n]B[0-9]{6}[}, \t\n] */
static void
bx_bid_BIDs(FILE *fp)
{
  int ch, lastch = '{';
  /* we are called after '=' */
  while (EOF != (ch = fgetc(fp)))
    if ('{' == ch)
      break;
  while (!feof(fp))
    {
      ch = fgetc(fp);
      if ('B' == ch && strchr(BID_STOPS, lastch))
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
		  if (!strchr(BID_STOPS, ch))
		    *bidbuf = '\0';
		  break;
		}
	    }
	  if (strlen(bidbuf) == 7)
	    {
	      if (verbose)
		fprintf(stderr, "bx_bid_id_BIDs: found %s\n", bidbuf);
	      unsigned long bval = strtoul(bidbuf+1,NULL,10);
	      if (bval > last_BID)
		last_BID = (unsigned int)bval;
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
      /* top of loop is start of line */
      /* read until first non-space */
      if (!isspace(ch))
	{
	  if ('i' == ch)
	    {
	      ch = fgetc(fp);
	      if ('d' == ch)
		{
		  ch = fgetc(fp);
		  if ('s' == ch)
		    {
		      ch = fgetc(fp);
		      if (isspace(ch) || '=' == ch)
			{
			  while (isspace(ch))
			    ch = fgetc(fp);
			  break; /* breaks outer loop */
			}
		    }
		}
	    }
	  /* skip to eol */
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
	bx_bid_BIDs(fp);
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
  if (verbose)
    fprintf(stderr, "bx_bid_pre: last_BID = %u\n", last_BID);
  
  /* load the .bib file that we are adding BID to */
  bx_ref_pre(bp);
  bx_ref_run(bp);
}

const char *
bx_bid_first(const char *ids)
{
  const char *B, *s = ids;
  
  while ((B = strchr(s, 'B')))
    {
      if (B == s || strchr(BID_STOPS, B[-1]))
	{
	  const char *b = B;
	  while (isdigit(b[1]))
	    ++b;
	  if (!b[1] || strchr(BID_STOPS, b[1]))
	    return B;
	}
    }
  return NULL;
}

void
bx_bid_add_bid(Bx *bp, Bibentry *ep)
{
  const char *x = (ep->fields[f_ids] ? ep->fields[f_ids]->data : "");
  char buf[strlen(x)+strlen("0 , B123456")];
  if (*x)
    strcpy(buf, " , ");
  sprintf(buf + strlen(x), "B%06u", ++last_BID);
  if (!ep->fields[f_ids])
    {
      ep->fields[f_ids] = memo_new(bp->m_bibfield);
      ep->fields[f_ids]->name = "ids";
    }
  ep->fields[f_ids]->data = (ccp)pool_copy((uccp)buf, bp->p);
}

void
bx_bid_run(Bx *bp)
{
  Bibentry *ep;
  for (ep = list_first(bp->entries); ep; ep = list_next(bp->entries))
    {
      if (bp->bid_mode != BX_BID_IDENTITY
	  && (!ep->fields[f_ids] || !bx_bid_first(ep->fields[f_ids]->data)))
	bx_bid_add_bid(bp, ep);
    }
}

void
bx_bid_out(Bx *bp)
{
  bx_ref_out(bp);
}
