#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <atf.h>
#include <gdl.h>
#include <gdlstate.h>
#include "ag.h"
#include "ag.tab.h"
extern int ag_scan_buffer(const char *base, size_t len);
extern void aglex_destroy(void);

int agtrace = 1;
extern int gdl_legacy, gdl_lexical, gdl_unicode;

Tree *
agyacc(void)
{
  extern int last_aglineno;

  Tree *tp = tree_init();

  (void)tree_root(tp, NS_XTF, "xtf", 1, NULL);

  longtext_set_sep(" ");
  
  ag_set_tree(tp);

  agparse();

  last_aglineno = aglineno;

  aglex_destroy();

  return tp;
}
