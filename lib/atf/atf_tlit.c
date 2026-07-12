#include <oraccsys.h>
#include <gdl.h>
#include <lng.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

/** N.B.: line_xxx nodes are not added to the tree--they are added to
 *  Group which is hosted by an <lg> node.
 *
 *  line_xxx content is added to the line.
 *
 *  The bottom edge of the tree structure is a mixture of <lg> nodes
 *  and <nonl> nodes; $-nodes are not permitted within a sequence of
 *  lines that belongs to an <lg> (not currently enforced).
 */
extern int atfflextrace;
List *curr_words;
struct lang_context *line_lang = NULL;

void
tlit_reinit_inline(void)
{
#if 0
  /* not sure what reinit will look like with the lib/atf implementation */
  mutex_state = square_bracket = notoken;
  g_reinit();
  in_g_surro = w_sparse_lem = 0;
#endif
}

void
tlit_parse_inline(Node *np, const char *s, int word_id_base, unsigned char *line_id)
{
  if (atfflextrace)
    fprintf(stderr, "tlit_parse_inline: begin: %s\n", s);
  extern int gdl_word_mode;
  gdl_word_mode = 1;
  gdl_unicode = 1;
  while (isspace(*s))
    ++s;
  if (text_lang)
    line_lang = text_lang;
  else
    line_lang = global_lang;
  gdl_set_lang(np->mloc, line_lang->fulltag, line_lang);
  gdl_set_ids(clid, word_id_base);
  Tree *tp = gdlparse_string(np->mloc, (char*)s);
  tree_graft(np, tp);
  curr_line->wl = gdl_get_word_list();
  np->user = np->kids;
  gdlparse_reset();
  if (atfflextrace)
    fprintf(stderr, "tlit_parse_inline: end: %s\n", s);
}
