#include <oraccsys.h>
#include "cfy.h"
#include "ligmax.h"

/* Cuneify ligatures are stored files $ORACC/lib/data/FONTNAME.lig
 *
 * The format is one ligature per line, and each ligature is given as
 * a series of path elements which can be hashed in turn to arrived at
 * a terminal sequence indicating the presence of the ligature in the
 * font.
 *
 * A ligature ABC is given in the
 * .lig file as
 *
 * 	a ab abc
 *
 * There is an array of hashes and each path segment is added to the
 * next hash in the array.  For non-terminal segments, the hash value
 * is -1; for terminal segments the hash value is 1.
 *
 * Cuneify has a maximum of 10 characters in a ligature but this could
 * be increased if necessary.
 */

static void
cfy_lig_breakage(Cfy*c, Elt **epp, int i, int j)
{
  int k;
  Btype b = epp[i]->btype;
  for (k = i+1; k <= j; ++k)
    {
      if (epp[k]->btype != b)
	b = BRK_HASH;
    }
  for (k = i; k <= j; ++k)
    {
      epp[k]->btype = b;
      if (k > i)
	epp[k]->etype = ELT_D;
    }
}

static int
cfy_lig_check(Cfy *c, Elt **epp, int i)
{
  Class *cp = epp[i]->c;
  char l[LIG_MAX*6];
  int j = i, k = 1;
  strcpy(l, epp[j]->data);
  List *l_ids = list_create(LIST_SINGLE);
  while (epp[++j])
    {
      if (epp[j]->etype == ELT_G)
	{
	  int hval;
	  strcat(l, epp[j]->data);
	  list_add(l_ids, (void*)epp[j]->xid);
	  if (1 == (hval = (intptr_t)hash_find(cp->lig[k++], (uccp)l)))
	    {
	      epp[i]->data = pool_copy((uccp)l, c->p);
	      epp[i]->xid = (ccp)list_to_str2(l_ids, "+");
	      list_free(l_ids, NULL);
	      cfy_lig_breakage(c, epp, i, j);
	      return j+1;
	    }
	  /* max length means failure */
	  if (k > LIG_MAX)
	    {
	      list_free(l_ids, NULL);
	      return i+1;
	    }
	}
    }
  list_free(l_ids, NULL);
  return i+1;
}

void
cfy_ligatures(Cfy*c, Elt **epp)
{
  int i = 0;
  while (epp[i])
    {
      if (epp[i]->etype == ELT_G
	  && epp[i]->c->lig
	  && hash_find(epp[i]->c->lig[0], epp[i]->data))
	i = cfy_lig_check(c, epp, i);
      else
	++i;
    }
}

int hsizes[LIG_MAX] = { 1024 , 512 , 256 , 128 , 64 , 32 , 16 , 16 , 16 , 16 };

static void
cfy_lig_hash(const char *ligfile, int lnum, Hash **hligs, Uchar *lp)
{
  Uchar *s;
  int i;
  Uchar orig[strlen((ccp)lp)+1];

  strcpy((char*)orig, (ccp)lp);

  for (s = lp, i = 0; *s; ++i)
    {
      if (i > LIG_MAX)
	{
	  /* This lig will never have a terminal state (hash value==1)
	     so it can never be actioned */
	  fprintf(stderr, "cfy: %s:%d: ligature %s with > %d characters will be ignored\n",
		  ligfile, lnum, orig, LIG_MAX);
	  return;
	}
      else
	{
	  if (!hligs[i])
	    hligs[i] = hash_create(hsizes[i]);

	  Uchar *sp = (ucp)strchr((ccp)s, ' ');
	  if (!sp)
	    {
	      hash_add(hligs[i], s, (void*)(uintptr_t)1);
	      break;
	    }
	  else
	    {
	      *sp = '\0';
	      hash_add(hligs[i], s, (void*)(uintptr_t)-1);
	      s = sp+1;
	    }
	}
    }
}

Hash **
cfy_lig_load(const char *ligfile)
{
  if (!access(ligfile, R_OK))
    {
      size_t nl;
      uchar **l = loadfile_lines((uccp)ligfile, &nl);
      int i;
      Hash **hl = calloc(LIG_MAX, sizeof(Hash*));
      for (i = 0; i < nl; ++i)
	cfy_lig_hash(ligfile, i, hl, l[i]);
      return hl;
    }
  return NULL;
}
