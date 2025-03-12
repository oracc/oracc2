/* Save table / thead / td to emit if a tbody is the target of an
 * xfrag ID
 */
#include <oraccsys.h>
#include "px.h"

Memo *m_xft_tr = NULL;
Memo *m_xft_thtd = NULL;
Pool *p_xft = NULL;
static const char **xft_atts(const char **atts);

void
xft_print(struct frag *frag)
{
  printStart(frag, "table", frag->xp->atts);
  printStart(frag, "thead", frag->xp->thead_atts);
  struct xft_tr *trp;
  for (trp = list_first(frag->xp->tr); trp; trp = list_next(frag->xp->tr))
    {
      printStart(frag, "tr", trp->atts);
      struct xft_thtd *tp;
      for (tp = list_first(trp->thtd); tp; tp = list_next(trp->thtd))
	{
	  printStart(frag, tp->name, tp->atts);
	  printText(tp->text, frag->fp);
	  printEnd(frag, tp->name);
	}
      printEnd(frag, "tr");
    }
  printEnd(frag, "thead");
  
  frag->nesting = 0;
}

xft*
xft_table(const char **atts)
{
  if (!m_xft_tr)
    {
      m_xft_tr = memo_init(sizeof(struct xft_tr), 16);
      m_xft_thtd = memo_init(sizeof(struct xft_thtd), 32);
      p_xft = pool_init();
    }
  xft *xp = calloc(1, sizeof(xft));
  xp->atts = xft_atts(atts);
  return xp;
}

static void
xft_free_atts(void *vp)
{
  struct xft_thtd *xp = vp;
  free(xp->atts);
}

void
xft_clear(xft *xp)
{
  struct xft_tr*trp;
  for (trp = list_first(xp->tr); trp; trp = list_next(xp->tr))
    {
      list_free(trp->thtd, xft_free_atts);
      free(trp->atts);
    }
  list_free(xp->tr, NULL);
  free(xp->atts);
  free(xp->thead_atts);
  memo_term(m_xft_tr);
  memo_term(m_xft_thtd);
  pool_term(p_xft);
}

void
xft_thead(xft *xp, const char **atts)
{
  xp->thead_atts = xft_atts(atts);
  xp->tr = list_create(LIST_SINGLE);
}

void
xft_tr(xft *xp, const char **atts)
{
  struct xft_tr *trp = memo_new(m_xft_tr);
  trp->atts = xft_atts(atts);
  trp->thtd = list_create(LIST_SINGLE);
  list_add(xp->tr, trp);
}

void
xft_thtd(xft *xp, const char *name, const char **atts)
{
  struct xft_thtd *thtd = memo_new(m_xft_thtd);
  thtd->name = ('h' == name[1] ? "th" : "td");
  thtd->atts = xft_atts(atts);
  list_add(((struct xft_tr*)list_last(xp->tr))->thtd, thtd);
}

void
xft_thtd_text(xft *xp, const char *t)
{
  struct xft_tr *tr = list_last(xp->tr);
  struct xft_thtd *thtd = list_last(tr->thtd);
  thtd->text = (ccp)pool_copy((uccp)t, p_xft);
}

static const char **
xft_atts(const char **atts)
{
  int n;
  for (n = 0; atts[n]; ++n)
    ;
  const char **cpp = malloc((1+n)*sizeof(const char *));
  for (n = 0; atts[n]; ++n)
    cpp[n] = (ccp)pool_copy((uccp)atts[n], p_xft);
  cpp[n] = NULL;
  return cpp;
}
