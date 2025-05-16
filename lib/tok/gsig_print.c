#include <oraccsys.h>
#include <tok.h>

void
gsig_print(FILE *fp, Gsig *gp, const char *id_sig_sep)
{
  fprintf(fp, "%s.%s.%s%s@%s%%%s:%c/%s=%s-%s-%s#%c%c%%%s:#%c%%%s:#%c%d#%c%d#%c%d#%c%c%s",
	  gp->soid, pp(gp->foid), pp(gp->value),
	  id_sig_sep,
	  pp(gp->project), gp->asltype, gp->gdltype ? gp->gdltype : 'u',
	  gp->form, gp->sname, pp(gp->fname), pp(gp->value),
	  gp->role ? gp->role : 'u', gp->roletype ? gp->roletype : 'u', pp(gp->w->word_lang),
	  gp->type ? gp->type : 'u', pp(gp->logolang),
	  gp->position ? gp->position : 'u', gp->index,
	  gp->no_d_position ? gp->no_d_position : 'u', gp->no_d_index,
	  gp->c_position ? gp->c_position : 'u', gp->ce_index,
	  gp->preserved, gp->editorial, gp->flags
	  );
}

void
gsig_print_seq(FILE *fp, Gsig *gp, const char *id_sig_sep)
{
  char obuf[strlen(gp->seq)+1], *o = obuf;
  strcpy(o, gp->seq);

  int i = 0;
  
  while (o)
    {
      char *sp = strchr(o, '_'), save;
      if (sp)
	{
	  save = '_';
	  *sp = '\0';
	}
      else
	save = '\0';
      fprintf(fp, "%s.%s.%s%s@%s%%%s:%c/%s=%s-%s-%s#%c%c%%%s:#%c%%%s:#%c%d^%d#%c%d#%c%d#%c%c%s\n",
	      o, "", "",
	      id_sig_sep,
	      pp(gp->project), gp->asltype, 'e',
	      "", "", "", "",
	      gp->role ? gp->role : 'u', gp->roletype ? gp->roletype : 'u', pp(gp->w->word_lang),
	      gp->type ? gp->type : 'u', pp(gp->logolang),
	      gp->position ? gp->position : 'u', gp->index, i++,
	      gp->no_d_position ? gp->no_d_position : 'u', gp->no_d_index,
	      gp->c_position ? gp->c_position : 'u', gp->ce_index,
	      gp->preserved, gp->editorial, gp->flags
	      );
      if (save)
	o = sp+1;
      else
	o = NULL;
    }
}
