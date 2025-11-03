#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static int ent_cmp(const void *a, const void *b)
{
  Bibentry *e1 = (*(Bibentry**)a);
  Bibentry *e2 = (*(Bibentry**)b);
  int i;
  for (i = 0; e1->names[i] && e2->names[i]; ++i)
    {
      int c = strcmp(e1->names[i]->nkey, e2->names[i]->nkey);
      if (c)
	return c;
    }
  if (e1->names[i])
    return 1;
  else if (e2->names[i])
    return -1;
  
  return e1->year - e2->year;
}

static int nam_cmp(const void *a, const void *b)
{
  Name *n1 = (*(Name**)a);
  Name *n2 = (*(Name**)b);
  int cmp = strcmp(n1->last, n2->last);
  if (cmp)
    return cmp;
  return strcmp(n1->nkey, n2->nkey);
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
      if (!strcmp(bp->people[i]->last, bp->people[i+1]->last))
	bp->people[i]->sames = bp->people[i+1]->sames = 1;
    }
}

static void
add_people(Hash *people, Name **nn)
{
  int i;
  for (i = 0; nn[i]; ++i)
    hash_add(people, (uccp)nn[i]->nkey, nn[i]);
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
      else
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
