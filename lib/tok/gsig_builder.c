#include <oraccsys.h>
#include <tok.h>

#define gsb_strcpy(dest,src) dest=(char*)hpool_copy((uccp)src,r->p)

static const char * gsb_target_lang = "sux";
void
gsb_set_target_lang(const char *l)
{
  gsb_target_lang = l;
}

Gsig *
gsb_new(Trun *r)
{
  Word *w = r->rw;
  if (w->gpp_used == w->gpp_alloced)
    {
      w->gpp_alloced += 16;
      w->gpp = realloc(w->gpp, w->gpp_alloced * sizeof(Gsig));
      memset((void*)(w->gpp + (w->gpp_alloced-16)), '\0', 16 * sizeof(Gsig));
    }
  w->gpp[w->gpp_used].w = loch_word(r);
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
gsb_status(Trun *r, const char *pres, const char *edit, const char *flag)
{
  Gsig *wgp = gsb_get_n(r->rw, r->rw->gpp_used-1);
  struct tokflags *tfp;
  if (*pres)
    {
      tfp = tokflags(pres,strlen(pres));
      wgp->preserved = *tfp->attr;
    }
  else
    wgp->preserved = '+';
  if (*edit)
    {
      tfp = tokflags(edit,strlen(edit));
      wgp->editorial = *tfp->attr;
    }
  else
    wgp->editorial = 'k';
  gsb_strcpy(wgp->flags, flag);
}

void
gsb_add(Trun *r,
	char type, const char *form, const char *oid, const char *sign,
	const char *spoid, const char *spsign, const char *lang, const char *logolang)
{
  /* If we are only processing sux accept sux, sux-x-emesal, etc., but reject everything else */
  if (lang && gsb_target_lang && strncmp(lang, gsb_target_lang, strlen(gsb_target_lang)))
    return;
  
  Gsig *wgp = gsb_new(r);
  wgp->project = (char*)loch_text(r)->text_project;
  wgp->gdltype = type;

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
  if (spoid && *spoid)
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
  if (r->rw->role)
    {
      wgp->role = r->rw->role;
      if (*r->rw->roletext)
	wgp->roletype = *r->rw->roletext;
    }
  if (r->rw->in_c)
    {
      if ('c' == type)
	{
	  r->rw->wgp_c_index = wgp->index;
	  wgp->c_index = wgp->index;
	}
      else
	{
	  wgp->type = 'c';
	  wgp->c_index = r->rw->wgp_c_index;
	  if (1 == r->rw->in_c++)
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
    wgp->no_d_index = ++r->rw->no_d_index;
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
		wgp->position = 't';
	      else
		wgp->position = 'm';
	    }
	  else
	    {
	      if (wgp->last)
		wgp->position = (w->psu ? 'A' : 'a');
	      else
		wgp->position = 'i';
	    }
	  if (wgp->role == 'd' || wgp->role == 'D')
	    wgp->index = 0;
	  if (wgp->no_d_index > 1)
	    {
	      if (wgp->no_d_last)
		wgp->no_d_position = 't';
	      else
		wgp->no_d_position = 'm';
	    }
	  else
	    {
	      if (wgp->no_d_last)
		wgp->no_d_position = (w->psu ? 'A' : 'a');
	      else
		wgp->no_d_position = 'i';
	    }
	  if (wgp->role == 'd' || wgp->role == 'D')
	    wgp->no_d_index = 0;
	}
    }
}

void
gsb_last(Trun *r)
{
  Word *w = r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = NULL;
      int lastindex = w->gpp_used - 1;

      while (lastindex >= 0 && (wgp = gsb_get_n(w, lastindex--)))
	if ('c' != wgp->type && 'd' != wgp->role && 'D' != wgp->role)
	  break;
      
      if (wgp)
	{
	  wgp->no_d_last = 1;
	  lastindex = w->gpp_used - 1;
	  wgp = gsb_get_n(w, w->no_d_index-1);
	  if (wgp)
	    wgp->last = 1;
	  else
	    fprintf(stderr,
		    "lib/gsig_builder/gsb_last: no_d_index=%d is out of bounds at %s\n",
		    w->no_d_index, loch_word(r)->word_id);
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
  Word *w = r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      gsb_strcpy(wgp->form, t);
    }
}

void
gsb_value(Trun *r, const char *t)
{
  Word *w = r->rw;
  if (w->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(w);
      gsb_strcpy(wgp->value, t);
      gsb_strcpy(wgp->form, t);
    }
}
