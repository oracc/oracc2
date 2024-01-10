#include <oraccsys.h>
#include <gsig.h>

/* This should be allocated for each input */
struct gsb_input
{
  struct gsb_word *wpp;
  int wpp_alloced;
  int wpp_used;
};

/* This should be allocated for each sH/g:w; for line-oriented
   programs it can be reset rather than reallocated.
 */
struct gsb_word
{
  Gsig **gpp;
  Gsig *curr_c_wgp;
  int gpp_alloced;
  int gpp_used;
};

struct gsb_input *
gsb_input_init(void)
{
  struct gsb_input *gsip = malloc(sizeof(struct gsb_input));
  gsip->wpp_alloced = 1;
  gsip->wpp = calloc(gsip->wpp_alloced * sizeof(struct gsb_word));
  gsip->wpp_used = 0;
};

void
gsb_input_term(struct gsb_input *gsip)
{
  free(gsip->wpp);
  free(gsip);
}

/* In the current implementation only a single gsb_word is used but a
   future implementation may collect an array of gsb_word while
   processing a text.
 */
struct gsb_word *
gsb_word_init(struct gsb_input *gsip)
{
  return &gsip->wpp[0];
}

void
gsb_word_reset(struct gsb_word gswp)
{
  memset(gswp->gpp, '\0', gswp_gpp_alloced * sizeof(Gsig));
  gswp->gpp_used = 0;
}

void
gsb_word_term(struct gsb_word *gswp)
{
  free(gswp->gpp);
  memset(gswp, '\0', sizeof(struct gsb_word));
}

struct Gsig *
gsb_new(struct gsb_word *gswp)
{
  if (gswp->gpp_used == gswp->gpp_alloced)
    {
      gswp->gpp_alloced += 16;
      gswp->gpp = realloc(gswp->gpp, gswp->gpp_alloced * sizeof(Gsig));
      memset((void*)(gswp->gpp + (gswp->alloced-16)), '\0', 16 * sizeof(Gsig));
    }
  return &gswp->gpp[gswp->gpp_used++];
}

Gsig *
gsb_get(struct gsb_word *gswp)
{
  return &gswp->gpp[gwsp->gpp_used-1];
}

Gsig *
gsb_get_n(struct gsb_word *gswp, int n)
{
  if (n < gpp_used)
    return &gswp->gpp[n];
  else
    return NULL;
}

void
gsb_add(struct gsb_word *gswp,
	char type, const char *form, const char *oid, const char *sign,
	const char *spoid, const char *spsign, const char *logolang)
{
  struct Gsig *wgp = gsb_get(gswp);
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
  if (spoid)
    {
      strcpy(wgp->soid, spoid);
      strcpy(wgp->sname, spsign);
      strcpy(wgp->foid, oid);
      strcpy(wgp->fname, sign);
    }
  else
    {
      strcpy(wgp->soid, oid);
      strcpy(wgp->sname, sign);
    }
  if (*word_lang)
    {
      const char *sl = "sl";
      if ('q' == lang[0] && 'p' == lang[1])
	sl = ('c' == lang[2] ? "pc" : "pe");
      strcpy(wgp->lang, word_lang);
      strcpy(wgp->asltype, sl);
    }
  if (role)
    {
      wgp->role = role;
      if (*roletext)
	wgp->roletype = *roletext;
    }
  if (in_c)
    {
      if ('c' == type)
	{
	  curr_c_wgp = wgp;
	  wgp->c_index = 1 + wg_index;
	}
      else
	{
	  wgp->type = 'c';
	  wgp->c_index = curr_c_wgp->c_index;
	  if (1 == in_c++)
	    wgp->c_position = 'b';
	  else
	    wgp->c_position = 'm';
	}
    }
  else if (*logolang)
    {
      wgp->type = 'l';
      strcpy(wgp->logolang, logolang);
    }
  else
    wgp->type = 'u';
  wgp->index = 1 + wg_index;
  if ('d' != role)
    wgp->no_d_index = 1 + ++no_d_index;
  if (form && *form)
    strcpy(wgp->form, form);
  list_add(wg, (void*)(uintptr_t)wg_index);
}

void
gsb_set_positions(struct gsb_word *gswp)
{
  int i;
  for (i = 0; i <= wg_index; ++i)
    {
      Gsig *wgp = gsb_get(gswp);
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
gsb_last(struct gsb_word *gswp)
{
  if (gswp->used > 0)
    {
      Gsig *wgp;
      int lastindex = gswp->used - 1;

      while (lastindex >= 0 && (wgp = wgp_get_n(wgsp, lastindex--)))
	if ('c' != wgp->type)
	  break;
      
      if (wgp)
	{
	  wgp->last = 1;
	  if (wg_index != no_d_index && no_d_index >= 0)
	    {
	      wgp = wgp_get_n(gswp, no_d_index);
	      wgp->no_d_last = 1;
	    }
	  else
	    wgp->no_d_last = 1;
	}
      wgp_set_positions(gswp);
    }
}

void
gsb_c_last(struct gsb_word *gswp)
{
  if (gswp->used > 0)
    {
      Gsig *wgp = wgp_get(gswp);
      wgp->c_position = 'e';
    }
}

void
gsb_punct(struct gsb_word *gswp, const char *t)
{
  if (gswp->used > 0)
    {
      Gsig *wgp = wgp_get(gswp);
      if ('*' == *t)
	wgp->roletype = 'b';
      else if (':' == *t)
	wgp->roletype = 'd';    
    }
}

void
gsb_sign(struct gsb_word *gswp, const char *t)
{
  if (gswp->used > 0)
    {
      Gsig *wgp = wgp_get(gswp);
      strcpy(wgp->form, t);
    }
}

void
gsb_value(struct gsb_word *gswp, const char *t)
{
  if (gswp > 0)
    {
      Gsig *wgp = wgp_get(gswp);
      strcpy(wgp->value, t);
      strcpy(wgp->form, t);
    }
}

void
gsb_print(struct gsb_word *gswp)
{
  int i;
  for (i = 0; i < gswp->used; ++i)
    {
      Gsig *wgp = wgp_get_n(gswp, i);
      g_signature(wgp, "\t");
      if (form_with_w)
	fprintf(tab, "\t%s", word_form);
      fputc('\n', tab);
    }
}
