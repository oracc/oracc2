#include <oraccsys.h>
#include "atf.h"
#include "atf.tab.h"
extern int atf_scan_buffer(const char *base, size_t len);
extern void atflex_destroy(void);

int atftrace = 1;
extern int gdl_legacy, gdl_lexical, gdl_unicode;

Tree *
atfyacc(void)
{
  extern int last_atflineno;
  /*curratffile = atffile;*/

  Tree *tp = tree_init();

  (void)tree_root(tp, NS_XTF, "xtf", 1, NULL);

  longtext_set_sep(" ");
  
  atf_set_tree(tp);

  atfparse();

  last_atflineno = atflineno;

  atflex_destroy();

  return tp;
}

#if 0
Tree *
atf_read(const char *file)
{
  if (file)
    {
      FILE *fp = NULL;
      atf_lex_init(fp, file);
    }
  atf_init();
  return atfyacc();
}
#endif

