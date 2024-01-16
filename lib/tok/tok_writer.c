#include <oraccsys.h>
#include <tok.h>

void
tokw_G(Trun *r)
{
  Word *w = r->rw;
  int i;
  for (i = 0; i < w->gpp_used; ++i)
    {
      Gsig *wgp = gsb_get_n(w, i);
      fputs("g\t", r->o);
      gsig_print(r->o, wgp, "\t");
      fprintf(r->o, "\t%s", wgp->w->word_form);
      fputc('\n', r->o);
    }
}
