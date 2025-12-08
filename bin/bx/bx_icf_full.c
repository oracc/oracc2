#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bib.h"

static int printing = 0;

static FILE *ms_fp = NULL;
static char *ms_buf = NULL;
static const char *ms_key = NULL;
static size_t ms_len = 0;

static const char *bxl_allbib_html(Bx *bp);

#define ubp	((Bx*)userData)

/* Read the <p> nodes for needed for @fullcite and store them for
   later use */
static void
bxi_printText(const char *s, FILE *frag_fp)
{
  fputs(s, frag_fp);
}

static void
bxi_printStart(FILE *fp, const char *name, const char **atts)
{
  bxi_printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      const char **ap = atts;
      for (ap = atts; ap[0]; ap += 2)
	fprintf(fp, " %s=\"%s\"",ap[0], ap[1]);
      bxi_printText(*ap++, fp);
    }
  fputc('>', fp);
}

static void
bxi_printEnd(FILE *fp, const char *name)
{
  bxi_printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

#define ptag(x) ('p'==x[0]&&!x[1])

static void
bxi_ei_sH(void *userData, const char *name, const char **atts)
{
  if (printing)
    {
      bxi_printStart(ms_fp, name, atts);
      if (ptag(name))
	++printing;
    }
  else if (ptag(name))
    {
      ms_key = (ccp)findAttr(atts, "data-key");
      if (hash_find(ubp->keys_full, (uccp)ms_key))
	{
	  ms_fp = open_memstream(&ms_buf, &ms_len);
	  printing = 1;
	  bxi_printStart(ms_fp, name, atts);
	}
    }
}

static void
bxi_ei_eH(void *userData, const char *name)
{
  if (printing)
    {
      bxi_printEnd(ms_fp, name);
      if (ptag(name))
	--printing;
      if (!printing)
	{
	  fclose(ms_fp);
	  ms_fp = NULL;
	  hash_add(ubp->keys_full, (uccp)ms_key, pool_copy((uccp)ms_buf, ubp->p));
	  free(ms_buf);
	  ms_key = ms_buf = NULL;
	}
    }
}

void
bx_icf_full(Bx *bp)
{
  char const *fnlist[2] = { 0,0 };
  fnlist[0] = bxl_allbib_html(bp);
  runexpatNSuD(i_list, fnlist, bxi_ei_sH, bxi_ei_eH, NULL, bp);
}

static const char *
bxl_allbib_html(Bx *bp)
{
  if (bp->project)
    {
      char buf[strlen(oracc())+strlen(bp->project)+strlen("/www//allbib.html0")];
      (void)sprintf(buf, "%s/www/%s/allbib.html", oracc(), bp->project);
      if (!access(buf, R_OK))
	bp->allbib_html = (ccp)pool_copy((uccp)buf, bp->p);
    }

  if (!bp->allbib_html)
    {
      char buf[strlen(oracc())+strlen("/www/bib/allbib.html0")];
      (void)sprintf(buf, "%s/www/bib/allbib.html", oracc());
      bp->allbib_html = (ccp)pool_copy((uccp)buf, bp->p);
    }

  return bp->allbib_html;
}

