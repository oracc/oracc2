#include <stdlib.h>
#include <wctype.h>
#include <memo.h>
#include <pool.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>
#include "gdl.h"
#include "gvl.h"
#define GDLLTYPE Mloc
#include "gdl.tab.h"

#define G_C10E_MIXED_CASE 0x02
#define G_C10E_FINAL_SUBX 0x04

static Hash *legacy_reported_h = NULL;
extern const char *curr_pqx;
extern int curr_pqx_line;

static void gdl_legacy_brackets(char *b);

/* Legacy bracketing
 * =================
 *
 * ATF does not allow square and half-brackets to occur within
 * graphemes. If the protocol #atf: use legacy is given this rule is
 * bent and the input is preprocessed so that the internal bracketing
 * is handled as if it were the equivalent external bracketing, and
 * the cleaned input is passed to the parser.  The parser, gdl.y, does
 * not care about square and half brackets.
 *
 * The lexer is configured to ensure that graphemes begin and end with
 * non-bracket characters, so the only conditions we need to consider
 * are []⸢⸣⸤⸥ occurring within a grapheme.
 *
 * The following rules are applied to the previous/current/next
 * grapheme extant or pending states (pgst/curr/ngst: [=currently open
 * square; #=currently open half):
 *
 *     type    		pgst 	curr 	ngst
 *   c ^...]...   	c]      #       -
 *   c ^...⸣...   	c⸢      #       -
 *   c ^...⸥...   	c⸥      #       -
 *   o ...[...$		o[      #	o
 *   o ^...⸢...   	o⸢      #       -
 *   o ^...⸤...   	o⸥      #       -
 *   h ...[...]...	-       #	-
 *
 *   o=opener; c=closer; h=hash
 *
 *   More than one of these conditions can occur in a single grapheme.
 *   In combinations of c/o and h, the h is irrelevant because the c/o
 *   already set curr's break state to h.  If both c and o occur, both
 *   rules are applied, to the pgst and ngst respectively.
 */

unsigned char *
gdl_unlegacy_str(Mloc *mlp, unsigned const char *g)
{
  unsigned char *ret = NULL;
  wchar_t *w;
  size_t len;
  int i;
  int suppress_case_check = 0;

  if ((w = utf2wcs(g, &len)))
    {
      char brackets[len], *b = brackets;
      wchar_t *x = malloc(3*len*sizeof(wchar_t));
      wchar_t cued_sub_23 = 0;
      size_t xlen = 0;
      int found_l = 0;
      int found_u = 0;
      
      for (i = 0; i < len; ++i)
	{
	  switch (w[i])
	    {
	    case '@':
	    case '~':
	    case '\\':
	      if (w[i+1] && iswalpha(w[i+1]) && iswlower(w[i+1]))
		suppress_case_check = 1;
	    case '|':
	    case '.':
	    case U_times:
	    case '-':
	    case ':':
	    case '+':
	    case '&':
	    case '%':
	    case '(':
	    case ')':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      x[xlen++] = w[i];
	      break;
	    case '*':
	      /* In initial position this is a bullet and must be
		 kept; otherwise it's a collation flag and is dropped */
	      if (0 == i)
		x[xlen++] = '*';
	      break;
	    case '[':
	      *b++ = '[';
	      break;
	    case ']':
	      *b++ = ']';
	      break;
	    case U_ulhsq:
	      *b++ = 'L';
	      break;
	    case U_urhsq:
	      *b++ = 'R';
	      break;
	    case U_llhsq:
	      *b++ = 'l';
	      break;
	    case U_lrhsq:
	      *b++ = 'r';
	      break;
	    case '#':
	    case '?':
	    case '!':
	    case U_cdot:
	    case U_degree:
	      /* delete these */
	      break;
	    case '<':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      if (w[i+1] == '(')
		{
		  x[xlen++] = '<';
		  x[xlen++] = '(';
		  ++i;
		}
	      break;
	    case '>':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      if (w[i-1] == ')')
		x[xlen++] = '>';
	      break;
	    case U_heth:
	      x[xlen++] = 'h';
	      ++found_l;
	      break;
	    case U_HETH:
	      x[xlen++] = 'H';
	      ++found_u;
	      break;
	    case '\'':
	      x[xlen++] = U_aleph;
	      break;
	    case a_acute:
	    case e_acute:
	    case i_acute:
	    case u_acute:
	    case a_grave:
	    case e_grave:
	    case i_grave:
	    case u_grave:
	      cued_sub_23 = g_subdig_of(w[i]);
	      x[xlen++] = g_vowel_of(w[i]);
	      ++found_l;
	      break;
	    case A_acute:
	    case E_acute:
	    case I_acute:
	    case U_acute:
	    case A_grave:
	    case E_grave:
	    case I_grave:
	    case U_grave:
	      cued_sub_23 = g_subdig_of(w[i]);
	      x[xlen++] = g_vowel_of(w[i]);
	      ++found_u;
	      break;
	    case U_s_x:
	      /* This block may be unnecessary with GVL */
	      x[xlen++] = w[i];
	      if ('(' != w[i+1])
		; /*err |= G_C10E_FINAL_SUBX;*/
	      break;
	    default:
	      if (iswalpha(w[i]) && !suppress_case_check)
		{
		  if (iswupper(w[i]))
		    ++found_u;
		  else
		    ++found_l;
		}
	      x[xlen++] = w[i];
	      break;
	    }
	}

      if (cued_sub_23)
	x[xlen++] = cued_sub_23;
      x[xlen] = 0;

      if (found_l && found_u && !suppress_case_check)
	{
	  /* size_t i; */
	  /*err |= G_C10E_MIXED_CASE;*/
#if 0
	  /* this may not be worth the problems it causes */
	  if (found_l > found_u)
	    for (i = 0; i < xlen; ++i)
	      x[i] = towlower(x[i]);
	  else
	    for (i = 0; i < xlen; ++i)
	      x[i] = towupper(x[i]);
#endif
	}
      ret = wcs2utf(x,xlen);
      free(x);
      *b = '\0';
      if (brackets[0])
	gdl_legacy_brackets(brackets);
    }
  return ret;
}

/* In legacy-bracketed graphemes there are three state variables that
 * must be handled at different times:
 *
 * last-grapheme-state
 * curr-grapheme-state
 * next-grapheme-state
 *
 * The first, last-grapheme-state, is the same as non-legacy
 * processing because it only affects closure states and they can be
 * applied directly to the last grapheme state on lgp.
 *
 * The other two need careful handling because they require a
 * double-pending structure: curr-grapheme-state needs to be applied
 * after the current grapheme is created; next-grapheme-state has to
 * be applied when the following grapheme is created.
 *
 * The argument is a simple char array where [] are lft/rt square; LR
 * are lft/rt upper-half-square; lr are lft/rt lower-half-square.
 *
 * Any adjacent open/close pair must match: [], ⸢⸣, ⸤⸥; it's easy to
 * check these and report errors, so we do that first.
 *
 * Any brackets at all automatically mean that the curr-grapheme-state
 * pending has to be set to hash-damage.
 *
 * If the arg array starts with a closer, that has to be put back on
 * last-grapheme-state.
 *
 * If the arg array ends with an opener, then the next-grapheme-state
 * pending (the double-pending) has to be set to the opener.
 */

static void
gdl_legacy_brackets(char *b)
{
  char *s = b;
  /* Check for paired opener-closer and if they are matched reduce to
   * '--'; error and abort if they are mismatched
   */
  while (*s)
    {
      if ('[' == *s)
	{
	  if (']' == s[1])
	    {
	      *s = s[1] = '-';
	      s += 2;
	    }
	  else if ('R' == s[1] || 'r' == s[1])
	    {
	      mesg_verr(&gdllloc, "mismatched bracketing ignored (reported once per grapheme)");
	      return;
	    }
	  else
	    ++s;
	}
      else if ('L' == *s)
	{
	  if ('R' == s[1])
	    {
	      *s = s[1] = '-';
	      s += 2;
	    }
	  else if (']' == s[1] || 'r' == s[1])
	    {
	      mesg_verr(&gdllloc, "mismatched bracketing ignored (reported once per grapheme)");
	      return;
	    }
	  else
	    ++s;
	}
      else if ('l' == *s)
	{
	  if ('r' == s[1])
	    {
	      *s = s[1] = '-';
	      s += 2;
	    }
	  else if (']' == s[1] || 'R' == s[1])
	    {
	      mesg_verr(&gdllloc, "mismatched bracketing ignored (reported once per grapheme)");
	      return;
	    }
	  else
	    ++s;
	}
      else
	++s;
    }

  
  /* If the remaining brackets have two openers or two closers in a
   * row that is also an error * If there are matching opener/closers
   * they must be nested around the previous set; this is also an error
   */
  s = b;
  while (*s)
    {
      if ('[' == *s || 'L' == *s || 'l' == *s)
	{
	  char *n = s+1;
	  while (*n && '-' == *n)
	    ++n;
	  if (*n)
	    {
	      if ('[' == *n || 'L' == *n || 'l' == *n)
		{
		  mesg_verr(&gdllloc, "double-opener; bracketing ignored (reported once per grapheme)");
		  return;
		}
	      else if (']' == *n || 'R' == *n || 'r' == *n)
		{
		  mesg_verr(&gdllloc, "nested pairs; bracketing ignored (reported once per grapheme)");
		  return;
		} 
	    }
	  else
	    ++s;
	}
      else if (']' == *s || 'R' == *s || 'r' == *s)
	{
	  char *n = s+1;
	  while (*n && '-' == *n)
	    ++n;
	  if (*n)
	    {
	      if (']' == *n || 'R' == *n || 'r' == *n)
		{
		  mesg_verr(&gdllloc, "double-closer; bracketing ignored (reported once per grapheme)");
		  return;
		}
	    }
	  else
	    ++s;
	}
      else
	++s;
    }

  /* always set up the current-pending as if a #-flag (hash) follows
   * the current grapheme */
  gdl_break_h_l();

  s = b;
  while (*s && '-' == *s)
    ++s;

  /* if the first bracket is a closer apply that to the last grapheme */
  if (']' == *s || 'R' == *s || 'r' == *s)
    {
      gdl_break_c_l(']' == *s ? e_R_squ : ('R' == *s ? e_R_uhs : e_R_lhs));
      ++s; /* point at char after closer */
    }
  else
    s = b;

  char *t = b + strlen(b);
  while (t > b && '-' == t[-1])
    --t;

  if ('[' == *t || 'L' == *t || 'l' == *t)
    {
      gdl_break_o_l('[' == *t ? e_L_squ : ('L' == *t ? e_L_uhs : e_L_lhs));
      if (t > s)
	--t; /* point at char before opener */
    }

  /* check if there were any unused brackets between the closer, if
     any, and the opener */
  while (s <= t)
    {
      if ('-' != *s)
	{
	  mesg_verr(&gdllloc, "unused brackets between closer and opener");
	  return;
	}
    }
}

#if 0
/* 20260618: no longer used now that unlegacy is handled completely in
   the lexer maw and not deferred until after the grammar sees it */

/* if gdl_legacy is set (normally via #atf: use legacy) this routine
   stores the value of np->text in a property named 'legacy' and
   replaces np->text with a cleaned version without the brackets. This
   means that GVL never sees legacy bracketed data */
void
gdl_unlegacy(Node *np)
{
  unsigned char *res = NULL;
  
  /* While this is only called from gdl on 's' nodes, np can only have
     kids if it is a sign+mod in which case the sign has already been
     through gdl_unlegacy */
  if (np->kids && np->kids->props && prop_find_kv(np->kids->props, "legacy", NULL))
    {
      np->text = np->kids->text;
      return;
    }

  if ((res = gdl_unlegacy_str(np->mloc, (uccp)np->text)))
    {
      if (strcmp(np->text, (ccp)res))
	{
	  prop_node_add(np, GP_TRACKING, PG_GDL_INFO, "legacy", np->text);
	  np->text = (ccp)pool_copy(res, np->tree->tm->pool);
	}
    }
  else
    mesg_verr(np->mloc, "gdl_unlegacy failed to convert %s\n", np->text);
}
#endif

int
gdl_legacy_check(Node *ynp, unsigned const char *t)
{
  int need_legacy = 0;

  if (gdl_legacy)
    return 0;
  
  if (ynp == NULL && t == NULL && legacy_reported_h)
    {
      hash_free(legacy_reported_h, NULL);
      legacy_reported_h = NULL;
      return 0;
    }

  if (!legacy_reported_h)
    legacy_reported_h = hash_create(1);
  
  if (curr_pqx)
    {
      if (!hash_find(legacy_reported_h, (uccp)curr_pqx))
	{
#if 0
	  unsigned char *res = gdl_unlegacy_str(ynp->mloc, t);
	  if (strcmp((ccp)res, (ccp)t))
	    {
#endif
	      Mloc m = *ynp->mloc;
	      m.line = curr_pqx_line;
	      mesg_verr(&m, "Text %s needs '#atf: use legacy'", curr_pqx);
	      hash_add(legacy_reported_h, (uccp)curr_pqx, "");
	      need_legacy = 1;
#if 0
	    }
#endif
	}
      else
	need_legacy = 1;
    }
  return need_legacy;
}
