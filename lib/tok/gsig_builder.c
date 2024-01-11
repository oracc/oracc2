#include <oraccsys.h>
#include <gsig.h>

struct gsb_input *
gsb_input_init(void)
{
  struct gsb_input *gsip = malloc(sizeof(struct gsb_input));
  gsip->wpp_alloced = 1;
  gsip->wpp = calloc(gsip->wpp_alloced, sizeof(struct gsb_word));
  gsip->wpp[0].in = gsip;
  gsip->wpp_used = 0;
  gsip->p = hpool_init();
  return gsip;
}

void
gsb_input_term(struct gsb_input *gsip)
{
  hpool_term(gsip->p);
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
gsb_word_reset(struct gsb_word *gswp)
{
  gswp->no_d_index = 0;
  gswp->lang = "";
  memset(gswp->gpp, '\0', gswp->gpp_alloced * sizeof(Gsig));
  gswp->gpp_used = 0;
}

void
gsb_word_term(struct gsb_word *gswp)
{
  free(gswp->gpp);
  memset(gswp, '\0', sizeof(struct gsb_word));
}

Gsig *
gsb_new(struct gsb_word *gswp)
{
  if (gswp->gpp_used == gswp->gpp_alloced)
    {
      gswp->gpp_alloced += 16;
      gswp->gpp = realloc(gswp->gpp, gswp->gpp_alloced * sizeof(Gsig));
      memset((void*)(gswp->gpp + (gswp->gpp_alloced-16)), '\0', 16 * sizeof(Gsig));
    }
  gswp->gpp[gswp->gpp_used].w = gswp;
  gswp->gpp[gswp->gpp_used].index = 1+gswp->gpp_used;
  return &gswp->gpp[gswp->gpp_used++];
}

Gsig *
gsb_get(struct gsb_word *gswp)
{
  return &gswp->gpp[gswp->gpp_used-1];
}

Gsig *
gsb_get_n(struct gsb_word *gswp, int n)
{
  if (n < gswp->gpp_used)
    return &gswp->gpp[n];
  else
    return NULL;
}

void
gsb_add(struct gsb_word *gswp,
	char type, const char *form, const char *oid, const char *sign,
	const char *spoid, const char *spsign, const char *lang, const char *logolang)
{
  Gsig *wgp = gsb_new(gswp);
  wgp->project = gswp->in->textproj;
  wgp->gdltype = type;

#define gsb_strcpy(dest,src) dest=(char*)hpool_copy((uccp)src,gswp->in->p)
  
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
  if (gswp->role)
    {
      wgp->role = gswp->role;
      if (*gswp->roletext)
	wgp->roletype = *gswp->roletext;
    }
  if (gswp->in->in_c)
    {
      if ('c' == type)
	{
	  gswp->curr_c_wgp = wgp;
	  wgp->c_index = wgp->index;
	}
      else
	{
	  wgp->type = 'c';
	  wgp->c_index = gswp->curr_c_wgp->c_index;
	  if (1 == gswp->in->in_c++)
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
    wgp->no_d_index = ++gswp->no_d_index;
  if (form && *form)
    gsb_strcpy(wgp->form, form);
}

void
gsb_set_positions(struct gsb_word *gswp)
{
  int i;
  for (i = 0; i < gswp->gpp_used; ++i)
    {
      Gsig *wgp = gsb_get_n(gswp, i);
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
  if (gswp->gpp_used > 0)
    {
      Gsig *wgp = NULL;
      int lastindex = gswp->gpp_used - 1;

      while (lastindex >= 0 && (wgp = gsb_get_n(gswp, lastindex--)))
	if ('c' != wgp->type)
	  break;
      
      if (wgp)
	{
	  wgp->last = 1;
	  if ((gswp->gpp_used-1) != gswp->no_d_index && gswp->no_d_index >= 0)
	    {
	      wgp = gsb_get_n(gswp, gswp->no_d_index-1);
	      wgp->no_d_last = 1;
	    }
	  else
	    wgp->no_d_last = 1;
	}
      gsb_set_positions(gswp);
    }
}

void
gsb_c_last(struct gsb_word *gswp)
{
  if (gswp->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(gswp);
      wgp->c_position = 'e';
    }
}

void
gsb_punct(struct gsb_word *gswp, const char *t)
{
  if (gswp->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(gswp);
      if ('*' == *t)
	wgp->roletype = 'b';
      else if (':' == *t)
	wgp->roletype = 'd';    
    }
}

void
gsb_sign(struct gsb_word *gswp, const char *t)
{
  if (gswp->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(gswp);
      gsb_strcpy(wgp->form, t);
    }
}

void
gsb_value(struct gsb_word *gswp, const char *t)
{
  if (gswp->gpp_used > 0)
    {
      Gsig *wgp = gsb_get(gswp);
      gsb_strcpy(wgp->value, t);
      gsb_strcpy(wgp->form, t);
    }
}

void
gsb_show(FILE *tab, struct gsb_word *gswp, int with_form)
{
  int i;
  for (i = 0; i < gswp->gpp_used; ++i)
    {
      Gsig *wgp = gsb_get_n(gswp, i);
      gsig_print(tab, wgp, "\t");
      if (with_form)
	fprintf(tab, "\t%s", gswp->form);
      fputc('\n', tab);
    }
}
