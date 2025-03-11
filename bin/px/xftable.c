/* Save table / thead / td to emit if a tbody is the target of an
 * xfrag ID
 */
#include <oraccsys.h>
#include "px.h"

Memo *m_xft = NULL;
Pool *p_xft = NULL;
static const char **xft_atts(const char **atts);

void
xft_print(struct frag *frag)
{
  printStart(frag, frag->xp->name, frag->xp->atts);
}

xft*
xft_table(const char **atts)
{
  if (!m_xft)
    {
      m_xft = memo_init(sizeof(xft), 32);
      p_xft = pool_init();
    }
  xft *xp = memo_new(m_xft);  
  xp->name = "table";
  xp->atts = xft_atts(atts);
  return xp;
}

static void
xft_free_atts(void *vp)
{
  xft *xp = vp;
  free(xp->atts);
}

void
xft_clear(xft *xp)
{
  memo_term(m_xft);
  pool_term(p_xft);
  list_free(xp->td, xft_free_atts);
  free(xp->atts);
  free(xp->thead->atts);
}

void
xft_thead(xft *xp, const char **atts)
{
  xp->thead = memo_new(m_xft);
  xp->thead->name = "thead";
  xp->thead->atts = xft_atts(atts);
  xp->td = list_create(LIST_SINGLE);
}

void
xft_thtd(xft *xp, const char *name, const char **atts)
{
  xft *thtd = memo_new(m_xft);
  thtd->name = name;
  thtd->atts = xft_atts(atts);
  list_add(xp->td, thtd);
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
