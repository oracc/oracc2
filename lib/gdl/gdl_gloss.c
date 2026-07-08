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
  ret->user = word_lang_tag;
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
