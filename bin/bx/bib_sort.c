#include <oraccsys.h>
#include <uninorm.h>
#include "bx.h"
#include "bib.h"

/*
  int u8_normcmp (const uint8_t *s1, size_t n1, const uint8_t *s2, size_t n2,
  uninorm_t nf, int *resultp)
*/

static int ent_cmp(const void *a, const void *b)
{
  Bibentry *e1 = (*(Bibentry**)a);
  Bibentry *e2 = (*(Bibentry**)b);
  Name **n1 = NULL, **n2 = NULL;

  if (e1->nm_author)
    {
      n1 = e1->nm_author->names;
      if (e2->nm_author)
	n2 = e2->nm_author->names;
      else if (e2->nm_editor)
	n2 = e2->nm_editor->names;
    }
  else if (e1->nm_editor)
    {
      n1 = e1->nm_editor->names;
      if (e2->nm_author)
	n2 = e2->nm_author->names;
      else if (e2->nm_editor)
	n2 = e2->nm_editor->names;
    }
  else if (e2->nm_author)
    n2 = e2->nm_author->names;
  else if (e2->nm_editor)
    n2 = e2->nm_editor->names;
  
  if (n1 && n2)
    {
      int i;
      int c = 0;
      for (i = 0; n1[i] && n2[i]; ++i)
	{
	  int res = 0;/*strcmp(n1[i]->nkey, n2[i]->nkey)*/
	  if (n1[i]->nkey && n2[i]->nkey)
	    {
	      res = u8_normcmp((uccp)n1[i]->nkey, strlen(n1[i]->nkey),
			       (uccp)n2[i]->nkey, strlen(n2[i]->nkey),
			       UNINORM_NFD, &c);
	      if (-1 == res)
		fprintf(stderr, "bib_sort: u8_normcmp returned error\n");
	    }
	  else if (n1[i]->nkey)
	    c = 1;
	  else if (n2[i]->nkey)
	    c = -1;
	  if (c)
	    return c;
	}
    }
  else if (n1)
    return 1;
  else if (n2)
    return -1;
  
  if (e1->year != e2->year)
    return e1->year - e2->year;
  else if (e1->fields[f_number] && e2->fields[f_number])
    return strcmp(e1->fields[f_number]->data, e2->fields[f_number]->data);

  return 0;
}

static int nam_cmp(const void *a, const void *b)
{
  Name *n1 = (*(Name**)a);
  Name *n2 = (*(Name**)b);
  int cmp = 0;
  if (n1->last && n2->last)
    cmp = strcmp(n1->last, n2->last);
  else if (n1->last)
    cmp = 1;
  else if (n2->last)
    cmp = -1;
  if (cmp)
    return cmp;
  if (n1->nkey && n2->nkey)
    return strcmp(n1->nkey, n2->nkey);
  else if (n1->nkey)
    return 1;
  else if (n2->nkey)
    return -1;
  else
    return 0;
}

void
bib_sort(Bx *bp)
{
  if (list_len(bp->entries))
    {
      bp->ents = (struct bibentry **)list2array_c(bp->entries, &bp->nents);
      qsort(bp->ents, bp->nents, sizeof(Bibentry *), (cmp_fnc_t)ent_cmp);
      int i;
      for (i = 0; bp->ents[i]; ++i)
	bp->ents[i]->sort = i;
    }
}

/* For Oracc bib a name-key (last+initials) designates a person; if
   multiple people have the same name key they must be disambiguated
   in the input data somehow */
static void
bib_disamb_people(Bx *bp, Hash *people)
{
  bp->people = (Name **)hash_vals2(people, &bp->npeople);
  qsort(bp->people, bp->npeople, sizeof(Name *), (cmp_fnc_t)nam_cmp);
  int i, penult;
  for (i = 0, penult = bp->npeople-1; i < penult; ++i)
    {
      if (bp->people[i]->last && bp->people[i+1]->last
	  && !strcmp(bp->people[i]->last, bp->people[i+1]->last))
	bp->people[i]->sames = bp->people[i+1]->sames = 1;
    }
}

static const char *
bib_which_allnames(Bibentry *ep)
{
  return
    (ep->nm_author
     ? ep->nm_author->allnames
     : (ep->nm_editor
	? ep->nm_editor->allnames
	: (ep->nm_bookauthor
	   ? ep->nm_bookauthor->allnames
	   : (ep->nm_translator
	      ? ep->nm_translator->allnames
	      : ""))));
}

static void
add_people(Hash *peeps, Name **nn)
{
  if (nn)
    {
      int i;
      for (i = 0; nn[i]; ++i)
	hash_add(peeps, (uccp)nn[i]->nkey, nn[i]);
    }
}

static void
bib_add_people(Hash *peeps,Bibentry *ep)
{
  if (ep->nm_author)
    add_people(peeps, ep->nm_author->names);
  if (ep->nm_editor)
    add_people(peeps, ep->nm_editor->names);
  if (ep->nm_bookauthor)
    add_people(peeps, ep->nm_bookauthor->names);
  if (ep->nm_translator)
    add_people(peeps, ep->nm_translator->names);
}

/* disambiguation is only done when sorting is done */
void
bib_disambiguate(Bx *bp)
{
  if (!bp->ents)
    return;
  
  int i, penult;
  Hash *hpeople = hash_create(1024);
  for (i = 0, penult=bp->nents-1; i < penult; ++i)
    {
      bib_add_people(hpeople,bp->ents[i]);
      const char *this_allnames = bib_which_allnames(bp->ents[i]);
      const char *next_allnames = bib_which_allnames(bp->ents[i+1]);
      
      if (!strcmp(this_allnames, next_allnames))
	{
	  bp->ents[i+1]->sameauth = 1;
	  if (bp->ents[i]->year == bp->ents[i+1]->year)
	    {
	      if (!bp->ents[i]->disamb)
		bp->ents[i]->disamb = 1;
	      bp->ents[i+1]->disamb = bp->ents[i]->disamb + 1;
	    }
	}
    }
  /* i == penult so add last people */
  bib_add_people(hpeople, bp->ents[i]);

  bib_disamb_people(bp, hpeople);
}
