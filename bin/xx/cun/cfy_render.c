#include <oraccsys.h>
#include "cun.h"

static int cfy_head_printed = 0;

/* dp is the first grapheme of a sequence; peek ahead to see if any further d nodes make up a ligature.
 *
 * If so, move cq's list_next so that it is at the d node that ends the ligature.
 *
 * When we are done, adjust the return d node to reflect breakage and
 * title for the ligature before sending it back.
 *
 * The breakage for a ligature is set to 1 if any member of the
 * ligature is broken.
 */  
static struct d *
cfy_liga(struct d *dp, List *cq)
{
  struct d *dp2 = list_peek(cq);
  if (dp2 && dp2->utf8)
    {
      char ligchk[64]; /* This is what we use for hash lookups */
      char ligbuf[64]; /* This is where we build the return utf8 */
      char lignam[1024];
      int brk = 0;
      int all_missing = 1;

      /* Initialize with leading component; if there is no lig these
	 initializations will be ignored */
      strcpy(ligbuf, dp->utf8);
      strcpy(lignam, dp->form);
      if (all_missing && !dp->gmissing)
	all_missing = 0;
      if (dp->brk)
	brk = 1;
      int nlig = 0;
      /* Initialize the buffer we use to check for ligs */
      strcpy(ligchk, dp->utf8);
      strcat(ligchk, dp2->utf8);
      while (hash_find(lig, (uccp)ligchk))
	{
	  ++nlig;
	  strcat(ligbuf, dp2->utf8);
	  strcat(lignam, "+");
	  strcat(lignam, dp2->form);
	  if (all_missing && !dp->gmissing)
	    all_missing = 0;
	  if (dp->brk)
	    brk = 1;
	  dp = list_next(cq);
	  dp2 = list_peek(cq);
	  if (dp2 && dp2->utf8)
	    strcat(ligchk, dp2->utf8);
	  else
	    break;
	}
      if (nlig)
	{
	  dp->utf8 = strdup(ligbuf);
	  dp->form = strdup(lignam);
	  dp->brk = brk;
	  dp->lig = 1;
	  dp->mis = all_missing;
	}
    }
  return dp;
}

static
int cfy_last_dtype(List *cq)
{
  if (cq->rover->prev)
    return ((struct d *)cq->rover->prev->data)->type;
  else
    return 0;
}

/* Determine the space attribute if any; if there is one, clear the
 * space on the dp node so it doesn't get output again
 *
 * Spaces are all left-facing: when dp->gsp is set a little padding is
 * put on the left of the span; when dp->wsp is set, a larger padding
 * amount is put on the left of the span.
 */
static const char *
cfy_space(struct d *dp)
{
  const char *space = "";
  if (dp->gsp)
    {
      space = " cfy-gsp";
      if (dp->gsp > 2)
	fputs("<wbr/>", outfp);
    }
  else if (dp->wsp)
    {
      fputs("<wbr/>", outfp);
      space = " cfy-wsp";
    }
  if (*space)
    dp->gsp = dp->wsp = 0;
  return space;
}

static const char *
cfy_missing(struct d *dp)
{
  if (dp->lig)
    return dp->mis ? " cfy-mis" : "";
  else
    return dp->gmissing ? " cfy-mis" : "";
}

static void
cfy_ellipsis(struct d *dp, int last)
{
  const char *miss = cfy_missing(dp);
  fprintf(outfp, "<span class=\"roman%s\">%s. . .</span>", miss, (last==CT_ELL) ? " " : "");
}

static void
cfy_render_x(void)
{
  fprintf(outfp, "<span class=\"roman gray\">×</span>");
}

static void
cfy_cun(struct d *dp)
{
  const char *miss = cfy_missing(dp);
  const char *space = cfy_space(dp);

  if (dp->oid)
    fprintf(outfp,
	    "<span id=\"c.%s\" title=\"%s\" "
	    "data-oid=\"%s\" data-asl=\"%s\" onclick=\"cfySL(event)\" oncontextmenu=\"cfyHi(event); return false;\" "
	    "class=\"cfy-cun%s%s\">%s</span>",
	    dp->xid, dp->form, dp->oid, dp->oid[2]=='9'?"pcsl":"osl", miss, space, dp->utf8);
  else
    fprintf(outfp, "<span id=\"c.%s\" title=\"%s\" class=\"cfy-fam cfy-def%s%s\">%s</span>",
	    dp->xid, dp->form, miss, space, dp->utf8);
}

void
cfy_render(void)
{
  if (!cfy_head_printed)
    cfy_head(outfp, xn, cp);


  if (!list_len(cqueue))
    return;
  
  struct d *dp;
  int in_brk = 0;
  for (dp = list_first(cqueue); dp; dp = list_next(cqueue))
    {
      if (CT_GRP == dp->type && dp->utf8)
	dp = cfy_liga_line(dp, cqueue);

      if (dp->brk)
	{
	  if (!in_brk)
	    {
	      /* Ensure any space comes before the break patterning */
	      const char *space = cfy_space(dp);
	      fprintf(outfp, "<span class=\"broken%s\">", space);
	      in_brk = 1;
	    }
	}
      else if (in_brk)
	{
	  /* Ensure that the break patterning ends before any space */
	  fputs("</span>", outfp);
	  in_brk = 0;
	}

      switch (dp->type)
	{
	case CT_ELL:
	  cfy_ellipsis(dp, cfy_last_dtype(cqueue));
	  break;
	case CT_XXX:
	  cfy_x();
	  break;
	case CT_GRP:
	  cfy_cun(dp);
	  break;
	default:
	  fprintf(stderr, "cunx: unknown token in cuneify queue\n");
	  break;
	}
#if 0
      d_free(dp);
#endif
    }
  if (in_brk)
    {
      fputs("</span>", outfp);
      in_brk = 1;
    }
  list_free(cqueue, NULL);
  cqueue = NULL;
}


void
cfy_head(FILE *fp, const char *n, Cun_class *cp)
{
  cfy_head_printed = 1;
  if (!weboutput) /* paradoxically, weboutput skips the html head/body
		     because those are provided by P4 */
    {
      const char *cfyfam = cfy_fam(project);
      fprintf(fp,
	      "<html >"
	      "<head><meta charset=\"utf-8\"/>"
	      "<title>Cuneified %s</title>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/fonts.css\"/>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p4-cuneify.css\"/>",
	      n);
      if (cfyfam)
	fprintf(fp, "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"/>", cfyfam);
      fputs("<script type=\"text/javascript\" src=\"/js/p4.js\">&#160;</script>"
	    "</head><body onload=\"p4Onload()\">", fp);
    }
  fprintf(fp,
	  "<div id=\"p4Cuneify\" "
	  "data-cfy-fnt=\"%s\" data-cfy-mag=\"%s\" data-cfy-scr=\"%s\" data-proj=\"%s\">",
	  cp->fnt, cp->mag, cp->scr, project);
  fprintf(outfp, "<h1 class=\"p3h2 border-top heading\"><span class=\"cfy-generic\">Cuneified </span><span class=\"cfy-specific\">%s</span></h1><table class=\"cfy-table\">", n);
}

void
cfy_foot(FILE *fp)
{
  fprintf(fp, "</table></div>");
  if (!weboutput)
    fprintf(fp, "</body></html>");
}

