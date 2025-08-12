#include <oraccsys.c>

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
 * Cuneify has a maximum of 10 characters in a ligature but this can
 * be increased if necessary.
 */

/* no Oracc ligature can be more characters than this */
#define LIG_MAX 10

static const char *ligs = NULL;

Hash *hligs[LIG_MAX];

int hsizes[LIG_MAX] = { 1024 , 512 , 256 , 128 , 128 , 128 , 128 , 128 , 128 , 128 };

static void
cfy_lig_hash(Uchar *lp)
{
  Uchar *s;
  int i;
  for (s = lp, i = 0; *s; ++i)
    {
      if (i > LIG_MAX)
	{
	  /* This lig will never have a terminal state (hash value==1)
	     so it can never be actioned */
	  fprintf(stderr, "%s:%d: ligature > %d characters will be ignored\n",
		  ligfile, i+1, LIG_MAX);
	  return;
	}
      else
	{
	  if (!hligs[i])
	    hligs[i] = hash_create(hsizes[i]);

	  Uchar *sp = strchr(s, ' ');
	  if (sp)
	    {
	      hash_add(hligs[i], 1);
	      s = sp;;
	    }
	  else
	    {
	      *sp = '\0';
	      hash_add(hligs[i], -1);
	      s = sp+1;
	    }
	}
    }
}

void
cfy_lig_check(List *lp, List **resumep)
{
  unsigned char lchars[(LIG_MAX * CHAR_MAX) + 1];
  int i, hval;
  List *lp_init = lp;

  strcpy(lchars, elt_u8(lp);
  for (i = 0; strlen(lchars) < (LIG_MAX * (CHAR_MAX-1)) + 1; ++i)
    {
      if ((hval = hash_find(hligs[i], lchars)))
	{
	  if (hval == 1)
	    {
	      List *lp_end = lp;
	      elt_u8(lp_init) = hpool_copy(lchars);
	      lp = lp->init->next;
	      enum Btype b_init = elt_btype(lp), new_b_init = BRK_CLEAR;
	      while (lp < lp_end)
		{
		  if (elt_btype(lp) != b_init && new_b_init == BRK_CLEAR)
		    new_b_init = B_HASH;
		  elt_etype(lp) = ELT_D;
		}
	      *resumep = lp_end;
	      break;
	    }
	  else if (lp->next && elt_grapheme(lp->next))
	    {
	      lp = lp->next;
	      strcat(lchars, elt_u8(lp));
	    }
	  else
	    break;
	}
      else
	break;
    }
}

/* Traverse the list of elements that will be output looking for
 * matches to the ligatures hash.  For any match, concatenate the
 * components to the first list node and reset the breakage state if
 * there is more than one Btype in the ligature's graphemes.
 */
void
cfy_lig_line(List *lp)
{
  while (lp->next)
    {
      if (elt_grapheme(lp))
	cfy_lig_check(lp);
      lp = lp->next;
    }
}

void
cfy_lig_load(const char *ligfile)
{
  if (!access(ligfile, R_OK))
    {
      ligs = ligfile;
      size_t nl;
      uchar **l = loadfile_lines((uccp)ligfile, &nl);
      int i;
      cfy_lig_init();
      for (i = 0; i < nl; ++i)
	cfy_lig_hash(l[i]);
    }
}
