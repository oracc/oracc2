#include <oraccsys.h>
#include "qx.h"

extern int ntoks;
extern struct token *toks;
extern struct Datum result;
extern Vido *vp;

static void
selemx_print_fields(FILE *fp, int f)
{
  switch (f)
    {
    case 0x1000:
      fputs("; gw/sense", fp);
      break;
    case 0x2000:
      fputs("; cf/norm", fp);
      break;
    case 0x4000:
      fputs("; m1", fp);
      break;
    case 0x8000:
      fputs("; m2", fp);
      break;
    }
}

void
debug_tuw(struct location8 *p8)
{
  if ('v' == id_prefix(p8->text_id))
    {      
      fprintf(f_log, "%s.%d.%d ",
	      vido_get_id(vp,idVal(p8->text_id)/*, vido_display_proj*/),
	      p8->unit_id, p8->word_id);
      
    }
  else
    {
      fprintf(f_log, "%c%06d.%d.%d\n", 
	      id_prefix(p8->text_id), XidVal(p8->text_id),
	      p8->unit_id, p8->word_id);
    }
}

void
debug_location8(struct location8 *l8p, struct location8 *r8p)
{
  debug_tuw(l8p);
  
  fputs(" :: ", f_log);
  
  if (r8p)
    debug_tuw(r8p);
}

static int nl_16 = 1;

void
debug_location16(struct location16 *l16p, struct location16 *r16p)
{
  debug_tuw((struct location8 *)l16p);
  fprintf(f_log, "b=%d; sc=%d; ec=%d", 
	  l16p->branch_id, mask_sc(l16p->start_column), logo_mask(l16p->end_column));

  if (prop_sc(l16p->start_column))
    selemx_print_fields(f_log, prop_sc(l16p->start_column));

  fputs(" :: ", f_log);
	
  if (r16p)
    {
      debug_tuw((struct location8 *)r16p);
      fprintf(f_log, "b=%d; sc=%d; ec=%d", 
	      r16p->branch_id, mask_sc(r16p->start_column), logo_mask(r16p->end_column));
	    
      if (prop_sc(r16p->start_column))
	selemx_print_fields(f_log, prop_sc(r16p->start_column));
    }

  if (nl_16)
    fputc('\n', f_log);
}

void
debug_location24(struct location24 *l24p, struct location24 *r24p)
{
  nl_16 = 0;
  debug_location16((struct location16 *)l24p, (struct location16 *)r24p);
  /*  if (l24p->sentence_id) */
    fprintf(f_log, 
	    " [s=%d;c=%d;p=%d;l=%d",
	    l24p->sentence_id,
	    l24p->clause_id,
	    l24p->phrase_id,
	    l24p->lemma_id);
    if (r24p/* && r24p->sentence_id*/)
    fprintf(f_log, 
	    " // s=%d;c=%d;p=%d;l=%d]",
	    r24p->sentence_id,
	    r24p->clause_id,
	    r24p->phrase_id,
	    r24p->lemma_id);

  fputc('\n', f_log);
  nl_16 = 1;
}

void
debug_message(const char *msg)
{
  fprintf(f_log,"=====begin: %s====\n", msg);
}

void
debug_results(const char *label, struct Datum *dp)
{
  int i;
  
  fprintf(f_log,"=====begin: %s [key=%s]====\n", label, dp->key);

  if (dp->data_size == sizeof(struct location8))
    for (i = 0; i < dp->count; ++i)
      debug_location8(dp->l.l8p[i], dp->r.l8p ? dp->r.l8p[i] : NULL);
  else if (dp->data_size == sizeof(struct location16))
    for (i = 0; i < dp->count; ++i)
      debug_location16(dp->l.l16p[i], dp->r.l16p ? dp->r.l16p[i] : NULL);
  else
    for (i = 0; i < dp->count; ++i)
      debug_location24(dp->l.l24p[i], dp->r.l24p ? dp->r.l24p[i] : NULL);

  fprintf(f_log,"=====end: %s [key=%s]=====\n", label, dp->key);
  fflush(f_log);
}
