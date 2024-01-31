#include <oraccsys.h>
#include <tok.h>

/* Callers should call this first, e.g.,

   Trun *r = trun_init(0)
   loch_init(r, arg_project, arg_type);
   
 */
Trun *
trun_init(int multi)
{
  Trun *r = calloc(1, sizeof(Trun));
  r->multi = multi;
  r->t_m = memo_init(sizeof(Tloc), r->multi ? 128 : 1);
  r->l_m = memo_init(sizeof(Lloc), r->multi ? 1024 : 1);
  r->w_m = memo_init(sizeof(Wloc), r->multi ? 1024 : 1);
  r->p = hpool_init();
  return r;
}

void
trun_term(Trun *r)
{
  memo_term(r->t_m);
  memo_term(r->l_m);
  memo_term(r->w_m);
  hpool_term(r->p);
  trun_word_term(r);
  free(r);
}

/* In the current implementation only a single trun_word is used but a
   future implementation may collect an array of trun_word while
   processing a text.
 */
Word *
trun_word_init(Trun *r)
{
#define TW_MAX 4	/* max g:w nesting depth; even 2 deep is unusual */
  static struct trun_word tw[TW_MAX];
  if (r->rs.in_w >= TW_MAX)
    {
      fprintf(stderr, "lib/tok/trun.c: internal limit reached TW_MAX=%d\n", TW_MAX);
      return NULL;
    }
  r->rw = &tw[r->rs.in_w];
  r->rw->gpp_alloced = 1;
  r->rw->gpp = calloc(r->rw->gpp_alloced, sizeof(Gsig));
  r->rw->gpp_used = 0;
  r->rw->w = loch_word(r);
  loch_word(r)->w = r->rw;
  return r->rw;
}

void
trun_state_reset(Trun *r)
{
  memset(&r->rs, '\0', sizeof(struct trun_gdl_state));
}

void
trun_word_reset(Trun *r)
{
#if 1
  trun_word_term(r);
#else
  memset(r->rw.gpp, '\0', r->rw.gpp_alloced * sizeof(Gsig));  
  r->rw.gpp_used = 0;
#endif
}

void
trun_word_term(Trun *r)
{
  if (r->rw)
    {
      free(r->rw->gpp);
      memset(r->rw, '\0', sizeof(Word));
    }
  /* This routine is called on </g:w> before r->rs.in_w is
     decremented; we need to anticipate where r->r->rw should be after
     that decrement. */
  if (r->rs.in_w > 1)
    --r->rw;
}

void
trun_file(Trun *r, const char *f)
{
  r->rs.file = f;
  r->rs.F_done = 0;
}
