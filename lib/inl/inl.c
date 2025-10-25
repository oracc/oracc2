#include <memo.h>
#include <pool.h>
#include "inl.h"

Memo *inl_scan_m = NULL;
Pool *inl_scan_p = NULL;

void
inl_init(void)
{
  if (!inl_scan_m)
    inl_scan_m = memo_init(sizeof(Scan), 256);
  if (!inl_scan_p)
    inl_scan_p = pool_init();
}

void
inl_term(void)
{
  if (inl_scan_m)
    {
      memo_term(inl_scan_m);
      inl_scan_m = NULL;
    }
  if (!inl_scan_p)
    {
      pool_term(inl_scan_p);
      inl_scan_p = NULL;
    }
}

/* scan an inline string and return a Tree whose children are the
 * parsed components of the string
 *
 * arg 's' must be a string that can be chopped up by the inl routines.
 */
Tree *
inl(char *s)
{
  Tree *tp = NULL;
  if (s && *s)
    {
      tp = tree_init();
      tree_ns_default(tp, NS_INL);
      tree_root(tp, NS_INL, "inl", 0, NULL);
      tp->rootless = 1;
      tp->root->text = s;

      (void)inl_nodes(&tp->root, tp->root->text);
    }
  return tp;
}
