#ifndef GDLLIB_H_
#define GDLLIB_H_

#include <tree.h>
#include <pool.h>
#include <bits.h>
#include "gdltype.h"

/* Definitions and variables that don't need to be local to the GDL
   parser instance */

extern int gdl_no_xml_ids;
extern int gdltrace, gdl_legacy, gdl_legacy_hash, gdl_orig_mode, gdl_word_mode, gvl_no_mesg_add;
extern int gdl_flex_debug, gdldebug;

/* Applications that can call gdl in multiple contexts (like ax) need
   to manage a stack of Tree* with tcurr set to the last item in the
   stack so that the following defs can work on the tcurr */

extern Tree *tcurr;
#define tcurrG ((GDL*)(tcurr->user))

#define break_stack	(tcurrG->break_stack)
#define c_dangling_gps	(tcurrG->c_dangling_gps)
#define c_delim_sentinel	(tcurrG->c_delim_sentinel)
#define c_explicit_gps	(tcurrG->c_explicit_gps)
#define c_implicit_gps	(tcurrG->c_implicit_gps)
#define c_implicit_times_reset	(tcurrG->c_implicit_times_reset)
#define c_last_explicit_group_node	(tcurrG->c_last_explicit_group_node)
#define c_last_implicit_delim	(tcurrG->c_last_implicit_delim)
#define c_processing	(tcurrG->c_processing)
#define curr_field	(tcurrG->curr_field)
#define curr_lang	(tcurrG->curr_lang)
#define curr_sur_id	(tcurrG->curr_sur_id)
#define deep_parse	(tcurrG->deep_parse)
#define dst	(tcurrG->dst)
#define g_literal_flag	(tcurrG->g_literal_flag)
#define g_logoforce_flag	(tcurrG->g_logoforce_flag)
#define gdl_ascii	(tcurrG->gdl_ascii)
#define gdl_bilingual	(tcurrG->gdl_bilingual)
#define gdl_break_pending	(tcurrG->gdl_break_pending)
#define gdl_cell_count	(tcurrG->gdl_cell_count)
#define gdl_corrq	(tcurrG->gdl_corrq)
#define gdl_group_attach	(tcurrG->gdl_group_attach)
#define gdl_legacy	(tcurrG->gdl_legacy)
#define gdl_legacy_o	(tcurrG->gdl_legacy_o)
#define gdl_legacy_c	(tcurrG->gdl_legacy_c)
#define gdl_legacy_accented	(tcurrG->gdl_legacy_accented)
#define gdl_legacy_hash	(tcurrG->gdl_legacy_hash)
#define gdl_legacy_pending	(tcurrG->gdl_legacy_pending)
#define gdl_legacy_prop	(tcurrG->gdl_legacy_prop)
#define gdl_lexical	(tcurrG->gdl_lexical)
#define gdl_line_id	(tcurrG->gdl_line_id)
#define gdl_orig_mode	(tcurrG->gdl_orig_mode)
#define gdl_pending_varo	(tcurrG->gdl_pending_varo)
#define gdl_post_det_gp_attach	(tcurrG->gdl_post_det_gp_attach)
#define gdl_recycled_word	(tcurrG->gdl_recycled_word)
#define gdl_state_pending	(tcurrG->gdl_state_pending)
#define gdl_unicode	(tcurrG->gdl_unicode)
#define gdl_word_id	(tcurrG->gdl_word_id)
#define gdlpool	(tcurrG->gdlpool)
#define gid_insertp	(tcurrG->gid_insertp)
#define glosstck	(tcurrG->glosstck)
#define grapheme_id	(tcurrG->grapheme_id)
#define lang_stack	(tcurrG->lang_stack)
#define lgp	(tcurrG->lgp)
#define lst	(tcurrG->lst)
#define mgstck	(tcurrG->mgstck)
#define mlstck	(tcurrG->mlstck)
#define modq	(tcurrG->modq)
#define nonw_found	(tcurrG->nonw_found)
#define pst	(tcurrG->pst)
#define rst	(tcurrG->rst)
#define sstck	(tcurrG->sstck)
#define state_stack	(tcurrG->state_stack)
#define wd_list	(tcurrG->wd_list)
#define wid_base	(tcurrG->wid_base)
#define word_lang_tag	(tcurrG->word_lang_tag)

/* Funcs and defines that are safe when an application hosts multiple
   gdl instances given the above tcurrG definitions */

#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
#define gdl_break_peek() stck_peek(break_stack)
#define gdl_break_pop() stck_pop(break_stack)
#define gdl_break_push(x) stck_push(break_stack,x)
#define gdl_state_peek() stck_peek(state_stack)
#define gdl_state_pop() stck_pop(state_stack)
#define gdl_state_push(x) stck_push(state_stack,x)

#define gs_is(s,x) (bit_get((s),(x)))

#define ps_on(x) (bit_set(pst,(x)))
#define ps_no(x) (bit_off(pst,(x)))

#define rs_on(x) (bit_set(rst,(x)))
#define rs_no(x) (bit_off(rst,(x)))

#define fs_on(x) (bit_set(fst,(x)))
#define fs_no(x) (bit_off(fst,(x)))

#define wordnode(w_)	(strlen(w_->name)==3&&'w'==(w_->name[2]))

extern gdl_grapheme_type_p gdl_grapheme_type;
extern gdl_delim_p gdl_delim;
extern gdl_graph_node_p gdl_graph_node;

extern struct mapi *scripttok(const char *k, int len);
extern struct lang_context *gdl_lang_context;

extern Gstck*gstck_new(int i);

extern void gdl_pop_wd_list(void *vp);

extern void gdl_init(void);
extern void gdl_term(void);

extern void gdl_lex_init(const char *file, int line);
extern Tree*gdl_literal(Mloc *m, char *s);
extern void gdl_prefix(Tree *ytp, unsigned const char *p);
extern void gdl_setup_buffer(char *buf);
extern void gdl_set_tree(Tree *tp);

extern void gdlxml_setup(void);
extern void gdlxml_rnv_setup(void);
extern void gdl_xml_handler(Node *np, void *xhp);

extern Node *gdl_pop(Tree *ytp, const char *s);
extern Node *gdl_push(Tree *ytp, const char *s);
extern Node *gdl_push_l(Mloc *mlp, Tree *ytp, const char *s);
extern void gdl_prop(Node *ynp, int p, int g);
extern void gdl_prop_kv(Node *ynp, int p, int g, const char *k, const char *v);
extern void gdl_remove_q_error(Mloc m, Node *ynp);

extern Node *gdl_nongraph(Mloc *locp, Tree *ytp, const char *data, const char *type);

extern void gdl_break_o(Bracket_e bt);
extern void gdl_break_c(Bracket_e bt);
extern void gdl_break_h_l(void);
extern void gdl_break_o_l(Bracket_e bt);
extern void gdl_break_c_l(Bracket_e bt);
extern void gdl_state_o(Bracket_e bt, int state);
extern int gdl_state_c(Bracket_e bt);

extern Node *gdl_gloss_o(Mloc *mlp, Tree *ytp, const char *data, Bracket_e bt);
extern Node *gdl_gloss_c(Mloc *mlp, Tree *ytp, const char *data, Bracket_e bt);

extern void gdl_cell(Tree *ytp, const char *span);
extern Node *gdl_field(Tree *ytp, const char *ftype);
extern Node *gdl_graph(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_normseg(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_mod(Tree *ytp, const char *data);
extern Node *gdl_punct(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_listnum(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_new_word(Tree *ytp);
extern Node *gdl_nnum(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_number(Mloc *locp, Tree *ytp, const char *data);
extern Node *gdl_barenum(Mloc *locp, Tree *ytp, const char *data);
extern void gdl_unlegacy(Node *np);
extern unsigned char *gdl_unlegacy_str(Mloc *mloc, unsigned const char *gp);
extern int gdl_legacy_check(Node *, unsigned const char *gp);
extern void gdl_incr_qin(void);
extern void gdl_decr_qin(void);
extern intptr_t gdl_balance_break_c(Mloc mlp, int tok);
extern intptr_t gdl_balance_state(Mloc mlp, int tok, int state);
extern void gdl_balance_init(void);
extern void gdl_balance_term(void);
extern void gdl_balance_flush(Mloc mlp);
extern void gdl_gp_type(Tree *ytp, enum gdlpropvals p);
extern const char *gdl_mod_cat(Node *np);
extern void gdl_modq_add(Node *np);
extern Node *gdl_mod_cmod(Mloc *locp, Node *np, Node *mnp, const char *m);
extern void gdl_modq_flush(void);
extern void gdl_modq_init(void);
extern void gdl_modq_term(void);
extern void gdl_mod_wrap(Node *np, int sub_simplexg);
extern void gdl_mod_wrap_q(Node *np);
extern char *gdl_one_off(const char *file, int line, const char *g, int voidsl);
extern const char *gdlsig(Tree*tp);
extern void gdlsig_node(Node *np, void *user);
extern void gdl_update_state(Node *np, gdlstate_t gs_tok);

extern void gdl_c_init(Node *ycp);
extern Node* gdl_c_term(Tree*);

extern const char *gdlsig_str(Mloc *mp, unsigned char *atf, int frag_ok, int deep);

extern const unsigned char *gdl_first_s(Node *gdl);

extern unsigned char *gdlseq(unsigned char *s);
extern unsigned char *gdlseq_oids(unsigned char *s, const char *j);

extern void gdlstate_props(Node *np, gdlstate_t sp);
extern void gdl_set_ids(const char *line_id, int widb);

extern void gdl_break_node(Node *np);
extern void gdl_state_node(Node *np);

extern void gdl_lex_flag(Mloc *mlp, unsigned const char *f);
extern void gdl_lex_hash(gdlstate_t *st);
extern int gdl_lex_state(void);

extern List *gdl_get_word_list(void);
extern void gdl_set_lang(Mloc *mp, const char *tag, struct lang_context *ctxt);
extern void gdl_set_word_id(const char *wid);
extern void gdl_auto_id(void);
extern void gdl_det_props(Node *d);
extern void gdl_group(Mloc mp, int type);
extern void gdl_clear_gg(Tree *ytp);

extern void gdl_indent(void);
extern void gdl_note_mark(const char *n);
extern void gdl_var_mark(Bracket_e type, char *v);

extern int gdl_lang_tag(const char *v);
extern void gdl_lang_flip(void);

extern void gdl_hc(int force);
extern void gdl_lex_closers(void);
extern void gdl_line_wrapup(Mloc m);

extern const char *gdl_grapheme_id(void);
extern Tree *gdl_get_tree(void);

extern Node *gdl_gloss_curr(void);
extern void gdl_gloss_props(Node *d);
extern void gdl_surro(void);

extern void gdl_set_lzr_sparse(Hash *l);
extern void gdl_legacy_lexer(int legacy);

extern gdl_graph_node_p gdl_graph_node;
extern gdl_grapheme_type_p gdl_grapheme_type;
extern gdl_delim_p gdl_delim;
extern Node *gdl_delim_l(Tree *ytp, const char *data);
extern Node *gdl_delim_s(Tree *ytp, const char *data);
extern Node *gdl_graph_node_l(Mloc *locp, Tree *ytp, const char *name, const char *data);
extern Node *gdl_graph_node_s(Mloc *locp, Tree *ytp, const char *name, const char *data);

extern void gdl_word_attr(Node *w);
extern List *gdl_rem_nonw(List *wds);

extern int gdlflextrace, gdl_word_mode;
extern int gdl_no_xml_ids, gdl_xmlids;

#endif/*GDLLIB_H*/
