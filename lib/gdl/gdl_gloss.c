#include <oraccsys.h>
#include <stck.h>
#include "gdl.tab.h"
#include "gdl.h"
#include "gdlstate.h"
#include "gdlstream.h"

/* Glosses {{ ... }} and {( ... )} host separate word streams and must
 * be managed with a stack because in they could be nested (not clear
 * if that ever actually happens).
 *
 * In XTF glosses rigorously preserve the order of incoming graphemes,
 * meaning that words contain glosses, e.g., a-{{he-pi₂}}-bi.
 *
 * Glosses are also managed with the stream system (see gdlstream.h);
 * in XCL glosses are pulled into their respective streams so they can
 * be processed naturally with tools like the ngrammer.
 */

extern const char *word_lang_tag;
extern Node *gdl_recycled_word;
static Stck *glosstck;

Node *
gdl_gloss_curr(void)
{
  if (glosstck)
    return (Node *)stck_peek(glosstck);
  else
    return NULL;
}

static int
gloss_stream(const unsigned char *s)
{
  const unsigned char *t = s;
  while (*s && isdigit(*s))
    ++s;
  return s > t && *s == ':';
}

/* If data is /{{[0-9]+:/ the digits are a stream code */
Node*
gdl_gloss_o(Mloc *mlp, Tree *ytp, const char *data, Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  int stream = -1;
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/o: %d=%s\n", bt, bp->str);
  if ((bp->tok == L_dbl_cur || bp->tok == L_cur_par) && gloss_stream((uccp)(data+2)))
    {
      stream = atoi(data+2);
      if (stream <= 0 || stream > 99)
	{
	  mesg_vwarning(currgdlfile, gdllineno, "stream out of range in '%s'", data);
	  stream = 0;
	}
      /*data = (bp->tok == L_dbl_cur ? "{{" : "{(");*/ /* data
	probably doesn't need to be used after this */
    }
  (void)gdl_balance_state(*mlp, bp->tok);

  /* if the current word is empty unwind it because it means the {{/{( is at SOL */
  if (gdl_word_mode && !strcmp(ytp->curr->name, "g:w") && !ytp->curr->kids)
    {
      tree_curr(ytp->curr->rent);
      gdl_recycled_word = kids_rem_last(ytp);
    }
    
  ret = gdl_push(ytp, "g:gloss");
  gdl_prop_kv(ret, GP_ATTRIBUTE, PG_GDL_INFO, "g:type",
	      bt == e_L_cur_par ? "text" : (bt == e_L_dbl_cur ? "lang" : "surro"));
  gdl_gloss_props(ret);
  
  /* We keep a stack of gloss containers and use it to set word parents in gdl_new_word */
  if (!glosstck)
    glosstck = stck_init(3);

  /* track word_lang_tag on entry to the gloss in np->user */
  ret->utype = N_U_STR;
  ret->user = (void*)word_lang_tag;
  stck_push(glosstck, (intptr_t)ret);

  /* For now we always start a new word at start of g:gloss--it is
     conceivable that we may need to introduce discontinuous word
     structures across glosses */
  ps_on(bp->oc);
  rs_on(bp->s);
  prop_node_add(ret, GP_STREAM, PG_GDL_STATE, (void*)(uintptr_t)stream, NULL);

  Node *wret = gdl_new_word(ytp);
  return wret;
}

Node *
gdl_gloss_c(Mloc *mlp, Tree *ytp, const char *data, Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/c: %d=%s\n", bt, bp->str);

  if (-1 != gdl_balance_state(*mlp, bp->tok))
    {
      /*ret = gdl_pop(ytp, data);*/
      /* We don't need to do anything with this popped container
	 because we access it via stck_peek in gdl_new_word */
      ret = (Node *)stck_pop(glosstck);
      word_lang_tag = ret->user;
      ret->utype = N_U_NODE;
      ret->user = NULL;

      if (glosstck->top < 0)
	{
	  stck_term(glosstck);
	  glosstck = NULL;
	}
      if (ret)
	ret = ret->rent;
      else
	ret = gdl_pop(ytp, data);
    }
  else
    ret = gdl_pop(ytp, data);
  bit_set(*lst,bp->oc);
  rs_no(bp->s);
  tree_curr(ret);
  return ret;
}

/* Surrogates
 *
 * A surrogate is a sign or sign group that stands in for one or more
 * signs or words: the most common forms are MIN or |KI.MIN| used to
 * mean "ditto" in lexical texts.
 *
 * In ATF surrogates have the form: MIN<(a)> where 'a' is the text for
 * which MIN is a surrogate.
 *
 * This syntax is similar to that of implied text, i.e., <(...)> used
 * especially in liturgies where blank spaces are used to imply
 * repetition of the above text.
 *
 * The defining difference between a surrogate and an implied sequence
 * is that a sign, compound or group followed directly by '<(' with no
 * intervening delimiter is a surrogate.  All other combinations are
 * parsed as implied sequences of signs.  This means that {d}<(namma)>
 * is not a surrogate, but AN<({d}namma)> is a surrogate.
 *
 * Surrogates are lumped together with glosses in the GDL schema and
 * the implementation in ax is very different from that of ox--the
 * latter has actually been broken for several years because
 * multi-word surrogates are not treated specially as a result of
 * changes in the code which had unintended consequences.
 *
 * In ax, the head signs of the surrogate are wrapped in <g:sur>.
 * This has an xml:id which is used as the @ref for the sequence of
 * signs which are implied by the surrogate.  Surrogacy is a state,
 * identical to the 'implied' state.  There is no distinction in ax
 * between grapheme-level surrogates and word-level surrogates.
 * 
 */

/* Called when <( is found; return 1 if the last grapheme pointer is
 * NULL or meets the conditions for a surrogate
 *
 * Need to watch whether simply wrapping lgp is good enough--if lgp is
 * in a group may need to wrap parent group in g:sur.
 *
 */
void
gdl_surro(void)
{
  if (lgp)
    {
      if (!strcmp(lgp->name, "g:gg")
	  || (strlen(lgp->name) == 3
	      && ((lgp->name[2] == 's' || lgp->name[2] == 'c')
		  || (lgp->name[2] == 'g' && sll_has_sign_indicator((uccp)lgp->text)))))
	{
	  if (!prop_find_kv(lgp->props, "g:delim", NULL))
	    {
	      Node *np = tree_node(lgp->tree, lgp->ns, "g:sur", lgp->depth, lgp->mloc);
	      Prop *xidp = prop_find_kv(lgp->props, "xml:id", NULL);
	      curr_sur_id = (char*)pool_alloc(strlen(xidp->u.k->v)+5, gdlpool);
	      sprintf((char*)curr_sur_id, "%s.sur", xidp->u.k->v);
	      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id", curr_sur_id);
	      if (lgp->rent->kids == lgp)
		lgp->rent->kids = np;
	      np->kids = lgp;
	      np->rent = lgp->rent;
	      np->prev = lgp->prev;
	      np->next = lgp->next;
	      tree_curr(lgp->rent);
	      lgp->last = np;
	      lgp->rent = np;
	      lgp->prev = lgp->next = NULL;
	      rs_on(gs_surro);
	    }
	}
    }
}
