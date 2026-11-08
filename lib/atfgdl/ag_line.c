#include <oraccsys.h>
#include <lng.h>
#include <atf.h>
#include <atf_bld.h>
#include <atf_line.h>
#include <otf-defs.h>
#include <gdllib.h>

int already_lemmed = 0;
unsigned char *curr_line_tok;
Hash *ag_hpool = NULL;

void
ag_gdl_handoff(Mloc *mp, Node *lnode)
{
  gdl_unicode = (atf_use_ascii() ? 0 : 1);
  if (text_lang)
    line_lang = text_lang;
  else
    line_lang = global_lang;
  gdl_set_lang(lnode->mloc, line_lang->fulltag, line_lang);
  gdl_set_ids(clid, 1);

  wd_list = list_create(LIST_SINGLE);
}

void
ag_line_o_mts(Mloc l, const char *tok)
{
  atf_line_lg(&l);
  atf_line_node(LINE_MTS);
  atf_line_host();
  atf_line_id();
  atf_line_label();
  gdl_cell_count = 0;
  ag_gdl_handoff(lnode->mloc, lnode);
}

void
ag_line_o_bil(Mloc l)
{
  atf_lg_guard();
  atf_line_node(LINE_BIL);
  atf_line_host();
  already_lemmed = 0;
  note_initialize_line();
  atf_xprop(lnode,"type", "bil");
  ag_gdl_handoff(lnode->mloc, lnode);
}

#define ag_common_c() \
  Node *w;	      \
  nonw_found = 0;						\
  for (w = list_first(wd_list); w; w = list_next(wd_list))	\
    gdl_word_attr(w);						\
  if (nonw_found)						\
    wd_list = gdl_rem_nonw(wd_list);				\
  curr_line->wl = gdl_get_word_list();				\
  if (gdl_cell_count > atf_cell_count)				\
    atf_cell_count = gdl_cell_count

void
ag_line_c_mts(Mloc l)
{
  Node *w;
  nonw_found = 0;

  for (w = list_first(wd_list); w; w = list_next(wd_list))
    gdl_word_attr(w);

  if (nonw_found)
    wd_list = gdl_rem_nonw(wd_list);

  curr_line->wl = gdl_get_word_list();
  /* gdl_cell_count is the number of times gdl_cell was
     called, i.e., one less than the number of columns */
  if (gdl_cell_count > atf_cell_count)
    atf_cell_count = gdl_cell_count;
  /* possibly some gdl break/state wrapup? */
}

void
ag_line_c_bil(Mloc l)
{
  ag_common_c();
}

void
ag_line_o_nts(Mloc l)
{
}

void
ag_line_o_gus(Mloc l)
{
}

void
ag_line_o_var(Mloc l)
{
}

void
ag_line_o_lgs(Mloc l)
{
}

void
ag_line_c_nts(Mloc l)
{
}

void
ag_line_c_gus(Mloc l)
{
}

void
ag_line_c_var(Mloc l)
{
}

void
ag_line_c_lgs(Mloc l)
{
}

