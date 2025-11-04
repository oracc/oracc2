#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static int ent_cmp(const void *a, const void *b)
{
  Bibentry *e1 = (*(Bibentry**)a);
  Bibentry *e2 = (*(Bibentry**)b);
  Name **n1, **n2;

  if (e1->names)
    {
      n1 = e1->names;
      if (e2->names)
	n2 = e2->names;
      else if (e2->enames)
	n2 = e2->enames;
    }
  else
    {
      n1 = e1->enames;
      n2 = e2->names;
      if (!n2)
	n2 = e2->enames;
    }
  
  if (n1 && n2)
    {
      int i;
      int c = 0;
      for (i = 0; n1[i] && n2[i]; ++i)
	{
	  if (n1[i]->nkey && n2[i]->nkey)
	    c = strcmp(n1[i]->nkey, n2[i]->nkey);
	  else if (n1[i]->nkey)
	    c = -1;
	  else if (n2[i]->nkey)
	    c = 1;
	  if (c)
	    break;
	}
      if (c)
	return c;
    }
  else if (n1)
    return 1;
  else if (n2)
    return -1;
  
  return e1->year - e2->year;
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
  bp->ents = (struct bibentry **)list2array_c(bp->entries, &bp->nents);
  qsort(bp->ents, bp->nents, sizeof(Bibentry *), (cmp_fnc_t)ent_cmp);
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

static void
add_people(Hash *people, Name **nn)
{
  if (nn)
    {
      int i;
      for (i = 0; nn[i]; ++i)
	hash_add(people, (uccp)nn[i]->nkey, nn[i]);
    }
}

/* disambiguation is only done when sorting is done */
void
bib_disambiguate(Bx *bp)
{
  int i, penult;
  Hash *people = hash_create(1024);
  for (i = 0, penult=bp->nents-1; i < penult; ++i)
    {
      if (bp->ents[i]->fields[f_author])
	add_people(people, bp->ents[i]->names);
      else if (bp->ents[i]->fields[f_editor])
	add_people(people, bp->ents[i]->enames);

      if (!strcmp(bp->ents[i]->allnames, bp->ents[i+1]->allnames))
	{
	  bp->ents[i+1]->sameauth = 1;
	  if (bp->ents[i]->year == bp->ents[i+1]->year)
	    bp->ents[i+1]->disamb = bp->ents[i]->disamb + 1;
	}
    }
  /* i == penult so add last people */
  if (bp->ents[i]->fields[f_author])
    add_people(people, bp->ents[i]->names);
  else
    add_people(people, bp->ents[i]->enames);

  bib_disamb_people(bp, people);
}
