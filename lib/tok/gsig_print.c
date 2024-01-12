#include <oraccsys.h>
#include <gsig.h>

void
gsig_print(FILE *fp, Gsig *gp, const char *id_sig_sep)
{

#undef pp
#define pp(p) ((p)?(p):"")

  fprintf(fp, "%s.%s.%s%s@%s%%%s:%c/%s=%s-%s-%s#%c%c%%%s:#%c%%%s:#%c%d#%c%d#%c%d",
	  gp->soid, pp(gp->foid), pp(gp->value),
	  id_sig_sep,
	  pp(gp->project), gp->asltype, gp->gdltype ? gp->gdltype : 'u',
	  gp->form, gp->sname, pp(gp->fname), pp(gp->value),
	  gp->role ? gp->role : 'u', gp->roletype ? gp->roletype : 'u', pp(gp->w->word_lang),
	  gp->type ? gp->type : 'u', pp(gp->logolang),
	  gp->position ? gp->position : 'u', gp->index,
	  gp->no_d_position ? gp->no_d_position : 'u', gp->no_d_index,
	  gp->c_position ? gp->c_position : 'u', gp->ce_index
	  );
}
