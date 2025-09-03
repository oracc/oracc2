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

static const char *ligs = NULL;

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

/* Check the arg List_node to see if it is the first char of a
   ligature; if so, look along the list and see if there is a complete
   ligature here.  If so, wrap it up as a unit and return the 'next'
   member of the last List_node that is part of the ligature.  If not,
   return the 'next' node of the arg List_node
 */
List_node *
cfy_lig_check(Cfy *c, List_node *lnp)
{
  unsigned char lchars[(LIG_MAX * CHAR_MAX) + 1];
  int i, hval;
  List_node *lnp_init = lnp;

  strcpy((char*)lchars, (ccp)elt_cun(lnp));
  for (i = 0; strlen((char*)lchars) < (LIG_MAX * (CHAR_MAX-1)) + 1; ++i)
    {
      if ((hval = (uintptr_t)hash_find(curr_cp->lig[i], (uccp)lchars)))
	{
	  if (hval == 1)
	    {
	      List_node *lnp_end = lnp;
	      elt_cun(lnp_init) = (void*)hpool_copy(lchars, c->hp);
	      lnp = lnp_init;
	      Btype b_init = elt_btype(lnp), new_b_init = BRK_NONE;
	      while (lnp < lnp_end)
		{
		  if (elt_btype(lnp) != b_init && new_b_init == BRK_NONE)
		    new_b_init = BRK_HASH;
		  elt_etype(lnp) = ELT_D;
		}
	      return lnp_end->next;
	    }
	  else if (lnp->next && elt_grapheme(lnp->next))
	    {
	      lnp = lnp->next;
	      strcat((char*)lchars, (ccp)elt_cun(lnp));
	    }
	  else
	    break;
	}
      else
	break;
    }
  return lnp->next;
}

/* Traverse the list of elements that will be output looking for
 * matches to the ligatures hash.  For any match, concatenate the
 * components to the first list node and reset the breakage state if
 * there is more than one Btype in the ligature's graphemes.
 */
void
cfy_lig_line(Cfy *c, List *lp)
{
  List_node *r = lp->first;
  while (r)
    {
      if (elt_grapheme(r))
	r = cfy_lig_check(c, r);
      else
	r = r->next;
    }
}

Hash **
cfy_lig_load(const char *ligfile)
{
  if (!access(ligfile, R_OK))
    {
      ligs = ligfile;
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
