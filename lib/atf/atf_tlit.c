#include <oraccsys.h>
#include <gdl.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

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
  extern int gdl_word_mode;
  gdl_word_mode = 1;
  gdl_unicode = 1;
  while (isspace(*s))
    ++s;
  Tree *tp = gdlparse_string(np->mloc, (char*)s);
  tree_graft(np, tp);
  Node *kp;
  for (kp = np->kids; kp; kp = kp->next)
    {
      char buf[strlen(clid)+10];
      sprintf(buf, "%s.%d", clid, word_id_base++);
      gdl_prop_kv(kp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id", pool_copy(buf, atfmp->pool));
      gdl_prop_kv(kp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", "sux");
      if (kp->text)
	gdl_prop_kv(kp, GP_ATTRIBUTE, PG_GDL_INFO, "form", kp->text);
    }
  gdlparse_reset();
}

void
xtlit_parse_inline(unsigned char *line, unsigned char *end, struct node*lnode, 
		  int word_id_base, unsigned char *line_id)
{
#if 0
  /*struct lang_context *line_lang = NULL;*/

  if (verbose > 1)
    fprintf(stderr, "%d\n", lnum);

  nonw_id = 1;
  
  tlit_reinit_inline(with_word_list);
  tokenize_reinit();
  nfields = 0;
  curr_field = NULL;

#if 0
  if (text_lang)
    line_lang = text_lang;
  else
    line_lang = global_lang;
#endif
  
  if (!use_unicode)
    ascii_check(line);

  tokenize(line,end);

  if (!curr_lang || curr_lang->mode != m_alphabetic) /* FIXME: tokcheck should be tuned for alpha vs graphemic */
    {
      if (tokcheck())
	{
	  status = 1;
	  return;
	}
    }
  word_id = 1 * word_id_base;
  xstrcpy(word_id_buf,line_id);
  xstrcat(word_id_buf,".");
  word_id_insertp = word_id_buf + xxstrlen(word_id_buf);
  mutex_state = notoken;

  /* (TOO HARSH)
  if (status)
    return;
  */

  if (fp_forms)
    {
      static int did_first_line = 0;
      if (!did_first_line)
	{
	  ++did_first_line;
	  list_locator(fp_forms);
	}
      else if (!in_split_word)
	{
	  fputc('\n',fp_forms);
	  list_locator(fp_forms);
	}
      /* fprintf(fp_forms,"%d ", lnum); */
    }

  curr_cell = 0;

  if (has_cells())
    process_cells(lnode,line_id);
  else if (has_fields())
    process_fields(lnode,0,last_token,line_id);
  else
    process_words(lnode,0,last_token, with_word_list);

 if (surro_node)
   {
     warning("unclosed surrogate");
     surro_node = NULL;
   }
#endif
}
