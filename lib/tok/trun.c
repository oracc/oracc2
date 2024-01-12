#include <oraccsys.h>
#include <trun.h>


/* Callers should call this first, e.g.,

   Trun *r = trun_init(loch_init(0));
 */
Trun *
trun_init(Loch *l)
{
  Trun *r = calloc(1, sizeof(Trun));
  r->l = l;
  l->r = r;
  r->p = hpool_init();
  r->rw->wpp_alloced = 1;
  r->rw->wpp = calloc(r->rw->wpp_alloced, sizeof(struct Word));
  r->rw->wpp[0].r = r;
  r->rw->wpp_used = 0;
  return r;
}

void
trun_term(Trun *r)
{
  hpool_term(r->p);
  free(r->rw->wpp);
  free(r);
}

/* In the current implementation only a single gsb_word is used but a
   future implementation may collect an array of gsb_word while
   processing a text.
 */
Word *
trun_word_init(Trun *r)
{
  return &r->rw->wpp[0];
}

void
trun_state_reset(Trun *r)
{
  memset(&r->rs, '\0', sizeof(struct trun_gdl_state));
}

void
trun_word_reset(Trun *r)
{
  memset(r->rw->gpp, '\0', r->rw->gpp_alloced * sizeof(Gsig));
  r->rw->gpp_used = 0;
}

void
trun_word_term(Trun *r)
{
  free(r->rw->gpp);
  memset(&r->rw, '\0', sizeof(Word));
}
