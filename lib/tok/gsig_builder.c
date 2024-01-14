#include <oraccsys.h>
#include <tok.h>

Gsig *
gsb_new(Trun *r)
{
  Word *w = &r->rw;
  if (w->gpp_used == w->gpp_alloced)
    {
      w->gpp_alloced += 16;
      w->gpp = realloc(w->gpp, w->gpp_alloced * sizeof(Gsig));
      memset((void*)(w->gpp + (w->gpp_alloced-16)), '\0', 16 * sizeof(Gsig));
    }
  w->gpp[w->gpp_used].w = loch_word(r)->w->w;
  w->gpp[w->gpp_used].index = 1+w->gpp_used;
  return &w->gpp[w->gpp_used++];
}

Gsig *
gsb_get(Word *w)
{
  return &w->gpp[w->gpp_used-1];
}

Gsig *
gsb_get_n(Word *w, int n)
{
  if (n < w->gpp_used)
    return &w->gpp[n];
  else
    return NULL;
}

void
gsb_add(Trun *r,
	char type, const char *form, const char *oid, const char *sign,
	const char *spoid, const char *spsign, const char *lang, const char *logolang)
{
  Gsig *wgp = gsb_new(r);
  wgp->project = loch_text(r)->text_project;
  wgp->gdltype = type;

#define gsb_strcpy(dest,src) dest=(char*)hpool_copy((uccp)src,r->p)
  
  if ('p' == type)
    {
      wgp->role = 'p';
      wgp->roletype = 'u';
    }
  else
    {
      wgp->role = 'w'; 	/* may be reset later */
      wgp->roletype = 'n'; 	/* ditto */
    }
  if (spoid)
    {
      gsb_strcpy(wgp->soid, spoid);
      gsb_strcpy(wgp->sname, spsign);
      gsb_strcpy(wgp->foid, oid);
      gsb_strcpy(wgp->fname, sign);
    }
  else
    {
      gsb_strcpy(wgp->soid, oid);
      gsb_strcpy(wgp->sname, sign);
    }
  if (lang)
    {
      const char *sl = "sl";
      if ('q' == lang[0] && 'p' == lang[1])
	sl = ('c' == lang[2] ? "pc" : "pe");
      gsb_strcpy(wgp->asltype, sl);
    }
  if (r->rs.role)
    {
      wgp->role = r->rs.role;
      if (*r->rs.roletext)
	wgp->roletype = *r->rs.roletext;
    }
  if (r->rs.in_c)
    {
      if ('c' == type)
	{
	  r->rs.curr_c_wgp = wgp;
	  wgp->c_index = wgp->index;
	}
      else
	{
	  wgp->type = 'c';
	  wgp->c_index = r->rs.curr_c_wgp->c_index;
	  if (1 == r->rs.in_c++)
	    wgp->c_position = 'b';
	  else
	    wgp->c_position = 'm';
	}
    }
  else if (*logolang)
    {
      wgp->type = 'l';
      gsb_strcpy(wgp->logolang, logolang);
    }
  else
    wgp->type = 'u';
  if ('d' != wgp->role)
    wgp->no_d_index = ++r->rs.no_d_index;
  if (form && *form)
    gsb_strcpy(wgp->form, form);
}

void
gsb_set_positions(Word *w)
{
  int i;
  for (i = 0; i < w->gpp_used; ++i)
    {
      Gsig *wgp = gsb_get_n(w, i);
      if ('c' == wgp->type)
	{
	  wgp->ce_index = wgp->index - wgp->c_index;
	  wgp->index = wgp->no_d_index = 0;
	  wgp->position = wgp->no_d_position = 'u';
	}
      else
	{
	  int use_index = (wgp->c_index ? wgp->c_index : wgp->index);
	  if (use_index > 1)
	    {
	      if (wgp->last)
		wgp->position = 'e';
	      else
		wgp->position = 'm';
	    }
	  else
	    {
	      if (wgp->last)
		wgp->position = 'i';
	      else
		wgp->position = 'b';
	    }
	  if (wgp->no_d_index > 1)
	    {
	      if (wgp->no_d_last)
		wgp->no_d_position = 'e';
	      else
		wgp->no_d_position = 'm';
	    }
	  else
	    {
	      if (wgp->no_d_last)
		wgp->no_d_position = 'i';
	      else
		wgp->no_d_position = 'b';
	    }
	}
    }
}

void
gsb_last(Trun *r)
{
  Word *w = &r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = NULL;
      int lastindex = w->gpp_used - 1;

      while (lastindex >= 0 && (wgp = gsb_get_n(w, lastindex--)))
	if ('c' != wgp->type)
	  break;
      
      if (wgp)
	{
	  wgp->last = 1;
	  if ((w->gpp_used-1) != r->rs.no_d_index && r->rs.no_d_index >= 0)
	    {
	      wgp = gsb_get_n(w, r->rs.no_d_index-1);
	      wgp->no_d_last = 1;
	    }
	  else
	    wgp->no_d_last = 1;
	}
      gsb_set_positions(w);
    }
}

void
gsb_c_last(Word *w)
{
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      wgp->c_position = 'e';
    }
}

void
gsb_punct(Word *w, const char *t)
{
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      if ('*' == *t)
	wgp->roletype = 'b';
      else if (':' == *t)
	wgp->roletype = 'd';    
    }
}

void
gsb_sign(Trun *r, const char *t)
{
  Word *w = &r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      gsb_strcpy(wgp->form, t);
    }
}

void
gsb_value(Trun *r, const char *t)
{
  Word *w = &r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      gsb_strcpy(wgp->value, t);
      gsb_strcpy(wgp->form, t);
    }
}

void
gsb_show(FILE *tab, Trun *r)
{
  Word *w = &r->rw;
  int i;
  for (i = 0; i < w->gpp_used; ++i)
    {
      Gsig *wgp = gsb_get_n(w, i);
      gsig_print(tab, wgp, "\t");
      fprintf(tab, "\t%s", wgp->w->word_form);
      fputc('\n', tab);
    }
}
